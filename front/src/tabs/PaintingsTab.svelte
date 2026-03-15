<script lang="ts">
  import { paintingsStore } from "../stores/paintingsStore";
  import type { MainModule } from "../bindings/wiwitool";
  import { workspace } from "../stores/workspaceStore";

  import HelpUsingPaintingsTool from "../paintings/HelpUsingPaintingsTool.svelte";
  import DropZone from "../components/DropZone.svelte";
  import PaintingCard from "../paintings/PaintingCard.svelte";
  import BulkActions from "../paintings/BulkActions.svelte";
  import { InfoIcon } from "@lucide/svelte";
  import { onMount } from "svelte";
  import Button from "../components/Button.svelte";

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
    <Button onclick={() => (showingInfo = !showingInfo)} icon="Info">
      Information
    </Button>
  </div>

  <div class="app-card" style={`display: ${showingInfo ? "block" : "none"}`}>
    <HelpUsingPaintingsTool />
  </div>

  <DropZone handler={handleImageDrop}>
    <span>Drag and drop images here</span>
  </DropZone>

  {#if $paintingsStore.length > 0}
    <div class="title-component">
      <h2>Customise your paintings</h2>

      <Button
        onclick={toggleSelectAll}
        icon={allSelected ? "CircleMinus" : "CircleCheck"}
      >
        {allSelected ? "Deselect" : "Select"} all
      </Button>
    </div>

    <div>
      {#each [...$paintingsStore].reverse() as wrapper (wrapper.cppPainting.stringId())}
        <PaintingCard {wrapper} />
      {/each}
    </div>
  {/if}

  <div class="pagination">
    <Button onclick={() => move("discs")} icon="ArrowRight">
      {$paintingsStore.length > 0 ? "Next" : "Skip"}
    </Button>
  </div>

  <BulkActions />
</div>

<style>
</style>
