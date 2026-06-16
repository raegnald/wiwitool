#include "Wiwiworkspace.hpp"

#include "Image_data.hpp"
#include "Minecraft_pack.hpp"
#include "Serialiser.hpp"
#include "Minecraft_packer.hpp"

#include "music/Music_discs_pack.hpp"
#include "paintings/Paintings_pack.hpp"
#include "paintings/Painting.hpp"

#include "Invisible_item_frame_pack.hpp"

#include "util/memstat.hpp"
#include "util/wiwidebug.hpp"

#include <filesystem>
#include <memory>
#include <print>
#include <vector>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void Wiwiworkspace::set_workspace_image(Image_data im) {
  if (im.empty()) return;

  workspace_image = im.crop(1, 1).scale(256, 256);
}

Image_data Wiwiworkspace::get_workspace_icon(void) const {
  return workspace_image;
}

// Paintings

void Wiwiworkspace::add_painting(std::shared_ptr<Painting> painting) {
  paintings.push_back(std::move(painting));
}

void Wiwiworkspace::remove_painting(std::string painting_id) {
  std::erase_if(paintings, [&](const auto painting) {
    return painting->string_id() == painting_id;
  });
}

std::vector<std::shared_ptr<Painting>>
Wiwiworkspace::get_paintings(void) const {
  return paintings;
}

// Music discs

void Wiwiworkspace::add_music_disc(std::shared_ptr<Music_disc> disc) {
  discs.push_back(std::move(disc));
}

void Wiwiworkspace::remove_music_disc(std::string disc_id) {
  std::erase_if(discs, [&](const auto disc) {
    return disc->string_id() == disc_id;
  });
}

std::vector<std::shared_ptr<Music_disc>>
Wiwiworkspace::get_music_discs(void) const {
  return discs;
}

// Invisible item frames

void Wiwiworkspace::set_invisible_item_frames(bool set) {
  invisible_item_frames = set;
}

bool Wiwiworkspace::get_invisible_item_frames(void) const {
  return invisible_item_frames;
}

// Serialisation and deserialisation

std::vector<uint8_t> Wiwiworkspace::serialise(void) const {
  json j;

  j["workspace_name"] = workspace_name;
  j["workspace_image"] = workspace_image;
  j["export_count"] = export_count;

  // Paintings
  j["paintings_namespace_override"] = *paintings_namespace_override;
  for (const auto &painting : paintings)
    j["paintings"].push_back(*painting);

  // Music discs
  j["music_discs_namespace_override"] = *music_discs_namespace_override;
  for (const auto &disc : discs)
    j["music_discs"].push_back(*disc);

  // Invisible item frames
  j["invisible_item_frames_namespace_override"] = *iif_namespace_override;
  j["invisible_item_frames"] = invisible_item_frames;

  return nlohmann::json::to_msgpack(j);
}

void Wiwiworkspace::deserialise(const std::vector<uint8_t> &binary_msgpack) {
  paintings.clear();
  discs.clear();

  json j = nlohmann::json::from_msgpack(binary_msgpack);

  if (j.contains("workspace_name"))
    j.at("workspace_name").get_to(workspace_name);
  else
    wiwidebug std::println("This workspace does not have a name");

  if (j.contains("workspace_image"))
    j.at("workspace_image").get_to(workspace_image);

  if (j.contains("export_count"))
    j.at("export_count").get_to(export_count);

  // Paintings

  if (j.contains("paintings_namespace_override"))
    j.at("paintings_namespace_override").get_to(*paintings_namespace_override);

  if (j.contains("paintings")) {
    auto flat_paintings = j["paintings"].get<std::vector<Painting>>();

    for (auto &p : flat_paintings)
      paintings.push_back(std::make_shared<Painting>(p));
  }

  // Music discs

  if (j.contains("music_discs_namespace_override"))
    j.at("music_discs_namespace_override")
        .get_to(*music_discs_namespace_override);

  if (j.contains("music_discs")) {
    auto flat_discs = j["music_discs"].get<std::vector<Music_disc>>();

    for (auto &d : flat_discs)
      discs.push_back(std::make_shared<Music_disc>(d));
  }

  // Invisible item frames

  if (j.contains("invisible_item_frames_namespace_override"))
    j.at("invisible_item_frames_namespace_override")
        .get_to(*iif_namespace_override);

  invisible_item_frames = j.value("invisible_item_frames", false);
}

