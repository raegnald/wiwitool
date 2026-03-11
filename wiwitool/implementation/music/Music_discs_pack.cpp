#include "music/Music_discs_pack.hpp"

#include <print>
#include <utility>

#include "util/wiwidebug.hpp"

Music_discs_pack::Music_discs_pack(void)
    : Minecraft_pack{"Minecraft pack containing custom music discs"} {}

void Music_discs_pack::set_discs(std::vector<Music_disc> ds) { discs = ds; }

void Music_discs_pack::generate_data(void) {
  wiwidebug std::println("Music_discs_pack::generate_data unimplemented");
  std::unreachable();
}

void Music_discs_pack::generate_resource(void) {
  wiwidebug std::println("Music_discs_pack::generate_resource unimplemented");
  std::unreachable();
}
