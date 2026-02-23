#include "Wiwiworkspace.hpp"

#include "Serialiser.hpp"
#include "Minecraft_packer.hpp"

#include "paintings/Paintings_pack.hpp"
#include "paintings/Painting.hpp"

#include "Invisible_item_frame_pack.hpp"

#include <filesystem>
#include <memory>
#include <vector>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

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

void Wiwiworkspace::set_invisible_item_frames(bool set) {
  invisible_item_frames = set;
}

bool Wiwiworkspace::get_invisible_item_frames(void) const {
  return invisible_item_frames;
}

std::string Wiwiworkspace::serialise(void) const {
  json j;

  j["invisible_item_frames"] = invisible_item_frames;

  std::vector<Painting> flat_paintings;
  for (const auto &painting : paintings)
    flat_paintings.push_back(Painting{*painting});

  j["paintings"] = flat_paintings;

  return j.dump(-1);
}

void Wiwiworkspace::deserialise(const std::string &json_string) {
  paintings.clear();
  json j = json::parse(json_string);

  invisible_item_frames = j.value("invisible_item_frames", false);

  if (j.contains("paintings")) {
    auto flat_paintings = j["paintings"].get<std::vector<Painting>>();

    for (auto &p : flat_paintings)
      paintings.push_back(std::make_shared<Painting>(p));
  }
}

std::filesystem::path Wiwiworkspace::generate_zip(void) {
  Minecraft_packer packer;

  Paintings_pack paintings_pack;
  Invisible_item_frame_pack iif_pack;

  Serialiser::serialise(serialise()); // FOTUT: I don't like current
                                      // way I am injecting the
                                      // serialised workspace data.

  // Paintings pack
  if (not paintings.empty()) {
    std::vector<Painting> copies;

    for (const auto &p : paintings)
      copies.push_back(Painting{*p});

    paintings_pack.set_paintings(std::move(copies));
    packer.add(paintings_pack);
  }

  // Invisible item frames pack
  if (invisible_item_frames) {
    packer.add(iif_pack);
  }

  return packer.get_zip();
}


#ifdef EMSCRIPTEN
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(wiwiworkspace) {
  register_vector<std::shared_ptr<Painting>>("WorkspacePaintingsVector");

  class_<Wiwiworkspace>("Wiwiworkspace")
      .constructor<>()

      .function("addPainting", &Wiwiworkspace::add_painting)
      .function("removePainting", &Wiwiworkspace::remove_painting)
      .function("getPaintings", &Wiwiworkspace::get_paintings)

      .property("invisibleItemFrames",
                &Wiwiworkspace::get_invisible_item_frames,
                &Wiwiworkspace::set_invisible_item_frames)

      .function("serialise", &Wiwiworkspace::serialise)
      .function("deserialise", &Wiwiworkspace::deserialise)

      .function("generateZip", optional_override([](Wiwiworkspace &self) {
                  return std::string{self.generate_zip()};
                }));
}
#endif
