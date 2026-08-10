#pragma once

#include "Wiwiworkspace.hpp"
#include "music/Music_disc.hpp"
#include "util/base64.hpp"
#include <memory>
#include <nlohmann/json.hpp>
#include <saucer/smartview.hpp>
#include <string>
#include <vector>

namespace wiwi::bridge {

class Music_disc_bridge {
public:
  static std::shared_ptr<Music_disc>
  find_disc(const std::shared_ptr<Wiwiworkspace> &workspace,
            const std::string &id) {
    if (!workspace)
      return nullptr;
    for (const auto &d : workspace->get_music_discs()) {
      if (d->string_id() == id)
        return d;
    }
    return nullptr;
  }

  static void bind(saucer::smartview *webview,
                   std::shared_ptr<Wiwiworkspace> &workspace) {
    webview->expose("disc_set_title", [&workspace](const std::string &id,
                                                   const std::string &title) {
      auto d = find_disc(workspace, id);
      if (d)
        d->set_title(title);
    });

    webview->expose("disc_set_artist", [&workspace](const std::string &id,
                                                    const std::string &artist) {
      auto d = find_disc(workspace, id);
      if (d)
        d->set_artist(artist);
    });

    webview->expose(
        "disc_set_cover_from_bytes",
        [&workspace](const std::string &id, const std::vector<uint8_t> &bytes) {
          auto d = find_disc(workspace, id);
          if (d)
            d->set_cover(Image_data{bytes});
        });

    webview->expose(
        "disc_set_original_cover_from_bytes",
        [&workspace](const std::string &id, const std::vector<uint8_t> &bytes) {
          auto d = find_disc(workspace, id);
          if (d)
            d->set_original_cover(Image_data{bytes});
        });

    webview->expose("disc_set_has_stonecutter_recipe",
                    [&workspace](const std::string &id, bool has_recipe) {
                      auto d = find_disc(workspace, id);
                      if (d)
                        d->set_has_stonecutter_recipe(has_recipe);
                    });

    webview->expose("disc_set_comparator_output",
                    [&workspace](const std::string &id, size_t output) {
                      auto d = find_disc(workspace, id);
                      if (d)
                        d->set_comparator_output(output);
                    });

    webview->expose("disc_set_silence_gap",
                    [&workspace](const std::string &id, float gap) {
                      auto d = find_disc(workspace, id);
                      if (d)
                        d->set_silence_gap(gap);
                    });

    webview->expose("disc_set_trim", [&workspace](const std::string &id,
                                                  float start, float end) {
      auto d = find_disc(workspace, id);
      if (d)
        d->set_trim(start, end);
    });

    webview->expose("disc_get_trim_start",
                    [&workspace](const std::string &id) -> float {
                      auto d = find_disc(workspace, id);
                      return d ? d->get_trim_start() : 0.0f;
                    });

    webview->expose("disc_get_trim_end",
                    [&workspace](const std::string &id) -> float {
                      auto d = find_disc(workspace, id);
                      return d ? d->get_trim_end() : 0.0f;
                    });

    webview->expose("disc_get_duration_seconds",
                    [&workspace](const std::string &id) -> float {
                      auto d = find_disc(workspace, id);
                      return d ? d->get_duration_seconds() : 0.0f;
                    });

    webview->expose("disc_get_trimmed_duration",
                    [&workspace](const std::string &id) -> float {
                      auto d = find_disc(workspace, id);
                      return d ? d->get_trimmed_duration() : 0.0f;
                    });

    webview->expose("disc_is_cover_modified",
                    [&workspace](const std::string &id) -> bool {
                      auto d = find_disc(workspace, id);
                      return d ? d->is_cover_modified() : false;
                    });

    webview->expose("disc_has_original_cover",
                    [&workspace](const std::string &id) -> bool {
                      auto d = find_disc(workspace, id);
                      return d ? !d->get_original_cover().empty() : false;
                    });

    webview->expose("disc_remove_cover", [&workspace](const std::string &id) {
      auto d = find_disc(workspace, id);
      if (d)
        d->set_cover(Image_data{});
    });

    webview->expose("disc_restore_original_cover",
                    [&workspace](const std::string &id) {
                      auto d = find_disc(workspace, id);
                      if (d)
                        d->restore_original_cover();
                    });

    webview->expose("disc_get_waveform", [&workspace](const std::string &id) {
      auto d = find_disc(workspace, id);
      nlohmann::json res = nlohmann::json::array();
      if (d) {
        for (float v : d->samples(100)) {
          res.push_back(v);
        }
      }
      return res.dump();
    });

    webview->expose(
        "disc_get_ogg_audio_data",
        [&workspace](const std::string &id) -> std::vector<uint8_t> {
          auto d = find_disc(workspace, id);
          return d ? d->get_ogg_audio_data() : std::vector<uint8_t>{};
        });

    webview->expose(
        "disc_set_pcm_audio",
        [&workspace](const std::string &id,
                     const std::vector<uint8_t> &left_bytes,
                     const std::vector<uint8_t> &right_bytes, int sample_rate) {
          auto d = find_disc(workspace, id);
          if (d) {
            const float *left_ptr =
                reinterpret_cast<const float *>(left_bytes.data());
            size_t left_size = left_bytes.size() / sizeof(float);
            std::vector<float> left_vec(left_ptr, left_ptr + left_size);

            std::vector<float> right_vec;
            if (!right_bytes.empty()) {
              const float *right_ptr =
                  reinterpret_cast<const float *>(right_bytes.data());
              size_t right_size = right_bytes.size() / sizeof(float);
              right_vec.assign(right_ptr, right_ptr + right_size);
            }

            d->set_pcm_audio(std::move(left_vec), std::move(right_vec),
                             sample_rate);
          }
        });

    webview->expose("disc_get_cover_url",
                    [&workspace](const std::string &id) -> std::string {
                      auto d = find_disc(workspace, id);
                      if (!d)
                        return "";
                      auto png = d->get_disc_item_image().encode_png();
                      return "data:image/png;base64," + base64_encode(png);
                    });
  }
};

} // namespace wiwi::bridge
