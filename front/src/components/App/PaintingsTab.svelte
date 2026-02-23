<script lang="ts">
  import {
    paintingsStore,
    type PaintingWrapper,
  } from "../../stores/paintingsStore";
  import type { MainModule } from "../../bindings/wiwitool";
  import { workspace } from "../../stores/workspaceStore";

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
  import { onMount } from "svelte";

  export let module: MainModule;
  export let move: (id: string) => void;

  $: showingInfo = false;
  $: allSelected = $paintingsStore.every((painting) => painting.selected);

  onMount(() => {
    // Ask the user for confirmation when reloading the page
    window.addEventListener("beforeunload", (event: Event) => {
      event.preventDefault();
      event.returnValue = false;
    });
  });

  async function handleImageDrop(file: File) {
    if (!module || !$workspace) return;

    const buffer = await file.arrayBuffer();
    const uint8View = new Uint8Array(buffer);

    const vec = new module.PaintingBufferVector();
    for (let i = 0; i < uint8View.length; i++) vec.push_back(uint8View[i]);

    const cppPainting = new module.Painting(vec);
    vec.delete();

    cppPainting.title = file.name.split(".")[0];
    cppPainting.author = "";

    // Add to C++ workspace
    $workspace.addPainting(cppPainting);

    // Add to Svelte store
    paintingsStore.update((current) => [
      ...current,
      { cppPainting, selected: false },
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
      {#each [...$paintingsStore].reverse() as wrapper (wrapper.cppPainting.stringId())}
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
    margin-bottom: 20px;
  }

  button {
    display: flex;
    align-items: center;
    gap: 5px;
  }
</style>
