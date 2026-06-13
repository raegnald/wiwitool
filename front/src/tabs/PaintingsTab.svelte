<script lang="ts">
  import { paintingsStore } from "../stores/paintingsStore";
  import type { MainModule } from "../bindings/wiwitool";
  import {
    formatWorkspaceName,
    MAX_WORKSPACE_NAME_LENGTH,
    workspace,
  } from "../stores/workspaceStore";

  import HelpUsingPaintingsTool from "../paintings/HelpUsingPaintingsTool.svelte";
  import DropZone from "../components/DropZone.svelte";
  import PaintingCard from "../paintings/PaintingCard.svelte";
  import BulkActions from "../paintings/BulkActions.svelte";
  import {
    ImageUpIcon,
    InfoIcon,
    LogOut,
    Settings,
    User,
  } from "@lucide/svelte";
  import { onMount } from "svelte";
  import Button from "../components/Button.svelte";
  import ButtonListOptions from "../components/button-list/ButtonListOptions.svelte";
  import ListOption from "../components/button-list/ListOption.svelte";
  import ListSectionTitle from "../components/button-list/ListSectionTitle.svelte";

  export let module: MainModule;
  export let move: (id: string) => void;

  let paintingsNamespaceOverride: string = "";
  let namespaceOverrideDialog: HTMLDialogElement;

  $: showingInfo = false;
  $: allSelected = $paintingsStore.every((painting) => painting.selected);
  $: selectedCount = $paintingsStore.filter((p) => p.selected).length;

  onMount(() => {
    // Ask the user for confirmation when reloading the page
    window.addEventListener("beforeunload", (event: Event) => {
      event.preventDefault();
      event.returnValue = false;
    });

    paintingsNamespaceOverride = $workspace
      .getPaintingsNamespaceOverride()
      .get();
  });

  async function handleImageDrop(file: File) {
    if (!module || !$workspace) return;

    const buffer = await file.arrayBuffer();
    const uint8View = new Uint8Array(buffer);

    const cppPainting = new module.Painting(uint8View as any);

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

<dialog bind:this={namespaceOverrideDialog}>
  <div class="modal-content">
    {#if paintingsNamespaceOverride.length > 0}
      <span>Custom namespace will be used</span>
    {:else}
      <span>Using default namespace</span>
    {/if}

    <div>
      <input
        type="text"
        placeholder="Paintings namespace override"
        bind:value={paintingsNamespaceOverride}
        on:input={() => {
          paintingsNamespaceOverride = formatWorkspaceName(
            paintingsNamespaceOverride,
          );
        }}
        on:change={() =>
          $workspace
            .getPaintingsNamespaceOverride()
            .set(paintingsNamespaceOverride)}
      />

      <span
        style={"display: inline-block; width: 60px;" +
          (paintingsNamespaceOverride.length >= MAX_WORKSPACE_NAME_LENGTH
            ? "color: orange"
            : "")}
      >
        {#if paintingsNamespaceOverride.length > 0}
          <span style="display: inline-block; width: 25px; text-align: right;">
            {paintingsNamespaceOverride.length}
          </span>
          / {MAX_WORKSPACE_NAME_LENGTH}
        {/if}
      </span>
    </div>

    <div class="modal-actions">
      <Button
        secondary
        onclick={() => {
          paintingsNamespaceOverride = "";
          $workspace
            .getPaintingsNamespaceOverride()
            .set(paintingsNamespaceOverride);
        }}
        disabled={paintingsNamespaceOverride == ""}
      >
        Clear
      </Button>

      <Button
        primary
        onclick={() => {
          namespaceOverrideDialog.close();
        }}
      >
        Close
      </Button>
    </div>
  </div>
</dialog>

<main>
  <div class="app-card drop-zone-container">
    <DropZone handler={handleImageDrop} let:filePicker>
      <div class="empty-state">
        <ImageUpIcon size="80" strokeWidth="1.5" class="empty-icon" />

        <h2>Load your images</h2>
        <center class="with-title pills-horizontal-container">
          <span class="pill">jpeg</span>
          <span class="pill">png</span>
          <span class="pill">bmp</span>
          <span class="pill">gif</span>
          <span class="pill">hdr</span>
        </center>

        <div class="empty-actions">
          <Button icon="FolderInput" onclick={filePicker}>
            Select from file picker
          </Button>
        </div>
      </div>
    </DropZone>

    <center>
      <Button
        secondary
        hugeBorder={showingInfo}
        onclick={() => (showingInfo = !showingInfo)}
        icon="Info"
      >
        Information
      </Button>
    </center>

    <div
      class="instructions-box"
      style={`display: ${showingInfo ? "block" : "none"}`}
    >
      <HelpUsingPaintingsTool />
    </div>
  </div>

  {#if $paintingsStore.length > 0}
    <div class="title-component" style="margin-top: 2em">
      <h2>Customise your paintings</h2>

      <div style="display: flex; align-items: center; gap: 15px;">
        {#if selectedCount > 0}
          <span style="font-size: 110%">{selectedCount} selected</span>
        {/if}

        <ButtonListOptions rightAligned icon="Ellipsis">
          <ListOption
            icon={allSelected ? "CircleMinus" : "CircleCheck"}
            onclick={toggleSelectAll}
          >
            {allSelected ? "Deselect" : "Select"} all
          </ListOption>

          <ListSectionTitle>Advanced options</ListSectionTitle>
          <ListOption onclick={() => namespaceOverrideDialog.showModal()}>
            Set custom namespace
          </ListOption>
        </ButtonListOptions>
      </div>
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
</main>

<style>
  .drop-zone-container {
    display: flex;
    flex-direction: column;
    gap: 20px;
  }
</style>
