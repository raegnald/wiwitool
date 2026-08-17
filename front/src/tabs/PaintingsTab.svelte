<script lang="ts">
  import { paintingsStore } from "../stores/paintingsStore";
  import {
    formatWorkspaceName,
    MAX_WORKSPACE_NAME_LENGTH,
    workspace,
  } from "../stores/workspaceStore";
  import DropZone from "../components/DropZone.svelte";
  import PaintingCard from "../paintings/PaintingCard.svelte";
  import BulkActions from "../paintings/BulkActions.svelte";
  import { ChevronRightIcon, ImageUpIcon } from "@lucide/svelte";
  import { onMount } from "svelte";
  import Button from "../components/Button.svelte";
  import ButtonListOptions from "../components/button-list/ButtonListOptions.svelte";
  import ListOption from "../components/button-list/ListOption.svelte";
  import ListSectionTitle from "../components/button-list/ListSectionTitle.svelte";
  import { ERROR, toast } from "../stores/toastsStore";
  import { fileService } from "../services/backendProvider";
  import Wiwinput from "../components/Wiwinput.svelte";
  import Wiwicheckbox from "../components/Wiwicheckbox.svelte";

  export let move: (id: string) => void;

  let paintingsNamespaceOverride: string = "";
  let namespaceOverrideDialog: HTMLDialogElement;

  $: showingInfo = false;
  $: allSelected =
    $paintingsStore.length > 0 &&
    $paintingsStore.every((painting) => painting.selected);
  $: selectedCount = $paintingsStore.filter((p) => p.selected).length;

  let selectedPaintingId: string | null = null;

  onMount(async () => {
    if ($workspace) {
      const ns = await $workspace.getPaintingsNamespaceOverride();
      if (ns) paintingsNamespaceOverride = await ns.get();
    }
  });

  const validImageExtensions = [
    "png",
    "jpeg",
    "jpg",
    "tga",
    "bmp",
    "psd",
    "gif",
    "hdr",
    "pic",
    "ppm",
    "pgm",
    "bin",
  ];

  async function openPicker() {
    if (!$workspace) return;

    try {
      const picked = await $fileService.openImageFile();
      if (!picked) return;

      const painting = await $workspace.addPainting(picked.path || picked.data);

      paintingsStore.update((current) => [
        ...current,
        { painting, cppPainting: painting, selected: false },
      ]);
    } catch (e) {
      console.error(e);
      toast(ERROR, "Could not load selected image.");
    }
  }

  async function handleImageDrop(file: File) {
    if (!$workspace) return;

    try {
      const buffer = await file.arrayBuffer();
      const painting = await $workspace.addPainting(new Uint8Array(buffer));

      await painting.setTitle(file.name.split(".")[0]);

      paintingsStore.update((current) => [
        ...current,
        { painting, cppPainting: painting, selected: false },
      ]);
    } catch (e) {
      console.error(e);
      toast(ERROR, `Could not load ${file.name}`);
    }
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
      <Wiwinput
        type="text"
        placeholder="Paintings namespace override"
        bind:value={paintingsNamespaceOverride}
        oninput={() => {
          paintingsNamespaceOverride = formatWorkspaceName(
            paintingsNamespaceOverride,
          );
        }}
        onchange={async () =>
          $workspace
            ?.getPaintingsNamespaceOverride()
            .then((ns) => ns?.set(paintingsNamespaceOverride))}
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
            ?.getPaintingsNamespaceOverride()
            .then((ns) => ns?.set(paintingsNamespaceOverride));
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

<div class="paintings-interface">
  <div class="left-pane">
    <div class="pane-header elements-beside space-between">
      <span>
        {$paintingsStore.length} paintings
        {#if selectedCount > 0}
          &middot; {selectedCount} selected
        {/if}
      </span>

      <div class="elements-beside">
        {#if selectedCount === 0}
          <!-- <Button icon="ImageUp" onclick={openPicker}>Add</Button> -->
          <Button icon="ImagePlus" onclick={openPicker} title="Add images" />
        {/if}

        <div style="display: flex; align-items: center; gap: 15px;">
          <ButtonListOptions transparent rightAligned icon="Ellipsis">
            <!-- <ListOption
              disabled={$paintingsStore.length === 0}
              icon={allSelected ? "CircleMinus" : "CircleCheck"}
              onclick={toggleSelectAll}
            >
              {allSelected ? "Deselect all" : "Select all"}
            </ListOption> -->

            <ListOption
              disabled={$paintingsStore.length === 0 || allSelected}
              icon="CircleCheck"
              onclick={() => {
                $paintingsStore.forEach((p) => (p.selected = true));
                paintingsStore.update((paintings) => paintings);
              }}
            >
              Select all
            </ListOption>

            <ListOption
              disabled={$paintingsStore.length === 0 || selectedCount === 0}
              icon="CircleMinus"
              onclick={() => {
                $paintingsStore.forEach((p) => (p.selected = false));
                paintingsStore.update((paintings) => paintings);
              }}
            >
              Deselect all
            </ListOption>

            <ListSectionTitle>Advanced options</ListSectionTitle>
            <ListOption onclick={() => namespaceOverrideDialog.showModal()}>
              Set custom namespace
            </ListOption>
          </ButtonListOptions>
        </div>
      </div>
    </div>

    <div class="paintings-list">
      {#each [...$paintingsStore].reverse() as wrapper (wrapper.painting.stringId())}
        {@const id = wrapper.painting.stringId()}
        {@const paintingSelected = selectedPaintingId === id}
        <!-- svelte-ignore a11y_click_events_have_key_events -->
        <!-- svelte-ignore a11y_no_static_element_interactions -->
        <div class="painting-list-entry" class:active={paintingSelected}>
          <div
            class="left"
            onclick={() => (selectedPaintingId = paintingSelected ? null : id)}
          >
            <!-- svelte-ignore a11y_missing_attribute -->
            {#await wrapper.iconUrl || wrapper.painting.getIconUrl() then icon}
              <img src={icon} />
            {/await}
            <span class="painting-title">{wrapper.painting.title}</span>
          </div>
          <div
            class="right"
            class:checked={wrapper.selected || selectedCount > 0}
          >
            <Wiwicheckbox
              small
              checked={wrapper.selected}
              onclick={() => (wrapper.selected = !wrapper.selected)}
            />
          </div>
        </div>
      {/each}
    </div>

    <BulkActions />

    {#if selectedCount === 0}
      <div class="paintings-drop-zone-container">
        <DropZone handler={handleImageDrop} classes="paintings-drop-zone">
          <div class="empty-state">
            <ImageUpIcon size="80" strokeWidth="1.1" class="empty-icon" />

            <h3>
              {#if $paintingsStore.length === 0}
                Load your images
              {:else}
                Load more images
              {/if}
            </h3>
            <!-- <center class="with-title pills-horizontal-container">
          <span class="pill">jpeg</span>
          <span class="pill">png</span>
          <span class="pill">bmp</span>
          <span class="pill">gif</span>
          <span class="pill">hdr</span>
        </center> -->
          </div>
        </DropZone>
      </div>
    {/if}
  </div>

  <main>
    {#if selectedPaintingId}
      {@const wrapper = $paintingsStore.find(
        (p) => selectedPaintingId === p.painting.stringId(),
      )}
      {#if wrapper}
        <div class="breadcrumbs-container">
          <span class="breadcrumb">
            <ImageUpIcon size="24px" /> Paintings
          </span>

          <span class="chevron"><ChevronRightIcon /></span>

          <span class="breadcrumb">
            {#await wrapper.iconUrl || wrapper.painting.getIconUrl() then src}
              <img
                {src}
                alt="Current painting icon"
                width="28px"
                height="28px"
                style="margin-top: -5px"
              />
            {/await}
            <span class="active-breadcrumb">{wrapper.painting.title}</span>
          </span>
        </div>

        <PaintingCard {wrapper} />
      {/if}
    {/if}
  </main>
</div>

<style>
  main {
    flex: 1;
    overflow: auto;
  }

  .paintings-interface {
    display: flex;
    height: 100%;
  }

  .left-pane {
    width: 300px;
    background-color: #f7f8ff;
    display: flex;
    flex-direction: column;
    border-right: 1px solid #caceff;

    @media (prefers-color-scheme: dark) {
      & {
        background-color: #222;
        border-right-color: #444;
      }
    }

    & .pane-header {
      margin: 10px 10px 10px 20px;
    }

    & .paintings-list {
      flex: 1;
      overflow-y: scroll;
      display: flex;
      flex-direction: column;
      gap: 3px;
      padding: 0 8px;
      user-select: none;
      -webkit-user-select: none;

      & .painting-list-entry {
        padding: 6px 8px 8px 12px;
        transition: background-color 50ms linear;
        border-radius: 8px;
        display: flex;
        justify-content: space-between;
        align-items: center;

        & .left {
          cursor: pointer;
          flex: 1;
          height: 36px;
          display: flex;
          align-items: center;
          gap: 6px;

          & .painting-title {
            display: inline-block;
            text-wrap: nowrap;
            text-overflow: ellipsis;
          }
        }

        & .right {
          opacity: 0;
        }

        & .right.checked {
          opacity: 1;
        }

        &:hover {
          background-color: rgba(0, 0, 0, 0.05);

          @media (prefers-color-scheme: dark) {
            & {
              background-color: #333;
            }
          }

          & .right {
            opacity: 1;
          }
        }

        &.active {
          background-color: #e7e9fd;

          @media (prefers-color-scheme: Dark) {
            & {
              background-color: #565663;
            }
          }

          & .right {
            opacity: 1;
          }
        }

        & img {
          width: 24px;
          height: 24px;
          margin-top: -4px;
          margin-bottom: -5px;
        }
      }
    }

    /*& :global(.paintings-drop-zone-container) {
      margin: 0px 20px 20px;
    }*/

    & :global(.paintings-drop-zone) {
      background-color: #edefff;
      border-radius: 0;
      border: none;

      & .empty-state {
        padding-top: 40px;
        padding-bottom: 40px;

        & h3 {
          color: #6b72a3;
          margin: 10px 0 0;
          font-weight: 500;
        }

        & :global(.empty-icon) {
          margin-bottom: 0;
          stroke: #6b72a3;
        }
      }

      @media (prefers-color-scheme: dark) {
        & {
          background-color: #1a1a1a;
        }

        & .empty-state h3 {
          color: #999;
        }

        & .empty-state :global(.empty-icon) {
          stroke: #999;
        }
      }
    }
  }

  .breadcrumbs-container {
    margin: 20px;
    display: flex;
    align-items: center;
    gap: 10px;

    & .breadcrumb {
      display: flex;
      align-items: center;
      gap: 6px;
    }

    & .active-breadcrumb {
      font-weight: 500;
    }

    & .chevron {
      opacity: 0.6;
      margin-bottom: -2px;
    }
  }
</style>
