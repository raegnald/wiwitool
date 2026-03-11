#include "music/Music_disc.hpp"

#include "Image_data.hpp"

#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <format>
#include <print>

#include <vorbis/vorbisenc.h>
#include "stb_vorbis.h"

#include "util/wiwidebug.hpp"

Music_disc::Music_disc(void) : disc_id{next_disc_id++} {}

void Music_disc::set_cover(Image_data c) {
  cover = c.empty() ? Image_data{} : c.crop(1, 1).scale(256, 256);
}

std::string Music_disc::description(void) const {
  if (title.empty() and artist.empty())
    return "Music disc";

  if (artist.empty())
    return title;

  if (title.empty())
    return artist;

  return std::format("{} - {}", artist, title);
}

void Music_disc::set_pcm_audio(std::vector<float> &&left_channel,
                               std::vector<float> &&right_channel,
                               unsigned int rate) {
  sample_rate = rate;
  duration_seconds = static_cast<float>(left_channel.size()) / rate;

  // 1. Calculate and cache the 100Hz waveform immediately
  double samples_per_bucket = static_cast<double>(rate) / 100.0;
  size_t num_buckets = static_cast<size_t>(left_channel.size() / samples_per_bucket);

  cached_waveform.clear();
  cached_waveform.reserve(num_buckets);

  float global_max_rms = 0.0f;

  for (size_t i = 0; i < num_buckets; ++i) {
    size_t start_idx = static_cast<size_t>(i * samples_per_bucket);
    size_t end_idx = std::min(static_cast<size_t>((i + 1) * samples_per_bucket), left_channel.size());

    float sum_squares = 0.0f;
    size_t count = end_idx - start_idx;

    for (size_t j = start_idx; j < end_idx; ++j) {
      float val = left_channel[j];
      sum_squares += val * val; // square the amplitude
    }

    // Root Mean Square
    float rms = (count > 0) ? std::sqrt(sum_squares / count) : 0.0f;

    if (rms > global_max_rms) {
      global_max_rms = rms;
    }

    cached_waveform.push_back(rms);
  }

  // Normalize the waveform so the loudest part of the track fills 100% of the canvas height
  if (global_max_rms > 0.0f) {
    for (float& val : cached_waveform) {
      val /= global_max_rms;
    }
  }

  // 2. Immediately encode the raw PCM into OGG bytes and store it
  ogg_data = internal_vorbis_encode(left_channel, right_channel, rate);
}

std::string Music_disc::string_id(void) const {
  return std::format("music_disc_{}", disc_id);
}

std::vector<float> Music_disc::samples(unsigned int target_hz) const {
  return cached_waveform;
}

std::vector<uint8_t> Music_disc::encode_ogg(void) {
  wiwidebug std::println("Getting encoded OGG for {}", string_id());

  if (ogg_data.empty()) return {};

  // return cached OGG when no meaningful trim is applied
  if (trim_start <= 0.0f and (trim_end < 0.0f or trim_end >= duration_seconds))
    return ogg_data;

  // Decode, trim, and re-encode
  int channels, rate;
  short *decoded_audio;

  int num_samples = stb_vorbis_decode_memory(ogg_data.data(), ogg_data.size(),
                                             &channels, &rate, &decoded_audio);

  // If decode fails for some reason, return the untrimmed data as a
  // safe fallback
  if (num_samples <= 0) return ogg_data;

  // Calculate trim bounds in frames
  int start_frame =
      (trim_start >= 0.0f) ? static_cast<int>(trim_start * rate) : 0;
  int end_frame =
      (trim_end >= 0.0f) ? static_cast<int>(trim_end * rate) : num_samples;

  start_frame = std::clamp(start_frame, 0, num_samples);
  end_frame = std::clamp(end_frame, start_frame, num_samples);
  int trimmed_frames = end_frame - start_frame;

  if (trimmed_frames <= 0) {
    free(decoded_audio);
    return {};
  }

  // Allocate temp float arrays for the re-encoder
  std::vector<float> trimmed_left(trimmed_frames);
  std::vector<float> trimmed_right(channels == 2 ? trimmed_frames : 0);

  // De-interleave the short array and convert back to floats (-1.0 to 1.0)
  for (int i = 0; i < trimmed_frames; ++i) {
    int src_idx = (start_frame + i) * channels;
    trimmed_left[i] = decoded_audio[src_idx] / 32768.0f;

    if (channels == 2) {
      trimmed_right[i] = decoded_audio[src_idx + 1] / 32768.0f;
    }
  }

  free(decoded_audio);

  return internal_vorbis_encode(trimmed_left, trimmed_right, rate);
}

