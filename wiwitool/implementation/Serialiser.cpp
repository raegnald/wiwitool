#include "Serialiser.hpp"

#include "Minecraft_pack.hpp"

#include <fstream>
#include <vector>

void Serialiser::serialise(std::vector<uint8_t> data) {
  std::ofstream out{Minecraft_pack::genpath / import_filename,
                    std::ios::binary};
  out.write(reinterpret_cast<const char *>(data.data()), data.size());
}

std::vector<uint8_t> Serialiser::deserialise(void) {
  std::ifstream in{Minecraft_pack::genpath / import_filename, std::ios::binary};
  return std::vector<uint8_t>((std::istreambuf_iterator<char>(in)),
                              std::istreambuf_iterator<char>());
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
