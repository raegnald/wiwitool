
add_library(stb
  stb_image.c
  stb_image_write.c
  stb_perlin.c
  stb_vorbis.c
)

target_include_directories(stb
  PUBLIC
    include
)
