<script lang="ts">
  import { paintingsStore } from "../../stores/paintingsStore";
  import type { MainModule } from "../../bindings/wiwitool";

  import HelpUsingPaintingsTool from "./HelpUsingPaintingsTool.svelte";
  import DropZone from "./DropZone.svelte";
  import PaintingCard from "../PaintingCard.svelte";
  import {
    RotateCcwIcon,
    RotateCwIcon,
    SquareIcon,
    Trash2Icon,
    TypeIcon,
    UserIcon,
  } from "svelte-feather-icons";

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

  $: hasSelected = $paintingsStore.some((painting) => painting.selected);
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
  {/if}

  <center>
    <button onclick={() => move("misc")}>
      {$paintingsStore.length > 0 ? "Next" : "Skip"}
    </button>
  </center>

  {#if hasSelected}
    <div id="bulk-tools">
      <button> <SquareIcon /> </button>
      <button> <RotateCcwIcon /> </button>
      <button> <RotateCwIcon /> </button>
      <button> <UserIcon /> </button>
      <button> <TypeIcon /> </button>
      <button class="delete"> <Trash2Icon /> </button>
    </div>
  {/if}
</div>

<style>
  #bulk-tools {
    position: sticky;
    bottom: 5px;
    gap: 5px;
    display: flex;
    width: 46px;
    flex-direction: column;
    padding: 5px;
    justify-content: center;
    align-items: center;
  }

  @media only screen and (min-width: 1200px) {
    #bulk-tools {
      transform: translate(-73px, -10px);
    }
  }
  #bulk-tools > button {
    height: 46px;
    width: 46px;
    padding: 0.5em;
    line-height: 0.9;
    align-items: center;
    vertical-align: center;
  }

  .delete {
    background-color: #ffcccc;
    border: 2px solid #ffaaaa;
  }
  .delete:hover {
    border-color: #ff5555;
  }

  @media (prefers-color-scheme: dark) {
    .delete {
      background-color: #e21f1f;
      color: #f4d9d9;
      border: 2px solid #f4d9d9;
    }
  }
</style>
