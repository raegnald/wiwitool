<script lang="ts">
  import {
    paintingsStore,
    type PaintingWrapper,
  } from "../../stores/paintingsStore";
  import type { MainModule } from "../../bindings/wiwitool";

  import HelpUsingPaintingsTool from "./HelpUsingPaintingsTool.svelte";
  import DropZone from "./DropZone.svelte";
  import PaintingCard from "../PaintingCard.svelte";
  import BulkActions from "../BulkActions.svelte";
  import {
    CheckCheck,
    CircleCheck,
    CircleMinusIcon,
    InfoIcon,
    MinusIcon,
  } from "@lucide/svelte";

  export let module: MainModule;
  export let move: (id: string) => void;

  $: showingInfo = false;

  $: allSelected = $paintingsStore.every((painting) => painting.selected);

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

  function toggleSelectAll() {
    $paintingsStore.forEach((painting) => {
      painting.selected = !allSelected;
    });
    paintingsStore.update((paintings) => paintings);
  }
</script>

<div>
  <div class="title-component">
    <h2>Load your images</h2>
    <button onclick={() => (showingInfo = !showingInfo)}>
      Information <InfoIcon size="1em" />
    </button>
  </div>
  <div class="infoContainer app-card" class:showingInfo>
    <HelpUsingPaintingsTool />
  </div>
  <DropZone handler={handleImageDrop}>
    <span>Drag and drop images here</span>
  </DropZone>

  {#if $paintingsStore.length > 0}
    <div class="title-component">
      <h2>Customise your paintings</h2>
      <button onclick={toggleSelectAll}>
        {#if allSelected}
          Deselect all <CircleMinusIcon size="1.1em" />
        {:else}
          Select all <CircleCheck size="1.1em" />
        {/if}
      </button>
    </div>

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
  .infoContainer {
    display: none;
  }

  .infoContainer.showingInfo {
    display: block;
  }

  button {
    display: flex;
    align-items: center;
    gap: 5px;
  }
</style>
