
#ifndef EMSCRIPTEN
#error "This exectuable must be compiled with Emscripten"
#endif

#include <print>
#include <cstdlib>
#include "Minecraft_pack.hpp"
#include "Paintings_pack.hpp"
#include <emscripten/emscripten.h>

// EMSCRIPTEN_KEEPALIVE extern "C"
// int wiwisum(int a, int b) {
//   return a + b;
// }

// class Dummy_pack : public Minecraft_pack {
// public:
//   Dummy_pack(void) : Minecraft_pack("A dummy Minecraft package") {}
//
// protected:
//   virtual void generate_data() {}
//   virtual void generate_resource() {}
// };

// int main(void) {
//   Dummy_pack pack;
//
//   pack.generate();
//   pack.compress();
//
//   return EXIT_SUCCESS;
// }
