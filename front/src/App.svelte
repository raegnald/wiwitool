<script lang="ts">
  import { onMount } from "svelte";
  import { getWasmModule, wasmReady } from "./wasm";
  import { paintingsStore } from "./stores";
  import PaintingCard from "./components/PaintingCard.svelte";
  import HelpUsingWiwitool from "./components/App/HelpUsingWiwitool.svelte";
  import DropZone from "./components/App/DropZone.svelte";

  let module = null;

  onMount(async () => {
    module = await getWasmModule();
  });

  async function handleImageDrop(file: File) {
    if (!module) return;

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

  function generatePack() {
    if (!module) return;

    const pack = new module.PaintingsPack();
    const paintingVector = new module.PaintingsVector();

    $paintingsStore.forEach((wrapper) => {
      paintingVector.push_back(wrapper.cppPainting);
    });

    pack.setPaintings(paintingVector);
    pack.generate();
    pack.compress();
    download();

    // Cleanup
    pack.delete();
    paintingVector.delete();
  }

  function download() {
    if (!module) return;

    try {
      const zipPath = "/packs.zip";
      const fileContent = module.FS.readFile(zipPath);

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
  }
</script>

<main>
  <div id="wiwi-logo-wide">
    <img src="/logo-wide.svg" alt="Wide logo" />
  </div>

  {#if !$wasmReady}
    <p>Loading webassembly module...</p>
  {:else}
    <HelpUsingWiwitool />

    <h2>1. Load your images</h2>

    <DropZone handler={handleImageDrop} />

    {#if $paintingsStore.length > 0}
      <h2>2. Customise your paintings</h2>

      <div class="list">
        {#each [...$paintingsStore].reverse() as wrapper (wrapper.id)}
          <PaintingCard {wrapper} />
        {/each}
      </div>

      <h2>3. Enjoy your paintings!</h2>
      <div id="generate-btn-container">
        <button onclick={generatePack} class="generate-btn">
          Generate Minecraft Pack
        </button>
      </div>
    {/if}
  {/if}
</main>

<style>
  main {
    margin-bottom: 200px;
  }

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

  .list {
    margin-bottom: 50px;
  }

  #generate-btn-container {
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

  @media (prefers-color-scheme: dark) {
    #wiwi-logo-wide img {
      content: url("/logo-wide-dark.svg");
    }
  }
</style>
