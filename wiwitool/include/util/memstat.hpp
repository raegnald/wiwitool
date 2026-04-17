// Utility to see used memory in browser

#pragma once

#include <print>

#ifdef EMSCRIPTEN
#include <emscripten/heap.h>
#include <malloc.h>

inline void print_memory_stats(std::string step_name) {
  const size_t total_memory = emscripten_get_heap_size();
  const size_t used_memory = mallinfo().uordblks;

  std::println("[Memory] {} - Used: {} MB / Total: {} MB", step_name,
               used_memory / (1024 * 1024), total_memory / (1024 * 1024));
}
#else

inline void print_memory_stats(std::string step_name) {
  std::println("[Memory] In step {}", step_name);
}
#endif
