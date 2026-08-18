
add_subdirectory(json)
add_subdirectory(miniz_cpp)
add_subdirectory(stb)
add_subdirectory(vorbis)

if (NOT EMSCRIPTEN)
  add_subdirectory(pfd)
  add_subdirectory(saucer)
endif()
