#include "Serialiser.hpp"

#include "Minecraft_pack.hpp"

#include <fstream>
#include <sstream>

void Serialiser::serialise(std::string data) {
  std::ofstream{Minecraft_pack::genpath / import_filename} << data;
}

std::string Serialiser::deserialise(void) {
  std::stringstream buf;
  buf << std::ifstream{Minecraft_pack::genpath / import_filename}.rdbuf();
  return buf.str();
}


#ifdef EMSCRIPTEN
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(serialiser) {
  class_<Serialiser>("Serialiser")
      .class_function("serialise", &Serialiser::serialise)
      .class_function("deserialise", &Serialiser::deserialise);
}
#endif
