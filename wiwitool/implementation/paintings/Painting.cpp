#include "paintings/Painting.hpp"

#include <print>

#include "paintings/Painting_converter.hpp"

#include "paintings/Painting_ratio.hpp"
#include "util/wiwidebug.hpp"

Painting::Painting(std::filesystem::path image_path)
    : Painting{Image_data{image_path}} {}

Painting::Painting(std::vector<uint8_t> image_data)
    : Painting{Image_data{image_data}} {}

Painting::Painting(Image_data image_data)
    : painting_id{next_painting_id++}, original_image{image_data},
      conversion_ratio{
          nearest_ratio(original_image.width(), original_image.height())} {

  wiwidebug std::println("Loaded image from image data (painting id = {})",
                         painting_id);

  refresh();
}

// Painting::Painting(const Painting &other) : Painting{other.original_image} {}

std::shared_ptr<Painting> Painting::clone(void) const {
  auto cloned = std::make_shared<Painting>(*this);
  cloned->painting_id = next_painting_id++;
  return cloned;
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
  painting = converter.convert(conversion_ratio);
  icon = converter.miniatureise();

  wiwidebug std::println("Painting and icon image data computed!");
}


void Painting::rotate_clockwise(void) {
  original_image = original_image.rotate_clockwise();
  conversion_ratio = opposite_ratio(conversion_ratio);

  refresh();
}

void Painting::rotate_anticlockwise(void) {
  original_image = original_image.rotate_anticlockwise();
  conversion_ratio = opposite_ratio(conversion_ratio);

  refresh();
}


#ifdef EMSCRIPTEN
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(painting) {

  // Register std::vector<uint8_t> so we can pass arrays from JS to
  // the Painting constructor
  register_vector<uint8_t>("PaintingBufferVector");

  // We bind Painting as a smart_ptr to handle the move-only nature of
  // the class safely in Javascript
  class_<Painting>("Painting")
      .smart_ptr<std::shared_ptr<Painting>>("Painting")

      .constructor(optional_override([](std::vector<uint8_t> data) {
        Image_data hires{std::move(data)};

        const double maxlen = std::max(hires.width(), hires.height());
        const double newmaxlen = 256; // in pixels

        wiwidebug std::println("Scaling image by factor of {} (maxlen = {})",
                               newmaxlen / maxlen, maxlen);

        return std::make_shared<Painting>(hires.scale(newmaxlen / maxlen));
      }))

      .function("clone", &Painting::clone)

      // Properties (Getters/Setters mapping to JS properties)
      .property("title", &Painting::get_title, &Painting::set_title)
      .property("author", &Painting::get_author, &Painting::set_author)
      .function("stringId", &Painting::string_id)

      .property(
          "ratio",
          // Getter
          +[](const Painting &self) -> std::string {
            return string_of_ratio(self.get_ratio());
          },
          // Setter
          +[](Painting &self, std::string ratio_str) {
            wiwidebug std::println("Setting ratio to ", ratio_str);
            self.set_ratio(ratio_of_string(ratio_str));
          })

      // Image Accessors
      // These return Image_data by value, which works because
      // Image_data is also registered in Javascript
      .function("originalData", &Painting::original_data)
      .function("paintingData", &Painting::painting_data)
      .function("iconData", &Painting::icon_data)

      // Transformations
      .function("rotateClockwise", &Painting::rotate_clockwise)
      .function("rotateAnticlockwise", &Painting::rotate_anticlockwise);

}
#endif
