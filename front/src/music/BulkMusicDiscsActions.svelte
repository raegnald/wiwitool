<script lang="ts">
  import Button from "../components/Button.svelte";
  import { musicDiscsStore } from "../stores/musicDiscsStore";

  $: hasSelected = $musicDiscsStore.some((musicDisc) => musicDisc.selected);

  function deleteSelected() {
    musicDiscsStore.update((musicDiscs) =>
      musicDiscs.filter((musicDisc) => {
        let shouldDelete = musicDisc.selected;
        if (shouldDelete) musicDisc.cppDisc.delete();
        return !shouldDelete;
      }),
    );
  }

  let dialog: HTMLDialogElement;
  let editMode:
    | "artist"
    | "comparator output"
    | "silence gap"
    | "delete selected"
    | "randomise each";
  let bulkInput = "";

  function openModal(
    mode: "artist" | "comparator output" | "silence gap" | "delete selected",
  ) {
    editMode = mode;
    bulkInput = "";
    dialog.showModal();
  }

  function applyBulkEdit() {
    musicDiscsStore.update((discs) => {
      discs.forEach((d) => {
        if (d.selected) {
          if (editMode === "artist") {
            d.cppDisc.artist = bulkInput;
          } else if (editMode == "silence gap") {
            // d.cppDisc. = bulkInput;
            alert("Silence gap not implmented!"); // FOTUT
          } else if (editMode == "comparator output") {
            d.cppDisc.comparatorOutput = parseInt(bulkInput);
          } else if (editMode == "randomise each") {
            d.cppDisc.comparatorOutput = Math.floor(Math.random() * 14 + 1);
          }
        }
      });
      return discs;
    });
    dialog.close();
  }
</script>

<dialog bind:this={dialog}>
  {#if editMode == "delete selected"}
    <div class="modal-content">
      <span>
        Are you sure you want to delete the selected music discs? <br />
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
      <span>Set {editMode} for selected music discs</span>

      {#if editMode == "artist"}
        <input
          type="text"
          bind:value={bulkInput}
          placeholder="Enter new artist..."
        />
      {:else if editMode == "comparator output"}
        <input
          type="number"
          min="1"
          max="15"
          bind:value={bulkInput}
          placeholder="Enter new {editMode}..."
        />
      {:else if editMode == "silence gap"}
        <input
          type="number"
          min="0"
          bind:value={bulkInput}
          placeholder="Enter new {editMode}..."
        />
      {/if}

      <div class="modal-actions">
        {#if editMode == "comparator output"}
          <Button
            icon="Dices"
            onclick={() => {
              editMode = "randomise each";
              applyBulkEdit();
              dialog.close();
            }}
          >
            Randomise each
          </Button>
        {/if}
        <Button transparent onclick={() => dialog.close()}>Cancel</Button>
        <Button onclick={applyBulkEdit}>Apply</Button>
      </div>
    </div>
  {/if}
</dialog>

{#if hasSelected}
  <div id="bulk-tools">
    <Button
      onclick={() => openModal("artist")}
      title="Set artist for selected discs"
      icon="MicVocal"
    />

    <Button
      onclick={() => openModal("comparator output")}
      title="Set comparator output for selected discs"
      icon="Zap"
    />

    <Button
      onclick={() => openModal("silence gap")}
      title="Set silence gap for selected discs"
      icon="Timer"
    />

    <Button
      destructive
      onclick={() => openModal("delete selected")}
      title="Delete selected music discs"
      icon="Trash2"
    />
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
</style>
