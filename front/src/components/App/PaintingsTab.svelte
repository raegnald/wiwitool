<script lang="ts">
  import { paintingsStore } from "../../stores/paintingsStore";
  import type { MainModule } from "../../bindings/wiwitool";

  import HelpUsingPaintingsTool from "./HelpUsingPaintingsTool.svelte";
  import DropZone from "./DropZone.svelte";
  import PaintingCard from "../PaintingCard.svelte";

  export let module: MainModule;
  export let move: (id: string) => void;

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

<div>
  <HelpUsingPaintingsTool />

  <h2>1. Load your images</h2>

  <DropZone handler={handleImageDrop}>
    <p>Drag and drop images here</p>
  </DropZone>

  {#if $paintingsStore.length > 0}
    <h2>2. Customise your paintings</h2>

    <div class="list">
      {#each [...$paintingsStore].reverse() as wrapper (wrapper.id)}
        <PaintingCard {wrapper} />
      {/each}
    </div>

    <!-- <button onclick={generatePack} class="generate-btn">
        Generate Minecraft Pack
      </button> -->
  {/if}

  <center>
    <button onclick={() => move("misc")}
      >{$paintingsStore.length > 0 ? "Next" : "Skip"}</button
    >
  </center>
</div>

<style>
  .list {
    margin-bottom: 50px;
  }

  /*#generate-btn-container {
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
    cursor: pointer;
    box-shadow: 0 2px 16px rgba(0, 0, 0, 0.3);
  }*/
</style>
