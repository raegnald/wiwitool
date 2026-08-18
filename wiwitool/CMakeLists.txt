cmake_minimum_required(VERSION 3.25)

project(wiwitool LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external)

add_library(wiwi
  implementation/Image_data.cpp
  implementation/Invisible_item_frame_pack.cpp
  implementation/Minecraft_pack.cpp
  implementation/Minecraft_packer.cpp
  implementation/Serialiser.cpp
  implementation/Wiwiworkspace.cpp
  implementation/colour.cpp

  implementation/music/Music_disc.cpp
  implementation/music/Music_discs_pack.cpp

  implementation/paintings/Painting.cpp
  implementation/paintings/Painting_converter.cpp
  implementation/paintings/Painting_frame_generator.cpp
  implementation/paintings/Painting_ratio.cpp
  implementation/paintings/Paintings_pack.cpp
)

target_include_directories(wiwi
  PUBLIC
    include
)

target_compile_features(wiwi
  PUBLIC
    cxx_std_23
)

target_link_libraries(wiwi
  PUBLIC
    json
    vorbis
  PRIVATE
    miniz_cpp
    stb
)

# Desktop executable

if (NOT EMSCRIPTEN)
  add_executable(wiwidesktop
    desktop_main.cpp)

  target_link_libraries(wiwidesktop
    PRIVATE
      wiwi
      saucer::saucer
      portable-file-dialogs
  )
endif()
