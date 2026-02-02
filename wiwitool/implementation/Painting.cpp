#include "Paintings_pack/Painting.hpp"

#include <format>
#include <fstream>
#include <print>

#include "Paintings_pack/Painting_converter.hpp"
#include "wiwidebug.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;

Painting::Painting(std::filesystem::path image_path)
  : painting_id{next_painting_id++}, original_image{image_path} {
  wiwidebug std::println("Loaded image (painting id = {})", painting_id);
  refresh();
}

const Image_data Painting::original_data(void) const { return original_image; }

const Image_data Painting::painting_data(void) const {
  compute_painting_and_icon_if_necessary();
  return painting;
}

const Image_data Painting::icon_data(void) const {
  compute_painting_and_icon_if_necessary();
  return icon;
}

void Painting::refresh(void) const {
  auto converter = Painting_converter{original_image};
  painting = converter.convert();
  icon = converter.miniatureise();
}


void Painting::rotate_clockwise(void) {
  original_image.rotate_clockwise();
  refresh();
}

void Painting::rotate_anticlockwise(void) {
  original_image.rotate_anticlockwise();
  refresh();
}
