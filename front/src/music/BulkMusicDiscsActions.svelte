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
  let editMode: "delete selected";

  function openModal(mode: "delete selected") {
    editMode = mode;
    dialog.showModal();
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
  {/if}
</dialog>

{#if hasSelected}
  <div id="bulk-tools">
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
