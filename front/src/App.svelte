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
  <h1>Minecraft Painting Generator</h1>

  {#if !$wasmReady}
    <p>Loading Wasm Module...</p>
  {:else}
    <button
      class="drop-zone"
      class:hover={isDragOver}
      on:drop={handleDrop}
      on:dragover={handleDragOver}
      on:dragleave={handleDragLeave}
    >
      <p>Drag and drop images here</p>
    </button>

    <div class="list">
      {#each $paintingsStore as wrapper (wrapper.id)}
        <PaintingCard {wrapper} />
      {/each}
    </div>

    {#if $paintingsStore.length > 0}
      <button on:click={generatePack} class="generate-btn">
        Generate Minecraft Pack
      </button>
    {/if}
  {/if}
</main>

<style>
  .drop-zone {
    display: block;
    width: 100%;
    cursor: default;
    border: 2px dashed #aaa;
    padding: 2rem;
    text-align: center;
    margin-bottom: 2rem;
    transition: 0.2s;
  }
  .drop-zone.hover {
    background-color: #f0f8ff;
    border-color: #007bff;
  }
  .list {
    display: flex;
    flex-wrap: wrap;
  }
  .generate-btn {
    font-size: 1.5rem;
    padding: 10px 20px;
    background: #28a745;
    color: white;
    border: none;
    cursor: pointer;
  }
</style>
