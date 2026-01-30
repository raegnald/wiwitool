#pragma once

#include <filesystem>
#include <string>

/// A Minecraft pack generates both a data- and resouce-pack.
class Minecraft_pack {
private:
  struct mcversion_range { float min, max; };

  const std::string description;
  const mcversion_range data_ver{88.0, 94.1}, res_ver{73.0, 76.0};

  void generate_pack_skeletons(void);

public:
  static inline std::filesystem::path genpath{"/tmp"};

  virtual ~Minecraft_pack(void) = default;

  void generate(void);
  void compress(void);

protected:
  explicit Minecraft_pack(std::string description) : description{description} {}

  // Generates the inner folder and file structure necessary for the
  // specific data- and resource-pack to work.
  virtual void generate_data() = 0;
  virtual void generate_resource() = 0;

  /// Get existing or create new subfolder relative to data pack
  /// folder.
  std::filesystem::path in_data_folder(std::string name = std::string{});

  static inline const std::filesystem::path data_subfolder{"data/"};

  /// Get existing or create new subfolder relative to resource pack
  /// folder.
  std::filesystem::path in_resource_folder(std::string name = std::string{});

  static inline const std::filesystem::path assets_subfolder{"assets/"};
};

#ifdef EMSCRIPTEN
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(minecraft_pack) {
  class_<Minecraft_pack>("Minecraft_pack")
    .function("generate", &Minecraft_pack::generate)
    .function("compress", &Minecraft_pack::compress);
}
#endif
