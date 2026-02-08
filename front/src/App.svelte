<script>
  import { onMount } from "svelte";
  import { getWasmModule, wasmReady } from "./wasm";
  import { paintingsStore } from "./stores";
  import PaintingCard from "./components/PaintingCard.svelte";

  let module = null;
  let isDragOver = false;

  onMount(async () => {
    module = await getWasmModule();
  });

  async function handleDrop(e) {
    e.preventDefault();
    isDragOver = false;
    if (!module) return;

    const files = e.dataTransfer.files;

    for (const file of files) {
      const buffer = await file.arrayBuffer();
      const uint8View = new Uint8Array(buffer);

      // 1. Allocate memory in WASM for the vector
      // You registered 'PaintingBufferVector' in bindings
      const vec = new module.PaintingBufferVector();

      // Push data into the vector
      for (let i = 0; i < uint8View.length; i++) {
        vec.push_back(uint8View[i]);
      }

      // 2. Create the Painting C++ object
      // The binding: .constructor(optional_override([](std::vector<uint8_t> data)...
      const painting = new module.Painting(vec);

      // Clean up the temporary vector
      vec.delete();

      // 3. Set default metadata
      const name = file.name.split(".")[0];
      painting.title = name;
      painting.author = "User";

      // 4. Add to store
      paintingsStore.update((current) => [
        ...current,
        {
          id: Math.random(), // Simple ID for UI keys
          title: name,
          author: "User",
          originalImageBytes: uint8View,
          cppPainting: painting,
        },
      ]);
    }
  }

  function handleDragOver(e) {
    e.preventDefault();
    isDragOver = true;
  }
  function handleDragLeave() {
    isDragOver = false;
  }

  function generatePack() {
    if (!module) return;

    const pack = new module.PaintingsPack();
    const paintingVector = new module.PaintingsVector();

    // Fill vector with current C++ objects from the store
    $paintingsStore.forEach((wrapper) => {
      paintingVector.push_back(wrapper.cppPainting);
    });

    console.log(pack);

    pack.setPaintings(paintingVector);
    pack.generate(); // Generates files in MEMFS at /tmp/datapack and /tmp/respack
    pack.compress(); // Generates .zip files in /tmp

    // --- DOWNLOAD LOGIC ---
    // Read the file from Emscripten's Virtual File System
    try {
      const zipPath = "/packs.zip"; // path defined in your C++ compress()
      const fileContent = module.FS.readFile(zipPath);

      // Create a blob and download
      const blob = new Blob([fileContent], { type: "application/zip" });
      const url = URL.createObjectURL(blob);
      const a = document.createElement("a");
      a.href = url;
      a.download = "paintings_pack.zip";
      a.click();
      URL.revokeObjectURL(url);
    } catch (e) {
      console.error("Error reading generated file:", e);
      alert("Generation finished, but failed to read zip");
    }

    // Cleanup
    pack.delete();
    paintingVector.delete();
  }
</script>

<main>
  <div id="wiwi-logo-wide">
    <img src="/logo-wide.svg" alt="Wide logo" />
  </div>

  {#if !$wasmReady}
    <p>Loading Wasm Module...</p>
  {:else}
    <h2>What is this tool all about?</h2>
    <p>
      This is a web tool for creating <b
        >custom painting items in Minecraft out of your own images</b
      >. You drag and drop all the images you want to have as Minecraft painting
      items, and this tool bundles them into two files:
      <code>datapack.zip</code>
      and <code>respack.zip</code>.
    </p>

    <img
      clas="how-to-use-wiwitool-img"
      src="/flow.png"
      alt="Workflow of using the wiwitool"
    />

    <p>
      The paitings you get can be crafted out of a stonecutter. You can any of
      your paintings by adding any painting to a stonecutter.
    </p>

    <h3>Wiwitool features</h3>

    <ul>
      <li>
        Open-source software (<a
          href="https://github.com/raegnald/wiwitool/"
          target="_blank">Github repository</a
        >). Free to use forever.
      </li>
      <li>
        Runs locally on your browser (all your images are kept on <i>your</i> machine.)
      </li>
      <li>
        You can manipulate and preview how your image will be seen in Minecraft.
      </li>
    </ul>

    <h2>1. Load your images</h2>

    <button
      class="drop-zone"
      class:hover={isDragOver}
      on:drop={handleDrop}
      on:dragover={handleDragOver}
      on:dragleave={handleDragLeave}
    >
      <p>Drag and drop images here</p>
    </button>

    {#if $paintingsStore.length > 0}
      <h2>2. Customise your paintings</h2>
    {/if}

    <div class="list">
      {#each $paintingsStore as wrapper (wrapper.id)}
        <PaintingCard {wrapper} />
      {/each}
    </div>

    {#if $paintingsStore.length > 0}
      <div id="generate-btn-container">
        <button on:click={generatePack} class="generate-btn">
          Generate Minecraft Pack
        </button>
      </div>
    {/if}
  {/if}
</main>

<style>
  #wiwi-logo-wide {
    display: flex;
    justify-content: center;
    margin-bottom: 25px;
  }

  #wiwi-logo-wide img {
    margin: 50px 0;
    width: 100%;
    max-width: 768px;
  }
  .drop-zone {
    display: block;
    width: 100%;
    cursor: default;
    border: 2px dashed #aaa;
    padding: 6rem;
    text-align: center;
    margin-bottom: 2rem;
    transition: 0.2s;
  }
  .drop-zone.hover {
    background-color: #f0f8ff;
    border-color: #007bff;
  }

  .list {
    /*display: flex;
    flex-wrap: wrap;*/
    margin-bottom: 200px;
  }

  #generate-btn-container {
    position: sticky;
    z-index: 100;
    bottom: 30px;
    width: 100%;
    max-width: 1024px;
    display: flex;
    justify-content: center;
  }

  .generate-btn {
    font-size: 1.5rem;
    padding: 10px 20px;
    background: #28a745;
    color: white;
    border: none;
    cursor: pointer;
    box-shadow: 0 2px 16px rgba(0, 0, 0, 0.3);
  }

  @media (prefers-color-scheme: light) {
    .drop-zone {
      background-color: #e7f0a5;
    }
  }
</style>
