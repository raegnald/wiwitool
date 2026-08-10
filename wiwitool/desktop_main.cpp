#include <filesystem>
#include <fstream>
#include <functional.hpp>
#include <iostream>
#include <print>
#include <saucer/smartview.hpp>
#include <string>
#include <thread>
#include <unordered_map>

#include "native-bridge/Native_bridge.hpp"
#include "util/wiwidebug.hpp"

namespace fs = std::filesystem;

static wiwi::bridge::Native_bridge g_bridge;

coco::stray start(saucer::application *app) {
  auto window = saucer::window::create(app).value();
  auto webview = saucer::smartview::create({.window = window});

  // Register native workspace & file bridge bindings
  if (webview)
    g_bridge.register_bindings(&webview.value());

  window->set_title("Wiwitool");
  window->set_size({1280, 800});
  window->set_min_size({900, 600});

  window->set_decorations(saucer::window::decoration::partial);
  window->set_resizable(true);

  // Initial IPC bindings

  webview->expose("window_start_drag", [window]() { window->start_drag(); });

  webview->expose("window_start_resize", [window](int edge_val) {
    window->start_resize(static_cast<saucer::window::edge>(edge_val));
  });

  webview->expose("window_minimize",
                  [window]() { window->set_minimized(true); });
  webview->expose("window_maximize",
                  [window]() { window->set_maximized(!window->maximized()); });
  webview->expose("window_close", [window]() { window->close(); });

  wiwidebug webview->set_dev_tools(true);

  // Handle custom 'app' scheme to serve front/dist safely without CORS file://
  // issues
  webview->handle_scheme("app", [](saucer::scheme::request req,
                                   saucer::scheme::executor exec) {
    std::string full_url = req.url().string();
    std::string url_path = req.url().path();

    if (url_path == "/api/painting_image" || url_path == "/api/painting_icon") {
      std::string id, ratio;
      auto id_pos = full_url.find("id=");
      if (id_pos != std::string::npos) {
        auto end_pos = full_url.find("&", id_pos);
        id = full_url.substr(id_pos + 3, end_pos == std::string::npos
                                             ? std::string::npos
                                             : end_pos - (id_pos + 3));
      }
      auto ratio_pos = full_url.find("ratio=");
      if (ratio_pos != std::string::npos) {
        auto end_pos = full_url.find("&", ratio_pos);
        ratio = full_url.substr(ratio_pos + 6, end_pos == std::string::npos
                                                   ? std::string::npos
                                                   : end_pos - (ratio_pos + 6));
      }

      std::thread([req_type = url_path, id, ratio, exec]() mutable {
        std::vector<uint8_t> png_bytes;
        if (req_type == "/api/painting_icon") {
          png_bytes = g_bridge.get_painting_icon_png_bytes(id);
        } else {
          png_bytes = g_bridge.get_painting_png_bytes(id, ratio);
        }

        if (png_bytes.empty()) {
          exec.reject(saucer::scheme::error::not_found);
          return;
        }

        exec.resolve(saucer::scheme::response{
            .data = saucer::stash::from(std::move(png_bytes)),
            .mime = "image/png",
            .headers = {{"Access-Control-Allow-Origin", "*"}},
            .status = 200});
      }).detach();
      return;
    }

    if (url_path.empty() || url_path == "/") {
      url_path = "/index.html";
    }
    if (!url_path.empty() && url_path[0] == '/') {
      url_path = url_path.substr(1);
    }

    fs::path target_file = fs::absolute("front/dist") / url_path;
    if (!fs::exists(target_file) || fs::is_directory(target_file)) {
      return exec.reject(saucer::scheme::error::not_found);
    }

    std::ifstream file(target_file, std::ios::binary);
    if (!file.is_open()) {
      return exec.reject(saucer::scheme::error::failed);
    }

    std::vector<std::uint8_t> data((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());

    static const std::unordered_map<std::string, std::string> mime_types = {
        {".html", "text/html"},        {".js", "text/javascript"},
        {".css", "text/css"},          {".svg", "image/svg+xml"},
        {".png", "image/png"},         {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},       {".json", "application/json"},
        {".wasm", "application/wasm"},
    };

    std::string ext = target_file.extension().string();
    auto it = mime_types.find(ext);
    std::string mime =
        (it != mime_types.end()) ? it->second : "application/octet-stream";

    exec.resolve(saucer::scheme::response{
        .data = saucer::stash::from(std::move(data)),
        .mime = std::move(mime),
        .headers = {{"Access-Control-Allow-Origin", "*"}},
        .status = 200});
  });

  fs::path dist_index = fs::absolute("front/dist/index.html");
  if (fs::exists(dist_index)) {
    std::println(
        "[Wiwitool Desktop] Serving frontend via app://localhost/index.html");
    webview->set_url("app://localhost/index.html");
  } else {
    std::println(
        "[Wiwitool Desktop] Connecting to dev server http://localhost:8080");
    webview->set_url("http://localhost:8080");
  }

  window->show();

  co_await app->finish();
}

int main(int argc, char *argv[]) {
  std::println("[Wiwitool Desktop] Registering custom scheme 'app'...");
  saucer::smartview::register_scheme("app");

  auto app = saucer::application::create({.id = "net.gligan.wiwitool.app"});

  return app->run(start);
}
