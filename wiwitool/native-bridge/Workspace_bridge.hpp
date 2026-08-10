#pragma once

#include "Wiwiworkspace.hpp"
#include "music/Music_disc.hpp"
#include "paintings/Painting.hpp"
#include "util/base64.hpp"
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <saucer/smartview.hpp>
#include <string>
#include <vector>

namespace wiwi::bridge {

class Workspace_bridge {
public:
  static void bind(saucer::smartview *webview,
                   std::shared_ptr<Wiwiworkspace> &workspace) {
    webview->expose("workspace_init", [&workspace]() {
      workspace = std::make_shared<Wiwiworkspace>();
      std::cout << "[NativeBridge] Workspace initialized." << std::endl;
      return true;
    });

    webview->expose("workspace_get_name", [&workspace]() -> std::string {
      return workspace->get_workspace_name();
    });

    webview->expose("workspace_is_new", [&workspace]() -> bool {
      return workspace->is_workspace_new();
    });

    webview->expose("workspace_set_name",
                    [&workspace](const std::string &name) {
                      workspace->set_workspace_name(name);
                    });

    webview->expose("workspace_deserialise",
                    [&workspace](const std::vector<uint8_t> &bytes) {
                      workspace->deserialise(bytes);
                    });

    webview->expose("workspace_set_invisible_item_frames",
                    [&workspace](bool enabled) {
                      workspace->set_invisible_item_frames(enabled);
                    });

    webview->expose("workspace_get_invisible_item_frames",
                    [&workspace]() -> bool {
                      return workspace->get_invisible_item_frames();
                    });

    webview->expose("workspace_get_paintings", [&workspace]() {
      nlohmann::json result = nlohmann::json::array();

      for (const auto &p : workspace->get_paintings()) {
        nlohmann::json item = {{"id", p->string_id()},
                               {"title", p->get_title()},
                               {"author", p->get_author()},
                               {"cropX", p->get_crop_x()},
                               {"cropY", p->get_crop_y()},
                               {"cropW", p->get_crop_w()},
                               {"cropH", p->get_crop_h()},
                               {"contrast", p->get_contrast()},
                               {"brightness", p->get_brightness()},
                               {"placeable", p->is_placeable()},
                               {"hasStonecutterRecipe", p->has_stonecutter_recipe()},
                               {"pixelPerBlock", p->get_pixels_per_block()},
                               {"isProceduralFrame", p->is_frame_procedural()}};
        if (p->is_frame_procedural()) {
          auto settings = p->get_procedural_settings();
          if (settings) {
            auto t = settings->get_tint();
            item["frameSeed"] = std::to_string(settings->get_seed());
            item["tintHex"] = std::format("#{:02x}{:02x}{:02x}", t.r, t.g, t.b);
          }
        }
        result.push_back(item);
      }

      return result.dump();
    });

    webview->expose(
        "namespace_override_get", [&workspace](const std::string &target) {
          if (target == "paintings") {
            auto opt = workspace->get_paintings_namespace_override()->get();
            return opt.value_or("");
          } else if (target == "music_discs") {
            auto opt = workspace->get_music_discs_namespace_override()->get();
            return opt.value_or("");
          } else if (target == "invisible_item_frames") {
            auto opt = workspace->get_iif_namespace_override()->get();
            return opt.value_or("");
          }
          return std::string("");
        });

    webview->expose(
        "namespace_override_set",
        [&workspace](const std::string &target, const std::string &ns) {
          if (target == "paintings") {
            workspace->get_paintings_namespace_override()->set(ns);
          } else if (target == "music_discs") {
            workspace->get_music_discs_namespace_override()->set(ns);
          } else if (target == "invisible_item_frames") {
            workspace->get_iif_namespace_override()->set(ns);
          }
        });

    webview->expose(
        "namespace_override_reset", [&workspace](const std::string &target) {
          if (target == "paintings") {
            workspace->get_paintings_namespace_override()->reset();
          } else if (target == "music_discs") {
            workspace->get_music_discs_namespace_override()->reset();
          } else if (target == "invisible_item_frames") {
            workspace->get_iif_namespace_override()->reset();
          }
        });

    webview->expose("workspace_get_image_url", [&workspace]() -> std::string {
      auto img = workspace->get_workspace_icon();
      if (img.empty())
        return "";
      auto png = img.encode_png();
      return "data:image/png;base64," + base64_encode(png);
    });

    webview->expose("workspace_set_image_from_bytes",
                    [&workspace](const std::vector<uint8_t> &bytes) {
                      workspace->set_workspace_image(Image_data{bytes});
                    });

    webview->expose("workspace_set_image_from_path",
                    [&workspace](const std::string &file_path) {
                      workspace->set_workspace_image(
                          Image_data{std::filesystem::path(file_path)});
                    });

    webview->expose("workspace_reset_image",
                    [&workspace]() { workspace->reset_workspace_image(); });

    webview->expose(
        "workspace_add_painting_from_path",
        [&workspace](const std::string &file_path) {
          Image_data hires{std::filesystem::path(file_path)};

          const double maxlen = std::max(hires.width(), hires.height());
          const double newmaxlen = 1024.0;

          auto downscaled =
              (maxlen > newmaxlen) ? hires.scale(newmaxlen / maxlen) : hires;

          auto p = std::make_shared<Painting>(downscaled);
          p->set_title(std::filesystem::path(file_path).stem().string());

          workspace->add_painting(p);

          nlohmann::json res = {{"id", p->string_id()},
                                {"title", p->get_title()},
                                {"author", p->get_author()},
                                {"isProceduralFrame", p->is_frame_procedural()}};
          if (p->is_frame_procedural()) {
            auto settings = p->get_procedural_settings();
            if (settings) {
              auto t = settings->get_tint();
              res["frameSeed"] = std::to_string(settings->get_seed());
              res["tintHex"] = std::format("#{:02x}{:02x}{:02x}", t.r, t.g, t.b);
            }
          }

          return res.dump();
        });

    webview->expose("workspace_add_painting_from_bytes",
                    [&workspace](const std::vector<uint8_t> &bytes) {
                      Image_data hires{bytes};
                      const double maxlen =
                          std::max(hires.width(), hires.height());
                      const double newmaxlen = 1024.0;
                      auto downscaled = (maxlen > newmaxlen)
                                            ? hires.scale(newmaxlen / maxlen)
                                            : hires;
                      auto p = std::make_shared<Painting>(downscaled);
                      workspace->add_painting(p);

                      nlohmann::json res = {{"id", p->string_id()},
                                            {"title", p->get_title()},
                                            {"author", p->get_author()},
                                            {"isProceduralFrame", p->is_frame_procedural()}};
                      if (p->is_frame_procedural()) {
                        auto settings = p->get_procedural_settings();
                        if (settings) {
                          auto t = settings->get_tint();
                          res["frameSeed"] = std::to_string(settings->get_seed());
                          res["tintHex"] = std::format("#{:02x}{:02x}{:02x}", t.r, t.g, t.b);
                        }
                      }

                      return res.dump();
                    });

    webview->expose("workspace_remove_painting",
                    [&workspace](const std::string &id) {
                      workspace->remove_painting(id);
                    });

    webview->expose("workspace_get_music_discs", [&workspace]() {
      nlohmann::json result = nlohmann::json::array();
      for (const auto &d : workspace->get_music_discs()) {
        result.push_back({{"id", d->string_id()},
                          {"title", d->get_title()},
                          {"artist", d->get_artist()},
                          {"hasStonecutterRecipe", d->has_stonecutter_recipe()},
                          {"comparatorOutput", d->get_comparator_output()},
                          {"silenceGap", d->get_silence_gap()}});
      }
      return result.dump();
    });

    webview->expose("workspace_add_music_disc_from_path",
                    [&workspace](const std::string &file_path) {
                      auto d = std::make_shared<Music_disc>();
                      d->set_title(
                          std::filesystem::path(file_path).stem().string());
                      workspace->add_music_disc(d);
                      return nlohmann::json({{"id", d->string_id()},
                                             {"title", d->get_title()},
                                             {"artist", d->get_artist()}})
                          .dump();
                    });

    webview->expose("workspace_add_music_disc_from_bytes",
                    [&workspace](const std::vector<uint8_t> &bytes) {
                      auto d = std::make_shared<Music_disc>();
                      workspace->add_music_disc(d);
                      return nlohmann::json({{"id", d->string_id()},
                                             {"title", d->get_title()},
                                             {"artist", d->get_artist()}})
                          .dump();
                    });

    webview->expose("workspace_remove_music_disc",
                    [&workspace](const std::string &id) {
                      workspace->remove_music_disc(id);
                    });

    webview->expose("workspace_generate_zip", [&workspace]() {
      auto path = workspace->generate_zip();
      return path.string();
    });

    webview->expose("workspace_dispose", [&workspace]() {
      workspace = std::make_shared<Wiwiworkspace>();
    });
  }
};

} // namespace wiwi::bridge
