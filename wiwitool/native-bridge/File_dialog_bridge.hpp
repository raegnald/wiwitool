#pragma once

#include "Wiwiworkspace.hpp"
#include "portable-file-dialogs.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <saucer/smartview.hpp>
#include <string>
#include <vector>

namespace wiwi::bridge {

class File_dialog_bridge {
public:
  static void bind(saucer::smartview *webview,
                   std::shared_ptr<Wiwiworkspace> workspace) {
    webview->expose(
        "native_open_file_dialog", [](const std::string &file_type) {
          std::vector<std::string> filters;
          if (file_type == "image") {
            filters = {"Image Files", "*.png *.jpg *.jpeg *.webp"};
          } else if (file_type == "audio") {
            filters = {"Audio Files", "*.ogg *.mp3 *.wav *.flac"};
          } else {
            filters = {"All Files", "*"};
          }

          auto selection =
              pfd::open_file("Select File", "", filters, pfd::opt::none)
                  .result();
          if (selection.empty())
            return std::string("");

          std::string chosen_path = selection.front();
          std::filesystem::path p(chosen_path);
          nlohmann::json res;
          res["name"] = p.filename().string();
          res["path"] = chosen_path;

          std::ifstream in(chosen_path, std::ios::binary);
          if (in) {
            std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(in)),
                                       std::istreambuf_iterator<char>());
            res["data"] = bytes;
          } else {
            res["data"] = nlohmann::json::array();
          }
          return res.dump();
        });

    webview->expose("native_save_file_dialog",
                    [workspace](const std::string &default_name,
                                const std::string &data_or_path) {
                      auto destination =
                          pfd::save_file("Save Pack Zip", default_name,
                                         {"Zip Files", "*.zip"})
                              .result();
                      if (destination.empty())
                        return false;

                      std::filesystem::path zip_path =
                          workspace->generate_zip();
                      if (std::filesystem::exists(zip_path)) {
                        std::filesystem::copy_file(
                            zip_path, destination,
                            std::filesystem::copy_options::overwrite_existing);
                        return true;
                      }
                      return false;
                    });
  }
};

} // namespace wiwi::bridge