std::vector<uint8_t>
Music_disc::internal_vorbis_encode(const std::vector<float> &left,
                                   const std::vector<float> &right,
                                   unsigned int rate) {

  if (left.empty()) return {};

  int channels = right.empty() ? 1 : 2;
  size_t total_frames = left.size();

  std::vector<uint8_t> out_data;
  ogg_stream_state os;
  ogg_page         og;
  ogg_packet       op;
  vorbis_info      vi;
  vorbis_comment   vc;
  vorbis_dsp_state vd;
  vorbis_block     vb;

  vorbis_info_init(&vi);
  vorbis_encode_init_vbr(&vi, channels, rate, 0.4f);

  vorbis_comment_init(&vc);
  vorbis_comment_add_tag(&vc, "TITLE",
                         title.empty() ? "Unknown" : title.c_str());
  vorbis_comment_add_tag(&vc, "ARTIST",
                         artist.empty() ? "Unknown" : artist.c_str());
  vorbis_comment_add_tag(&vc, "ENCODER", "Wiwitool");

  vorbis_analysis_init(&vd, &vi);
  vorbis_block_init(&vd, &vb);
  ogg_stream_init(&os, disc_id + 98765);

  ogg_packet header, header_comm, header_code;
  vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
  ogg_stream_packetin(&os, &header);
  ogg_stream_packetin(&os, &header_comm);
  ogg_stream_packetin(&os, &header_code);

  while (ogg_stream_flush(&os, &og)) {
    out_data.insert(out_data.end(), og.header, og.header + og.header_len);
    out_data.insert(out_data.end(), og.body, og.body + og.body_len);
  }

  size_t frames_processed = 0;
  const size_t chunk_size = 4096;

  while (frames_processed < total_frames) {
    size_t current_chunk = std::min(chunk_size, total_frames - frames_processed);
    float **buffer = vorbis_analysis_buffer(&vd, current_chunk);

    for (size_t i = 0; i < current_chunk; ++i) {
      buffer[0][i] = left[frames_processed + i];
      if (channels == 2) buffer[1][i] = right[frames_processed + i];
    }

    vorbis_analysis_wrote(&vd, current_chunk);

    while (vorbis_analysis_blockout(&vd, &vb) == 1) {
      vorbis_analysis(&vb, nullptr);
      vorbis_bitrate_addblock(&vb);

      while (vorbis_bitrate_flushpacket(&vd, &op)) {
        ogg_stream_packetin(&os, &op);
        while (ogg_stream_pageout(&os, &og)) {
          out_data.insert(out_data.end(), og.header, og.header + og.header_len);
          out_data.insert(out_data.end(), og.body, og.body + og.body_len);
        }
      }
    }

    frames_processed += current_chunk;
  }

  vorbis_analysis_wrote(&vd, 0);

  while (vorbis_analysis_blockout(&vd, &vb) == 1) {
    vorbis_analysis(&vb, nullptr);
    vorbis_bitrate_addblock(&vb);
    while (vorbis_bitrate_flushpacket(&vd, &op)) {
      ogg_stream_packetin(&os, &op);
      while (ogg_stream_pageout(&os, &og) || ogg_stream_flush(&os, &og)) {
        out_data.insert(out_data.end(), og.header, og.header + og.header_len);
        out_data.insert(out_data.end(), og.body, og.body + og.body_len);
      }
    }
  }

  ogg_stream_clear(&os);
  vorbis_block_clear(&vb);
  vorbis_dsp_clear(&vd);
  vorbis_comment_clear(&vc);
  vorbis_info_clear(&vi);

  return out_data;
}

