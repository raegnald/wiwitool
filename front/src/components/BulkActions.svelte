<script lang="ts">
  import {
    ProportionsIcon,
    RotateCcwIcon,
    RotateCwIcon,
    UserIcon,
    TextCursorIcon,
    Trash2Icon,
  } from "@lucide/svelte";
  import { paintingsStore } from "../stores/paintingsStore";
  import RatioSelect from "./PaintingCard/RatioSelect.svelte";

  $: hasSelected = $paintingsStore.some((painting) => painting.selected);

  function rotateSelectedCCW() {
    $paintingsStore.forEach((painting) => {
      if (painting.selected) {
        painting.cppPainting.rotateAnticlockwise();
      }
    });
    paintingsStore.update((paintings) => paintings);
  }

  function rotateSelectedCW() {
    $paintingsStore.forEach((painting) => {
      if (painting.selected) {
        painting.cppPainting.rotateClockwise();
      }
    });
    paintingsStore.update((paintings) => paintings);
  }

  function deleteSelected() {
    paintingsStore.update((paintings) =>
      paintings.filter((painting) => {
        let shouldDelete = painting.selected;
        if (shouldDelete) painting.cppPainting.delete();
        return !shouldDelete;
      }),
    );
  }

  let dialog: HTMLDialogElement;
  let editMode: "title" | "author" | "aspect ratio" | "delete selected";
  let bulkInput = "";

  function openModal(
    mode: "title" | "author" | "aspect ratio" | "delete selected",
  ) {
    editMode = mode;
    bulkInput = ""; // clear previous input
    dialog.showModal();
  }

  function applyBulkEdit() {
    paintingsStore.update((paintings) => {
      paintings.forEach((p) => {
        if (p.selected) {
          if (editMode === "title") {
            p.title = bulkInput;
            p.cppPainting.title = bulkInput;
          } else if (editMode == "author") {
            p.author = bulkInput;
            p.cppPainting.author = bulkInput;
          } else if (editMode == "aspect ratio") {
            p.cppPainting.ratio = bulkInput;
          }
        }
      });
      return paintings;
    });
    dialog.close();
  }
</script>

<dialog bind:this={dialog}>
  {#if editMode == "delete selected"}
    <div class="modal-content">
      <span>Are you sure you want to delete the selected paintings?</span>

      <div class="modal-actions">
        <button class="secondary" onclick={() => dialog.close()}>Cancel</button>
        <button
          class="primary"
          onclick={() => {
            deleteSelected();
            dialog.close();
          }}
        >
          Delete selected
        </button>
      </div>
    </div>
  {:else}
    <div class="modal-content">
      <span>Set {editMode} for selected paintings</span>

      {#if editMode == "aspect ratio"}
        <RatioSelect bind:value={bulkInput} />
      {:else}
        <input
          type="text"
          bind:value={bulkInput}
          placeholder="Enter new {editMode}..."
        />
      {/if}

      <div class="modal-actions">
        <button class="secondary" onclick={() => dialog.close()}>Cancel</button>
        <button class="primary" onclick={applyBulkEdit}>Apply</button>
      </div>
    </div>
  {/if}
</dialog>

{#if hasSelected}
  <div id="bulk-tools">
    <button
      onclick={() => openModal("aspect ratio")}
      title="Set aspect ratio for selected paintings"
    >
      <ProportionsIcon />
    </button>
    <button
      onclick={rotateSelectedCCW}
      title="Rotate selected paintings counterclockwise"
    >
      <RotateCcwIcon />
    </button>
    <button
      onclick={rotateSelectedCW}
      title="Rotate selected paintings clockwise"
    >
      <RotateCwIcon />
    </button>
    <button
      onclick={() => openModal("author")}
      title="Set author for selected paintings"
    >
      <UserIcon />
    </button>
    <button
      onclick={() => openModal("title")}
      title="Set title for selected paintings"
    >
      <TextCursorIcon />
    </button>
    <button
      class="delete"
      onclick={() => openModal("delete selected")}
      title="Delete selected paintings"
    >
      <Trash2Icon />
    </button>
  </div>
{/if}

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
    box-shadow: 0px 4px 10px rgba(0, 0, 0, 0.15);
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
