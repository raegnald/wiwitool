<script lang="ts">
  import { paintingsStore } from "../../stores/paintingsStore";
  import type { MainModule } from "../../bindings/wiwitool";

  import HelpUsingPaintingsTool from "./HelpUsingPaintingsTool.svelte";
  import DropZone from "./DropZone.svelte";
  import PaintingCard from "../PaintingCard.svelte";
  import BulkActions from "../BulkActions.svelte";

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
        selected: false,
        crop: null,
      },
    ]);
  }
</script>

<div>
  <HelpUsingPaintingsTool />

  <h2>1. Load your images</h2>

  <DropZone handler={handleImageDrop}>
    <span>Drag and drop images here</span>
  </DropZone>

  {#if $paintingsStore.length > 0}
    <h2>2. Customise your paintings</h2>

    <div class="list">
      {#each [...$paintingsStore].reverse() as wrapper (wrapper.id)}
        <PaintingCard {wrapper} />
      {/each}
    </div>
  {/if}

  <center>
    <button onclick={() => move("misc")}>
      {$paintingsStore.length > 0 ? "Next" : "Skip"}
    </button>
  </center>

  <BulkActions />
</div>

<style>
</style>
