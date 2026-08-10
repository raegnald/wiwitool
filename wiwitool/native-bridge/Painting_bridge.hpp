#pragma once

#include "Wiwiworkspace.hpp"
#include "paintings/Painting.hpp"
#include "paintings/Painting_ratio.hpp"
#include "util/base64.hpp"
#include <format>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <saucer/smartview.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace wiwi::bridge {

class Painting_bridge {
private:
  static inline std::unordered_map<std::string, std::vector<uint8_t>> cache_map;
  static inline std::mutex cache_mutex;

public:
  static std::shared_ptr<Painting>
  find_painting(const std::shared_ptr<Wiwiworkspace> &workspace,
                const std::string &id) {
    if (!workspace)
      return nullptr;
    for (const auto &p : workspace->get_paintings()) {
      if (p->string_id() == id)
        return p;
    }
    return nullptr;
  }

  static void invalidate_cache(const std::string &id) {
    std::lock_guard<std::mutex> lock(cache_mutex);
    for (auto it = cache_map.begin(); it != cache_map.end();) {
      if (it->first.rfind(id, 0) == 0) {
        it = cache_map.erase(it);
      } else {
        ++it;
      }
    }
  }

  static void clear_cache() {
    std::lock_guard<std::mutex> lock(cache_mutex);
    cache_map.clear();
  }

  static std::vector<uint8_t>
  get_painting_png_bytes(const std::shared_ptr<Wiwiworkspace> &workspace,
                         const std::string &id, const std::string &ratio_str) {
    std::string cache_key = id + "_img_" + ratio_str;
    {
      std::lock_guard<std::mutex> lock(cache_mutex);
      auto it = cache_map.find(cache_key);
      if (it != cache_map.end())
        return it->second;
    }

    auto p = find_painting(workspace, id);
    if (!p)
      return {};
    Image_data img;
    if (!ratio_str.empty()) {
      img = p->painting_data(Painting_ratio::from_string(ratio_str));
    } else {
      img = p->cropped_original_data();
    }
    auto bytes = img.encode_png();
    {
      std::lock_guard<std::mutex> lock(cache_mutex);
      cache_map[cache_key] = bytes;
    }
    return bytes;
  }

  static std::vector<uint8_t>
  get_painting_icon_png_bytes(const std::shared_ptr<Wiwiworkspace> &workspace,
                              const std::string &id) {
    std::string cache_key = id + "_icon";
    {
      std::lock_guard<std::mutex> lock(cache_mutex);
      auto it = cache_map.find(cache_key);
      if (it != cache_map.end())
        return it->second;
    }

    auto p = find_painting(workspace, id);
    if (!p)
      return {};
    auto bytes = p->icon_data().encode_png();
    {
      std::lock_guard<std::mutex> lock(cache_mutex);
      cache_map[cache_key] = bytes;
    }
    return bytes;
  }

