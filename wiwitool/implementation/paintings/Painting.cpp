#include "paintings/Painting.hpp"

#include <print>

#include "nlohmann/json.hpp"

#include "paintings/Painting_converter.hpp"

#include "paintings/Painting_frame_generator.hpp"
#include "paintings/Painting_ratio.hpp"
#include "util/wiwidebug.hpp"

Painting::Painting(void) : painting_id{next_painting_id++} {}

Painting::Painting(std::filesystem::path image_path)
    : Painting{Image_data{image_path}} {}

Painting::Painting(std::vector<uint8_t> image_data)
    : Painting{Image_data{image_data}} {}

Painting::Painting(Image_data image_data)
    : painting_id{next_painting_id++}, original_image{image_data},
      conversion_ratio{Painting_ratio::nearest_to(original_image.size())} {

  wiwidebug std::println("Loaded image from image data (painting id = {})",
                         painting_id);

  invalidate_cache();
}

std::shared_ptr<Painting> Painting::clone(void) const {
  auto cloned = std::make_shared<Painting>(*this);
  cloned->painting_id = next_painting_id++;

  wiwidebug {
    auto [ow, oh] = this->get_ratio().sizes();
    auto [w, h] = cloned->get_ratio().sizes();

    std::println("Cloned painting with ratio {}x{}, while original has {}x{}",
                 w, h, ow, oh);
  }

  return cloned;
}

const Image_data Painting::original_data(void) const { return original_image; }

const Image_data Painting::cropped_original_data(void) const {
  return original_image.crop(
      static_cast<size_t>(crop_x * original_image.width()),
      static_cast<size_t>(crop_y * original_image.height()),
      static_cast<size_t>(crop_w * original_image.width()),
      static_cast<size_t>(crop_h * original_image.height()));
}

const Image_data Painting::painting_data(void) const {
  compute_painting_and_icon_if_necessary();
  return painting;
}

const Image_data Painting::icon_data(void) const {
  compute_painting_and_icon_if_necessary();
  return icon;
}

void Painting::refresh(void) const {
  auto cropped_source = original_image.crop(
      static_cast<size_t>(crop_x * original_image.width()),
      static_cast<size_t>(crop_y * original_image.height()),
      static_cast<size_t>(crop_w * original_image.width()),
      static_cast<size_t>(crop_h * original_image.height()));

  auto converter = Painting_converter{cropped_source};
  icon = converter.miniatureise();
  painting = converter.convert(frame_generator, pixels_per_block,
                               conversion_ratio, contrast, brightness);

  wiwidebug std::println("Painting and icon image data computed!");
}

void Painting::rotate_clockwise(void) {
  original_image = original_image.rotate_clockwise();
  conversion_ratio = conversion_ratio.opposite();

  // Transform crop coordinates
  double old_x = crop_x, old_y = crop_y, old_w = crop_w, old_h = crop_h;
  crop_x = 1.0 - (old_y + old_h);
  crop_y = old_x;
  crop_w = old_h;
  crop_h = old_w;

  invalidate_cache();
}

void Painting::rotate_anticlockwise(void) {
  original_image = original_image.rotate_anticlockwise();
  conversion_ratio = conversion_ratio.opposite();

  // Transform crop coordinates
  double old_x = crop_x, old_y = crop_y, old_w = crop_w, old_h = crop_h;
  crop_x = old_y;
  crop_y = 1.0 - (old_x + old_w);
  crop_w = old_h;
  crop_h = old_w;

  invalidate_cache();
}

void Painting::use_procedural_frame() {
  frame_generator = Procedural_frame_generator{};
  invalidate_cache();
}

void Painting::use_no_frame() {
  frame_generator = No_frame_generator{};
  invalidate_cache();
}

Procedural_frame_generator *Painting::get_procedural_settings(void) {
  return std::get_if<Procedural_frame_generator>(&frame_generator);
}

bool Painting::is_frame_procedural(void) const {
  return std::holds_alternative<Procedural_frame_generator>(frame_generator);
}

bool Painting::is_frame_nonexistent(void) const {
  return std::holds_alternative<No_frame_generator>(frame_generator);
}


void to_json(nlohmann::json &j, const Painting &p) {
  j = nlohmann::json{{"id", p.painting_id},
                     {"title", p.title},
                     {"author", p.author},
                     {"ratio", p.conversion_ratio},
                     {"sourceImage", p.original_image},
                     {"placeable", p.placeable},
                     {"hasStonecutterRecipe", p.stonecutter_recipe},
                     {"crop", {p.crop_x, p.crop_y, p.crop_w, p.crop_h}},
                     {"contrast", p.contrast},
                     {"brightness", p.brightness}};

  std::visit([&j](const auto &gen) { j["frame_generator"] = gen; },
             p.frame_generator);
}

