# The Wiwitool 🪿

**The Wiwitool** (https://wiwitool.gligan.net) is a free, open-source, browser-based generator that allows Minecraft players to effortlessly bring their own custom artwork and music into the game. 

Instead of manually wrestling with complex directory structures, JSON file linking, and audio format conversions, users can simply drag and drop their files into a clean web interface to instantly generate fully functional Minecraft Resource and Data packs.

Everything runs **entirely locally on your machine**. By utilising WebAssembly to handle heavy processing tasks—like audio encoding and image manipulation—directly in your browser, you never have to upload private files to a server or wait in processing queues.

## ✨ Features ✨

### Custom Paintings 🖼️
* **Drag & Drop Upload:** Upload standard images (`.png`, `.jpg`, etc.).
* **In-Game Customization:** Set custom aspect ratios, titles, and artist names.
* **Procedural Frames:** Apply customizable, procedurally generated Perlin-noise frames to create high-quality, immersive in-game artwork. (Or toggle them off for borderless art!)
* **Survival Friendly:** Obtain your custom paintings naturally by placing standard vanilla paintings into a **Stonecutter**.

### Custom Music Discs 🎵
* **Auto-Encoding:** Drop in standard audio files; the Wiwitool automatically encodes them to Minecraft's required `.ogg` format.
* **Track Editing:** Trim audio tracks and set custom names/artists.
* **Record Sleeves:** Assign custom images to act as the item texture for the disc.
* **Redstone Integration:** Manually modify the redstone comparator output signal for each custom music disc.
* **Survival Friendly:** Place any vanilla music disc into a **Stonecutter** to craft your custom tracks.

### Quality-of-Life Additions 🪄
* **Invisible Item Frames:** Includes a toggleable datapack feature that adds simple shapeless crafting recipes for Invisible Item Frames and Invisible Glowing Item Frames (using glass panes, fermented spider eyes, and golden carrots).

---

## For Users: How to Use

1. **Import Media:** Drag and drop your images or audio files into the respective tabs.
2. **Customize:** Tweak the titles, trim the audio, change the aspect ratios, or adjust the procedural painting frames.
3. **Generate:** Click "Download Pack" in the Generate tab. 
4. **Install:** You will receive a `wiwipacks.zip` file containing:
   * `respack.zip`: Put this in your Minecraft `resourcepacks` folder and enable it in-game.
   * `datapack.zip`: Put this in your world's `datapacks` folder (requires a world reload/relog).
   * `Wiwitool Project Save file`: **Keep this safe!** You can drag and drop this file back into the Wiwitool later to instantly reload your workspace, edit your items, and add more to your pack.

---

## For Developers: Architecture & Tech Stack

The Wiwitool combines a relatively high-performant C++ backend with a reactive JavaScript frontend, fully contained within the browser using WebAssembly.

### Tech Stack
* **Frontend:** [Svelte 5](https://svelte.dev/) (with Vite) and TypeScript.
* **Backend (WASM):** C++23 compiled via [Emscripten](https://emscripten.org/).
* **Interop:** `embind` and `emscripten::val` (utilising raw memory views for zero-copy binary transfers between JS and C++).
* **Core Libraries (C++):**
  * `nlohmann/json` (used with binary MessagePack for fast project saving/loading).
  * `stb_image` / `stb_image_write` / `stb_perlin` (image processing and procedural generation).
  * `libvorbis` / `libogg` / `stb_vorbis` (audio decoding and `.ogg` encoding).
  * `miniz_cpp` (in-memory ZIP archive generation).

### Directory Structure

```text
wiwitool/
├── front/                      # Svelte UI Application
│   ├── src/
│   │   ├── bindings/           # TypeScript definitions for the C++ WASM module
│   │   ├── components/         # Reusable Svelte UI components
│   │   ├── stores/             # Svelte stores managing UI state and the C++ Workspace instance
│   │   ├── paintings/          # Painting-specific UI logic & canvas rendering
│   │   └── music/              # Music-specific UI logic & waveform rendering
│   └── vite.config.js          
├── wiwitool/                   # C++ Core Logic
│   ├── include/                # Header files (.hpp)
│   ├── implementation/         # Source files (.cpp) and Embind definitions
│   │   ├── music/              # Audio encoding & disc JSON generation
│   │   ├── paintings/          # Image scaling, framing, & variant JSON generation
│   │   └── ...                 # Pack bundling, serialisation, etc.
│   └── external/               # Vendored C++ libraries (stb, json, miniz)
└── Makefile                    # Build scripts for native (unmaintained) and WASM compilation (default)
```

### Local Development Setup

To build and run the Wiwitool locally, you will need **Node.js/npm**, **Make**, and the **Emscripten SDK (`emsdk`)**.

```sh
make run-dev
```

## License

Please refer to the file `LICENSE` located alongside this one for the project's license.