  static void bind(saucer::smartview *webview,
                   std::shared_ptr<Wiwiworkspace> &workspace) {
    webview->expose("painting_get_reasonable_ratios", []() {
      nlohmann::json result = nlohmann::json::array();
      for (const auto &r : Painting_ratio::reasonable) {
        result.push_back(r.to_string());
      }
      return result.dump();
    });

    webview->expose("painting_get_ratios", [&workspace](const std::string &id) {
      auto p = find_painting(workspace, id);
      nlohmann::json result = nlohmann::json::array();
      if (p) {
        for (const auto &r : p->get_ratios()) {
          result.push_back(r.to_string());
        }
      }
      return result.dump();
    });

    webview->expose(
        "painting_add_ratio",
        [&workspace](const std::string &id, const std::string &ratio_str) {
          auto p = find_painting(workspace, id);
          if (p)
            p->add_ratio(Painting_ratio::from_string(ratio_str));
        });

    webview->expose(
        "painting_remove_ratio",
        [&workspace](const std::string &id, const std::string &ratio_str) {
          auto p = find_painting(workspace, id);
          if (p)
            return p->remove_ratio(Painting_ratio::from_string(ratio_str));
          return false;
        });

    webview->expose(
        "painting_set_title",
        [&workspace](const std::string &id, const std::string &title) {
          auto p = find_painting(workspace, id);
          if (p)
            p->set_title(title);
        });

    webview->expose(
        "painting_set_author",
        [&workspace](const std::string &id, const std::string &author) {
          auto p = find_painting(workspace, id);
          if (p)
            p->set_author(author);
        });

    webview->expose("painting_set_crop",
                    [&workspace](const std::string &id, double x, double y,
                                 double w, double h) {
                      invalidate_cache(id);
                      auto p = find_painting(workspace, id);
                      if (p)
                        p->set_crop(x, y, w, h);
                    });

    webview->expose("painting_set_contrast",
                    [&workspace](const std::string &id, float contrast) {
                      invalidate_cache(id);
                      auto p = find_painting(workspace, id);
                      if (p)
                        p->set_contrast(contrast);
                    });

    webview->expose("painting_set_brightness",
                    [&workspace](const std::string &id, int brightness) {
                      invalidate_cache(id);
                      auto p = find_painting(workspace, id);
                      if (p)
                        p->set_brightness(brightness);
                    });

    webview->expose("painting_set_placeable",
                    [&workspace](const std::string &id, bool placeable) {
                      auto p = find_painting(workspace, id);
                      if (p)
                        p->set_placeable(placeable);
                    });

    webview->expose("painting_set_has_stonecutter_recipe",
                    [&workspace](const std::string &id, bool has_recipe) {
                      auto p = find_painting(workspace, id);
                      if (p)
                        p->set_has_stonecutter_recipe(has_recipe);
                    });

    webview->expose("painting_set_pixels_per_block",
                    [&workspace](const std::string &id, size_t ppb) {
                      invalidate_cache(id);
                      auto p = find_painting(workspace, id);
                      if (p)
                        p->set_pixels_per_block(ppb);
                    });

    webview->expose("painting_use_no_frame",
                    [&workspace](const std::string &id) {
                      invalidate_cache(id);
                      auto p = find_painting(workspace, id);
                      if (p)
                        p->use_no_frame();
                    });

    webview->expose("painting_use_procedural_frame",
                    [&workspace](const std::string &id) {
                      invalidate_cache(id);
                      auto p = find_painting(workspace, id);
                      if (p)
                        p->use_procedural_frame();
                    });

    webview->expose(
        "painting_set_procedural_settings",
        [&workspace](const std::string &id, const std::string &seed_str,
                     const std::string &tint_hex) {
          invalidate_cache(id);
          auto p = find_painting(workspace, id);
          if (p) {
            auto settings = p->get_procedural_settings();
            if (settings) {
              if (!seed_str.empty()) {
                try {
                  settings->set_seed(std::stoull(seed_str));
                } catch (...) {}
              }
              if (tint_hex.size() == 7 && tint_hex[0] == '#') {
                try {
                  unsigned char r =
                      std::stoul(tint_hex.substr(1, 2), nullptr, 16);
                  unsigned char g =
                      std::stoul(tint_hex.substr(3, 2), nullptr, 16);
                  unsigned char b =
                      std::stoul(tint_hex.substr(5, 2), nullptr, 16);
                  settings->set_tint({r, g, b, 255});
                } catch (...) {}
              }
              p->invalidate_cache();
            }
          }
        });

    webview->expose(
        "painting_get_procedural_settings",
        [&workspace](const std::string &id) {
          auto p = find_painting(workspace, id);
          if (p && p->is_frame_procedural()) {
            auto settings = p->get_procedural_settings();
            if (settings) {
              auto t = settings->get_tint();
              std::string hex =
                  std::format("#{:02x}{:02x}{:02x}", t.r, t.g, t.b);
              return nlohmann::json(
                         {{"seed", std::to_string(settings->get_seed())},
                          {"tintHex", hex}})
                  .dump();
            }
          }
          return std::string("null");
        });

    webview->expose("painting_rotate_clockwise",
                    [&workspace](const std::string &id) {
                      invalidate_cache(id);
                      auto p = find_painting(workspace, id);
                      if (p)
                        p->rotate_clockwise();
                    });

    webview->expose("painting_rotate_anticlockwise",
                    [&workspace](const std::string &id) {
                      invalidate_cache(id);
                      auto p = find_painting(workspace, id);
                      if (p)
                        p->rotate_anticlockwise();
                    });

    webview->expose("painting_get_icon_url",
                    [&workspace](const std::string &id) -> std::string {
                      auto p = find_painting(workspace, id);
                      if (!p)
                        return "";
                      auto png = p->icon_data().encode_png();
                      return "data:image/png;base64," + base64_encode(png);
                    });

    webview->expose("painting_get_data_url",
                    [&workspace](const std::string &id,
                                 const std::string &ratio_str) -> std::string {
                      auto p = find_painting(workspace, id);
                      if (!p)
                        return "";
                      Image_data img;
                      if (!ratio_str.empty()) {
                        img = p->painting_data(
                            Painting_ratio::from_string(ratio_str));
                      } else {
                        img = p->cropped_original_data();
                      }
                      auto png = img.encode_png();
                      return "data:image/png;base64," + base64_encode(png);
                    });
  }
};

} // namespace wiwi::bridge
