// Generate a paintings pack with all the image files in a given
// folder.

#include <algorithm>
#include <array>
#include <filesystem>
#include <print>
#include <cstdlib>
#include <stdexcept>

#include "Minecraft_pack.hpp"
#include "Paintings_pack.hpp"
#include "Paintings_pack/Painting.hpp"

#include "util/wiwidebug.hpp"

int main(int argc, const char *argv[]) {
  Minecraft_pack::genpath = std::filesystem::temp_directory_path() / "wiwitool";

  if (argc != 2) throw std::runtime_error("invalid number of standard arguments");
  const std::filesystem::path images_dir = argv[1];

  Paintings_pack pack;

  const auto all_entries =
      std::filesystem::recursive_directory_iterator(images_dir);
  const auto image_extensions = std::array{".png", ".jpg", ".jpeg"};

  for (const auto &entry : all_entries) {
    const auto is_image =
        std::find(image_extensions.begin(), image_extensions.end(),
                  entry.path().extension()) != image_extensions.end();

    if (entry.is_regular_file() and is_image) {
      wiwidebug std::println("Adding image {}...", entry.path().string());
      auto &painting = pack.add_painting(entry.path());
      painting.set_title(entry.path().filename());
      painting.set_author("R2");
    }
  }

  wiwidebug std::println("Generating packs...");
  pack.generate();

  return EXIT_SUCCESS;
}
