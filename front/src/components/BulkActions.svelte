<script lang="ts">
  import {
    SquareIcon,
    RotateCcwIcon,
    RotateCwIcon,
    UserIcon,
    TypeIcon,
    Trash2Icon,
  } from "svelte-feather-icons";
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
  let editMode: "title" | "author" | "aspect ratio";
  let bulkInput = "";

  function openModal(mode: "title" | "author" | "aspect ratio") {
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
</dialog>

{#if hasSelected}
  <div id="bulk-tools">
    <button
      onclick={() => openModal("aspect ratio")}
      title="Set aspect ratio for selected paintings"
    >
      <SquareIcon />
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
      <TypeIcon />
    </button>
    <button
      class="delete"
      onclick={deleteSelected}
      title="Delete selected paintings"
    >
      <Trash2Icon />
    </button>
  </div>
{/if}

<style>
  dialog {
    border: none;
    border-radius: 12px;
    padding: 20px;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
  }
  dialog::backdrop {
    background: rgba(0, 0, 0, 0.5);
  }
  .modal-content {
    display: flex;
    flex-direction: column;
    gap: 15px;
  }
  .modal-actions {
    display: flex;
    justify-content: flex-end;
    gap: 10px;
  }

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
