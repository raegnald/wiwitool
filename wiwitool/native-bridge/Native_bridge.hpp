#pragma once

#include "File_dialog_bridge.hpp"
#include "Music_disc_bridge.hpp"
#include "Painting_bridge.hpp"
#include "Wiwiworkspace.hpp"
#include "Workspace_bridge.hpp"
#include <memory>
#include <saucer/smartview.hpp>

#include <string>
#include <vector>

namespace wiwi::bridge {

class Native_bridge {
private:
  std::shared_ptr<Wiwiworkspace> workspace;

public:
  Native_bridge() : workspace(std::make_shared<Wiwiworkspace>()) {}

  void invalidate_cache(const std::string &id) {
    Painting_bridge::invalidate_cache(id);
  }

  void clear_cache() { Painting_bridge::clear_cache(); }

  std::vector<uint8_t> get_painting_png_bytes(const std::string &id,
                                              const std::string &ratio_str) {
    return Painting_bridge::get_painting_png_bytes(workspace, id, ratio_str);
  }

  std::vector<uint8_t> get_painting_icon_png_bytes(const std::string &id) {
    return Painting_bridge::get_painting_icon_png_bytes(workspace, id);
  }

  void register_bindings(saucer::smartview *webview) {
    File_dialog_bridge::bind(webview, workspace);
    Workspace_bridge::bind(webview, workspace);
    Painting_bridge::bind(webview, workspace);
    Music_disc_bridge::bind(webview, workspace);
  }
};

} // namespace wiwi::bridge
