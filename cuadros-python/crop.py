import imageio.v3 as iio
import numpy as np
from pathlib import Path
import listpicker
import sys, os

IMAGE_RATIOS = [
    (1, 1),
    (1, 2),
    (2, 1),
    (2, 2),
    (2, 3),
    (2, 4),
    (3, 2),
    (3, 3),
    (3, 4),
    (4, 2),
    (4, 3),
    (4, 4),
]

FRAMES_PATH = Path("frames/")

def frame(img, ratio):
    frame_img = load_frame(ratio)

    assert frame_img.shape[:2] == img.shape[:2], "Suspi image and frame must have the same size"
    assert frame_img.shape[-1] == 4, "Frame should be a 4 channel image"

    out = np.empty(img.shape, dtype=img.dtype)

    h, w, _ = img.shape
    for y in range(h):
        for x in range(w):
            frame_pixel = frame_img[y, x]
            image_pixel = img[y, x]
            frame_pixel_is_transparent = (frame_pixel[-1] == 0) # alpha is zero

            K = 3
            frame_pixel[-1] = min(frame_pixel[-1] * K, 255)

            out[y, x] = image_pixel if frame_pixel_is_transparent else frame_pixel[:3]

    return out


# def frame(img, ratio):
#     h, w, _ = img.shape
#     for y in range(h):
#         for x in range(w):
#             if y == 0 or y == h - 1 or x == 0 or x == w - 1:
#                 img[y, x] = [0, 0, 0]
#     return img

def ratio(image_size):
    return image_size[0] / image_size[1]

def nearest_ratio(image_size):
    return min(IMAGE_RATIOS, key=lambda r: abs(ratio(r) - ratio(image_size)))

def load_frame(ratio):
    w, h = ratio
    frame_path = FRAMES_PATH / f"{w}{h}.png"
    return iio.imread(frame_path, mode="RGBA")

def crop(image, ratio):
    img_h, img_w, _ = image.shape
    target_ratio = ratio[0] / ratio[1]
    current_ratio = img_w / img_h

    if current_ratio == target_ratio:
        return image

    # Imagen muy ancha (recortar lados)
    if current_ratio > target_ratio:
        new_w = int(img_h * target_ratio)
        x0 = (img_w - new_w) // 2
        return image[:, x0:(x0 + new_w)]

    # Imagen muy alta (recortar arriba y abajo)
    else:
        new_h = int(img_w / target_ratio)
        y0 = (img_h - new_h) // 2
        return image[y0:(y0 + new_h), :]

# Nearest-neighbour downscaling
def downscale(image, ratio, scale=16):
    target_w = ratio[0] * scale
    target_h = ratio[1] * scale

    img_h, img_w, channels = image.shape

    # Cada cuántos píxeles vamos a coger un sample
    step_x = img_w / target_w
    step_y = img_h / target_h

    out = np.empty((target_h, target_w, channels), dtype=image.dtype)

    for y in range(target_h):
        src_y = int(y * step_y)
        for x in range(target_w):
            src_x = int(x * step_x)
            out[y, x] = image[src_y, src_x]

    return out

def convert(img, ratio, output_filepath):
    img_height, img_width, _img_channels = img.shape

    cropped = crop(img, ratio)
    downscaled = downscale(cropped, ratio)
    framed = frame(downscaled, ratio)

    iio.imwrite(output_filepath, framed)

def interactive_conversion(image_filepath):
    img = iio.imread(image_filepath)
    img_height, img_width, _img_channels = img.shape

    p = Path(image_filepath)
    output_filepath = p.with_name(f"{p.stem}_cropped_downscaled_framed.png")

    default_ratio = nearest_ratio((img_width, img_height))

    print(f"Converting {image_filepath}...")

    options = {f"{default_ratio[0]}x{default_ratio[1]}": default_ratio} | \
              {"Skip": "Skip"} | \
              {f"{ratio[0]}x{ratio[1]}": ratio for ratio in IMAGE_RATIOS}
    choice = listpicker.pick("Pick a ratio:", list(options.keys()))

    if choice == "Skip":
        print("Skipping...\n")
        return

    ratio = options[choice]
    convert(img, ratio, output_filepath)

def convert_many_files(directory):
    for f in os.listdir(directory):
        if os.path.isfile(f) and Path(f).suffix.lower() in [".png", ".jpg", ".jpeg"]:
            interactive_conversion(f)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Give me a folder where images are stored")
    else:
        img_dir = sys.argv[1]
        convert_many_files(img_dir)
