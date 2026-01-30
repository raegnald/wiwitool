#include <print>
#include <cstdlib>

#include "Minecraft_pack.hpp"
#include "Paintings_pack.hpp"

int main(void) {
  Minecraft_pack::genpath = ".";
  Paintings_pack pack;

  auto &painting = pack.add_painting("./wiwi.jpg");

  painting.set_title("Mi wiwi");
  painting.set_author("Pixel");

  pack.generate();
  // pack.compress();

  return EXIT_SUCCESS;
}