Image_data Music_disc::get_disc_item_image(void) {
  constexpr auto blend = 0.25;

  if (cover.empty())
    return Image_data{"disc.png"};

  auto cover_mask = Image_data{"disc-mask.png"};

  const auto downscaled = cover.scale(cover_mask.width(), cover_mask.height());

  for (size_t y = 0; y < cover_mask.height(); y++) {
    for (size_t x = 0; x < cover_mask.width(); x++) {
      if (cover_mask.at(x, y).a != 0) {
        const auto m = cover_mask.at(x, y);
        const auto d = downscaled.at(x, y);

        const bool should_skip =
            m.r == 255 and m.b == 255 and m.g == 255 and m.a == 255;

        cover_mask.at(x, y) = should_skip ? d : rgba::blend(m, d, blend);
      }
    }
  }

  return cover_mask;
}


#ifdef EMSCRIPTEN
#include <emscripten/bind.h>
#include <emscripten/val.h>

void Music_disc::set_audio_from_js(std::string left_bytes,
                                   std::string right_bytes, int rate) {
  const float *left_ptr = reinterpret_cast<const float *>(left_bytes.data());
  size_t left_size = left_bytes.size() / sizeof(float);
  std::vector<float> left_vec(left_ptr, left_ptr + left_size);

  std::vector<float> right_vec;
  if (not right_bytes.empty()) {
    const float *right_ptr =
        reinterpret_cast<const float *>(right_bytes.data());
    size_t right_size = right_bytes.size() / sizeof(float);
    right_vec.assign(right_ptr, right_ptr + right_size);
  }

  set_pcm_audio(std::move(left_vec), std::move(right_vec), rate);
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(music_disc) {
  class_<Music_disc>("MusicDisc")
      .smart_ptr<std::shared_ptr<Music_disc>>("MusicDisc")
      .constructor<>()

      .function("stringId", &Music_disc::string_id)

      .property("title", &Music_disc::get_title, &Music_disc::set_title)
      .property("artist", &Music_disc::get_artist, &Music_disc::set_artist)

      .function("setTrim", &Music_disc::set_trim)
      .function("getTrimStart", &Music_disc::get_trim_start)
      .function("getTrimEnd", &Music_disc::get_trim_end)

      .function("getDurationSeconds", &Music_disc::get_duration_seconds)

      .function("setCover", &Music_disc::set_cover)

      // Fast-path to ingest the cover image bytes directly into an
      // Image_data object
      .function(
          "setCoverFromBytes",
          optional_override([](Music_disc &self, std::string image_bytes) {
            const uint8_t *ptr =
                reinterpret_cast<const uint8_t *>(image_bytes.data());
            std::vector<uint8_t> vec(ptr, ptr + image_bytes.size());
            self.set_cover(Image_data{vec});
          }))

      .function("removeCover", optional_override([](Music_disc &self) {
                  self.set_cover(Image_data{});
                }))

      .function("getDiscItemImage", &Music_disc::get_disc_item_image)

      // Fast-path to ingest the raw audio from JS
      .function("setPcmAudio", &Music_disc::set_audio_from_js)

      // Zero-copy getter for the waveform
      .function("getWaveform", optional_override([](const Music_disc &self) {
                  const auto &data = self.samples(100);
                  return val(typed_memory_view(data.size(), data.data()));
                }))

      .function("getOggAudioData", optional_override([](Music_disc &self) {
                  auto data = self.get_ogg_audio_data();
                  return val::global("Uint8Array")
                      .new_(typed_memory_view(data.size(), data.data()));
                }))

      // Zero-copy getter for the finalized OGG bytes (used by
      // WebWorker/Generate)
      .function("encodeOgg", optional_override([](Music_disc &self) {
                  auto data = self.encode_ogg();
                  return val::global("Uint8Array")
                      .new_(typed_memory_view(data.size(), data.data()));
                }));
}
#endif
