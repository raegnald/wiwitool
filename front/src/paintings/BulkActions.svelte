<script lang="ts">
  import { paintingsStore } from "../stores/paintingsStore";
  import Button from "../components/Button.svelte";
  import Wiwinput from "../components/Wiwinput.svelte";

  $: hasSelected = $paintingsStore.some((painting) => painting.selected);

  function rotateSelectedCCW() {
    $paintingsStore.forEach((painting) => {
      if (painting.selected) {
        painting.painting.rotateAnticlockwise();
      }
    });
    paintingsStore.update((paintings) => paintings);
  }

  function rotateSelectedCW() {
    $paintingsStore.forEach((painting) => {
      if (painting.selected) {
        painting.painting.rotateClockwise();
      }
    });
    paintingsStore.update((paintings) => paintings);
  }

  function deleteSelected() {
    paintingsStore.update((paintings) =>
      paintings.filter((painting) => {
        if (painting.selected) painting.painting.dispose();
        return !painting.selected;
      }),
    );
  }

  let dialog: HTMLDialogElement;
  let editMode: "title" | "author" | "delete selected";
  let bulkInput = "";

  function openModal(mode: "title" | "author" | "delete selected") {
    editMode = mode;
    bulkInput = ""; // clear previous input
    dialog.showModal();
  }

  function applyBulkEdit() {
    paintingsStore.update((paintings) => {
      paintings.forEach((p) => {
        if (p.selected) {
          if (editMode === "title") {
            p.painting.title = bulkInput;
          } else if (editMode == "author") {
            p.painting.author = bulkInput;
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
      <span>
        Are you sure you want to delete the selected paintings? <br />
        This action cannot be undone.
      </span>

      <div class="modal-actions">
        <Button transparent onclick={() => dialog.close()}>Cancel</Button>
        <Button
          destructive
          onclick={() => {
            deleteSelected();
            dialog.close();
          }}
        >
          Delete selected
        </Button>
      </div>
    </div>
  {:else}
    <div class="modal-content">
      <span>Set {editMode} for selected paintings</span>

      <Wiwinput
        type="text"
        bind:value={bulkInput}
        placeholder="Enter new {editMode}..."
      />

      <div class="modal-actions">
        <Button transparent onclick={() => dialog.close()}>Cancel</Button>
        <Button onclick={applyBulkEdit}>Apply</Button>
      </div>
    </div>
  {/if}
</dialog>

{#if hasSelected}
  <div id="bulk-tools">
    <Button
      transparent
      onclick={rotateSelectedCCW}
      title="Rotate selected paintings counterclockwise"
      icon="RotateCcw"
    />

    <Button
      transparent
      onclick={rotateSelectedCW}
      title="Rotate selected paintings clockwise"
      icon="RotateCw"
    />

    <Button
      transparent
      onclick={() => openModal("author")}
      title="Set author for selected paintings"
      icon="User"
    />

    <Button
      transparent
      onclick={() => openModal("title")}
      title="Set title for selected paintings"
      icon="TextCursor"
    />

    <Button
      transparent
      destructive
      onclick={() => openModal("delete selected")}
      title="Delete selected paintings"
      icon="Trash2"
    />
  </div>
{/if}

<style>
  #bulk-tools {
    padding: 12px;
    display: flex;
    gap: 6px;
    justify-content: center;
    align-items: center;
  }
</style>
