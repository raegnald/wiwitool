#include <print>
#include <cstdlib>
#include <array>

#include "Minecraft_pack.hpp"
#include "paintings/Paintings_pack.hpp"

int main(void) {
  Minecraft_pack::genpath = ".";
  Paintings_pack pack;

  auto painting = Painting("./wiwi.jpg");

  painting.set_title("Mi wiwi");
  painting.set_author("Pixel");

  std::array paintings{std::move(painting)};

  pack.set_paintings(paintings);

  pack.generate();
  // pack.compress();

  return EXIT_SUCCESS;
}
