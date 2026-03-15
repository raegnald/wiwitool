// A container and OGG encoder for a Minecraft music disc.

#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>
#include <cstdint>

#include "Image_data.hpp"

class Music_disc {
public:
  Music_disc(void);
  ~Music_disc(void) = default;

  std::string string_id(void) const;

  // Getters and setters for title, artist, and cover

  void set_title(std::string t) { title = t; }
  std::string get_title(void) const { return title; }

  void set_artist(std::string a) { artist = a; }
  std::string get_artist(void) const { return artist; }

  void set_cover(Image_data c);
  Image_data get_cover(void) const { return cover; }

  std::string description(void) const;

  void set_comparator_output(size_t x) { comparator_output = x > 15 ? 15 : x; }
  size_t get_comparator_output(void) const { return comparator_output; }

  // Audio data

  void set_pcm_audio(std::vector<float> &&left_channel,
                     std::vector<float> &&right_channel,
                     unsigned int sample_rate);

  std::vector<uint8_t> get_ogg_audio_data(void) const { return ogg_data; }

  unsigned int get_sample_rate(void) const { return sample_rate; }

  float get_duration_seconds(void) const { return duration_seconds; }

  std::vector<float> samples(unsigned int target_hz = 100) const;

  // Trimming

  void set_trim(float start_sec, float end_sec) { trim_start = start_sec; trim_end = end_sec; }
  float get_trim_start() const { return trim_start; }
  float get_trim_end() const { return trim_end; }

  float get_trimmed_duration(void) const {
    return (trim_end > 0) ? trim_end - trim_start : get_duration_seconds();
  }

  // Generation

  std::vector<uint8_t> encode_ogg(void); // trimmed

  Image_data get_disc_item_image(void);

  // Serialisation

  friend void to_json(nlohmann::json &, const Music_disc &);
  friend void from_json(const nlohmann::json &, Music_disc &);

#ifdef EMSCRIPTEN
  // Fast-path JS ingest
  void set_audio_from_js(std::string left_bytes, std::string right_bytes,
                         int rate);
#endif

private:
  static inline unsigned int next_disc_id{0};
  unsigned int disc_id;

  std::string title, artist;

  size_t comparator_output{15};

  Image_data cover; // cover.empty() means it'll use the default
                    // Minecraft disc image

  // Compressed baseline storage
  std::vector<uint8_t> ogg_data;
  std::vector<float> cached_waveform;

  unsigned int sample_rate{0};
  float trim_start{-1.0f}, trim_end{-1.0f}; // in seconds
  float duration_seconds{0.0f};

  // Internal helper to handle the heavy Vorbis math
  std::vector<uint8_t> internal_vorbis_encode(const std::vector<float> &left,
                                              const std::vector<float> &right,
                                              unsigned int rate);
};

void to_json(nlohmann::json &j, const Music_disc &);
void from_json(const nlohmann::json &, Music_disc &);