void from_json(const nlohmann::json &j, Painting &p) {
  j.at("id").get_to(p.painting_id);

  // Prevent string_id() collisions after loading an existing pack
  if (p.painting_id >= Painting::next_painting_id)
    Painting::next_painting_id = p.painting_id + 1;

  j.at("title").get_to(p.title);
  j.at("author").get_to(p.author);
  j.at("ratio").get_to(p.conversion_ratio);
  j.at("sourceImage").get_to(p.original_image);

  if (j.contains("placeable"))
    j.at("placeable").get_to(p.placeable);

  if (j.contains("hasStonecutterRecipe"))
    j.at("hasStonecutterRecipe").get_to(p.stonecutter_recipe);

  if (j.contains("crop")) {
    auto crop = j.at("crop").get<std::vector<double>>();
    if (crop.size() == 4) {
      p.crop_x = crop[0];
      p.crop_y = crop[1];
      p.crop_w = crop[2];
      p.crop_h = crop[3];
    }
  }

  if (j.contains("contrast"))
    j.at("contrast").get_to(p.contrast);

  if (j.contains("brightness"))
    j.at("brightness").get_to(p.brightness);

  if (j.contains("frame_generator")) {
    const auto &j_gen = j.at("frame_generator");

    if (j_gen.value("type", "default") == "procedural") {
      p.frame_generator = j_gen.get<Procedural_frame_generator>();
    }

  } else {
    wiwidebug std::println("No frame_generator found whilst importing");
    p.frame_generator = {};
  }

  p.refresh();
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

  // We bind Painting as a smart_ptr to handle the shared reference
  // nature of the class safely in Javascript
  class_<Painting>("Painting")
      .smart_ptr<std::shared_ptr<Painting>>("Painting")

      .constructor(optional_override([](emscripten::val js_array) {
        size_t length = js_array["length"].as<size_t>();
        std::vector<uint8_t> vec(length);
        emscripten::val memory_view = emscripten::val(emscripten::typed_memory_view(length, vec.data()));
        memory_view.call<void>("set", js_array);

        Image_data hires{std::move(vec)};

        const double maxlen = std::max(hires.width(), hires.height());
        const double newmaxlen = 1024; // in pixels

        wiwidebug std::println("Scaling image by factor of {} (maxlen = {})",
                               newmaxlen / maxlen, maxlen);

        auto downscaled = hires.scale(newmaxlen / maxlen);

        wiwidebug std::println("Scaled image successfully");

        return std::make_shared<Painting>(downscaled);
      }))

      .function("clone", &Painting::clone)

      // Properties (Getters/Setters mapping to JS properties)
      .property("title", &Painting::get_title, &Painting::set_title)
      .property("author", &Painting::get_author, &Painting::set_author)
      .function("stringId", &Painting::string_id)

      .function("setCrop", &Painting::set_crop)
      .property("cropX", &Painting::get_crop_x)
      .property("cropY", &Painting::get_crop_y)
      .property("cropW", &Painting::get_crop_w)
      .property("cropH", &Painting::get_crop_h)

      .property(
          "ratio",
          +[](const Painting &self) -> std::string {
            return self.get_ratio().to_string();
          },
          +[](Painting &self, std::string ratio_str) {
            self.set_ratio(Painting_ratio::from_string(ratio_str));
          })

      // Image Accessors
      // These return Image_data by value, which works because
      // Image_data is also registered in Javascript
      .function("originalData", &Painting::original_data)
      .function("croppedOriginalData", &Painting::cropped_original_data)
      .function("paintingData", &Painting::painting_data)
      .function("iconData", &Painting::icon_data)

      // Transformations
      .function("rotateClockwise", &Painting::rotate_clockwise)
      .function("rotateAnticlockwise", &Painting::rotate_anticlockwise)

      .property("contrast", &Painting::get_contrast, &Painting::set_contrast)
      .property("brightness", &Painting::get_brightness, &Painting::set_brightness)

      .function("useProceduralFrame", &Painting::use_procedural_frame)
      .function("useNoFrame", &Painting::use_no_frame)
      .function("getProceduralSettings", &Painting::get_procedural_settings,
                allow_raw_pointers())
      .function("isFrameProcedural", &Painting::is_frame_procedural)
      .function("isFrameNonexistent", &Painting::is_frame_nonexistent)

      .property("placeable", &Painting::is_placeable, &Painting::set_placeable)
      .property("hasStonecutterRecipe", &Painting::has_stonecutter_recipe,
                &Painting::set_has_stonecutter_recipe)
      .property("pixelPerBlock", &Painting::get_pixels_per_block,
                &Painting::set_pixels_per_block)

      .function("refresh", &Painting::refresh);
}
#endif
