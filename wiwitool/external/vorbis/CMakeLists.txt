if(EMSCRIPTEN)

  add_library(vorbis INTERFACE)

  target_compile_options(vorbis
    INTERFACE
      --use-port=vorbis
  )

  target_link_options(vorbis
    INTERFACE
      --use-port=vorbis
  )

else()

  find_package(PkgConfig REQUIRED)

  pkg_check_modules(VORBIS REQUIRED IMPORTED_TARGET vorbis)
  pkg_check_modules(VORBISENC REQUIRED IMPORTED_TARGET vorbisenc)
  pkg_check_modules(OGG REQUIRED IMPORTED_TARGET ogg)

  add_library(vorbis INTERFACE)

  target_link_libraries(vorbis
    INTERFACE
      PkgConfig::VORBISENC
      PkgConfig::VORBIS
      PkgConfig::OGG
  )


endif()