std::filesystem::path Wiwiworkspace::generate_zip(void) {
  Minecraft_packer packer;

  Paintings_pack paintings_pack;
  Music_discs_pack music_discs_pack;
  Invisible_item_frame_pack iif_pack;

  paintings_pack.set_workspace_name(
      paintings_namespace_override->get().value_or(workspace_name),
      not paintings_namespace_override->has_value());

  music_discs_pack.set_workspace_name(
      music_discs_namespace_override->get().value_or(workspace_name),
      not music_discs_namespace_override->has_value());

  iif_pack.set_workspace_name(
      iif_namespace_override->get().value_or(workspace_name),
      not iif_namespace_override->has_value());

  wiwidebug print_memory_stats("Start generate_zip");

  export_count++;
  Serialiser::serialise(serialise());

  wiwidebug print_memory_stats("After workspace serialisation");

  // Paintings pack
  if (not paintings.empty()) {
    paintings_pack.set_paintings(paintings);
    packer.add(paintings_pack);
  }

  // Music discs pack
  if (not discs.empty()) {
    music_discs_pack.set_discs(discs);
    packer.add(music_discs_pack);
  }

  // Invisible item frames pack
  if (invisible_item_frames) {
    packer.add(iif_pack);
  }

  // Add pack.png from workspace
  if (not workspace_image.empty()) {
    workspace_image.save_png(Minecraft_pack::genpath / "pack.png");
  }

  const auto zip_path = packer.get_zip();
  wiwidebug print_memory_stats("End generate_zip");

  return zip_path;
}


// Namespace overrides

void to_json(nlohmann::json &j, const Namespace_override &n) {
  if (not n.namespace_override)
    return;

  j = n.namespace_override.value();
}

void from_json(const nlohmann::json &j, Namespace_override &n) {
  if (not j.is_string())
    return;

  n.namespace_override = j.get<std::string>();
}


#ifdef EMSCRIPTEN
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(wiwiworkspace) {
  register_vector<std::shared_ptr<Painting>>("WorkspacePaintingsVector");
  register_vector<std::shared_ptr<Music_disc>>("WorkspaceMusicDiscsVector");

  class_<Namespace_override>("NamespaceOverride")
      .smart_ptr<std::shared_ptr<Namespace_override>>("NamespaceOverride")
      .constructor<>()
      .constructor<std::string>()

      .function("get", optional_override(
                           [](Namespace_override &self) -> emscripten::val {
                             auto opt = self.get();
                             if (opt.has_value()) {
                               return emscripten::val(opt.value());
                             }
                             return emscripten::val("");
                           }))
      .function("set", &Namespace_override::set)
      .function("reset", &Namespace_override::reset);

  class_<Wiwiworkspace>("Wiwiworkspace")
      .constructor<>()

      .property("workspaceName", &Wiwiworkspace::get_workspace_name,
                &Wiwiworkspace::set_workspace_name)

      .property("workspaceImage", &Wiwiworkspace::get_workspace_icon,
                &Wiwiworkspace::set_workspace_image)

      .function("resetWorkspaceImage", &Wiwiworkspace::reset_workspace_image)

      .function(
          "setWorkspaceIconFromBytes",
          optional_override([](Wiwiworkspace &self, std::string image_bytes) {
            const uint8_t *ptr =
                reinterpret_cast<const uint8_t *>(image_bytes.data());
            std::vector<uint8_t> vec(ptr, ptr + image_bytes.size());
            self.set_workspace_image(Image_data{vec});
          }))

      .function("addPainting", &Wiwiworkspace::add_painting)
      .function("removePainting", &Wiwiworkspace::remove_painting)
      .function("getPaintings", &Wiwiworkspace::get_paintings)

      .function("addMusicDisc", &Wiwiworkspace::add_music_disc)
      .function("removeMusicDisc", &Wiwiworkspace::remove_music_disc)
      .function("getMusicDiscs", &Wiwiworkspace::get_music_discs)

      .property("invisibleItemFrames",
                &Wiwiworkspace::get_invisible_item_frames,
                &Wiwiworkspace::set_invisible_item_frames)

      .function("getPaintingsNamespaceOverride",
                &Wiwiworkspace::get_paintings_namespace_override)
      .function("getMusicDiscsNamespaceOverride",
                &Wiwiworkspace::get_music_discs_namespace_override)
      .function("getIIFNamespaceOverride",
                &Wiwiworkspace::get_iif_namespace_override)

      .function("isWorkspaceNew", &Wiwiworkspace::is_workspace_new)

      .function("serialise", &Wiwiworkspace::serialise)
      .function("deserialise", optional_override([](Wiwiworkspace &self,
                                                    emscripten::val js_array) {
                  size_t length = js_array["length"].as<size_t>();
                  std::vector<uint8_t> vec(length);
                  emscripten::val memory_view = emscripten::val(
                      emscripten::typed_memory_view(length, vec.data()));
                  memory_view.call<void>("set", js_array);
                  self.deserialise(vec);
                }))

      .function("generateZip", optional_override([](Wiwiworkspace &self) {
                  return std::string{self.generate_zip()};
                }));
}
#endif
