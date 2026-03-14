#include "Wiwiworkspace.hpp"

#include "Serialiser.hpp"
#include "Minecraft_packer.hpp"

#include "music/Music_discs_pack.hpp"
#include "paintings/Paintings_pack.hpp"
#include "paintings/Painting.hpp"

#include "Invisible_item_frame_pack.hpp"

#include <filesystem>
#include <memory>
#include <vector>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

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

  // Paintings
  {
    std::vector<Painting> flat_paintings;
    for (const auto &painting : paintings)
      flat_paintings.push_back(Painting{*painting});

    j["paintings"] = flat_paintings;
  }

  // Music discs
  {
    std::vector<Music_disc> flat_music_discs;
    for (const auto &disc : discs)
      flat_music_discs.push_back(Music_disc{*disc});

    j["music_discs"] = flat_music_discs;
  }

  // Invisible item frames
  j["invisible_item_frames"] = invisible_item_frames;

  return nlohmann::json::to_msgpack(j);
}

void Wiwiworkspace::deserialise(const std::vector<uint8_t> &binary_msgpack) {
  paintings.clear();
  discs.clear();

  json j = nlohmann::json::from_msgpack(binary_msgpack);

  // Paintings
  if (j.contains("paintings")) {
    auto flat_paintings = j["paintings"].get<std::vector<Painting>>();

    for (auto &p : flat_paintings)
      paintings.push_back(std::make_shared<Painting>(p));
  }

  // Music discs
  if (j.contains("music_discs")) {
    auto flat_discs = j["music_discs"].get<std::vector<Music_disc>>();

    for (auto &d : flat_discs)
      discs.push_back(std::make_shared<Music_disc>(d));
  }

  // Invislbe item frames
  invisible_item_frames = j.value("invisible_item_frames", false);
}

std::filesystem::path Wiwiworkspace::generate_zip(void) {
  Minecraft_packer packer;

  Paintings_pack paintings_pack;
  Music_discs_pack music_discs_pack;
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

  // Music discs pack
  if (not discs.empty()) {
    std::vector<Music_disc> copies;

    for (const auto &disc : discs)
      copies.push_back(Music_disc{*disc});

    music_discs_pack.set_discs(copies);
    packer.add(music_discs_pack);
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
  register_vector<std::shared_ptr<Music_disc>>("WorkspaceMusicDiscsVector");

  class_<Wiwiworkspace>("Wiwiworkspace")
      .constructor<>()

      .function("addPainting", &Wiwiworkspace::add_painting)
      .function("removePainting", &Wiwiworkspace::remove_painting)
      .function("getPaintings", &Wiwiworkspace::get_paintings)

      .function("addMusicDisc", &Wiwiworkspace::add_music_disc)
      .function("removeMusicDisc", &Wiwiworkspace::remove_music_disc)
      .function("getMusicDiscs", &Wiwiworkspace::get_music_discs)

      .property("invisibleItemFrames",
                &Wiwiworkspace::get_invisible_item_frames,
                &Wiwiworkspace::set_invisible_item_frames)

      .function("serialise", &Wiwiworkspace::serialise)
      .function(
          "deserialise",
          optional_override([](Wiwiworkspace &self,
                               const std::vector<uint8_t> &binary_msgpack) {
            self.deserialise(binary_msgpack);
          }))

      .function("generateZip", optional_override([](Wiwiworkspace &self) {
                  return std::string{self.generate_zip()};
                }));
}
#endif
