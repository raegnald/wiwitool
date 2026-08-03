<script lang="ts">
  import ButtonListOptions from "../components/button-list/ButtonListOptions.svelte";
  import ListOption from "../components/button-list/ListOption.svelte";
  import ListSectionTitle from "../components/button-list/ListSectionTitle.svelte";
  import Button from "../components/Button.svelte";
  import Wiwicheckbox from "../components/Wiwicheckbox.svelte";
  import { paintingsStore } from "../stores/paintingsStore";

  type ButtonClickHandler = () => void;

  export let selected: boolean;
  export let clone: ButtonClickHandler;
  export let remove: ButtonClickHandler;
  export let updateSelected: () => void;

  let deletePaintingDialog: HTMLDialogElement;

  $: anySelected = $paintingsStore.some((painting) => painting.selected);
</script>

<dialog bind:this={deletePaintingDialog}>
  <div class="modal-content">
    <span>
      Are you sure you want to remove this painting? <br />
      All the customizations you've made to it will be lost.
    </span>

    <div class="modal-actions">
      <Button transparent onclick={() => deletePaintingDialog.close()}>
        Cancel
      </Button>

      <Button
        destructive
        onclick={() => {
          deletePaintingDialog.close();
          remove();
        }}
      >
        Remove painting
      </Button>
    </div>
  </div>
</dialog>

<div class="actions">
  <div>
    <ButtonListOptions rightAligned icon="EllipsisVertical" transparent>
      <ListOption
        icon={selected ? "X" : "Check"}
        hovered={selected}
        onclick={() => {
          selected = !selected;
          updateSelected();
        }}
      >
        {selected ? "Deselect" : "Select"}
      </ListOption>

      <ListSectionTitle>Painting options</ListSectionTitle>

      <ListOption icon="Copy" onclick={clone}>Duplicate</ListOption>

      <ListOption
        icon="Trash2"
        onclick={() => deletePaintingDialog.showModal()}
        destructive
      >
        Delete
      </ListOption>
    </ButtonListOptions>

    {#if anySelected}
      <Wiwicheckbox
        checked={selected}
        onclick={() => {
          selected = !selected;
          updateSelected();
        }}
      />
    {/if}
  </div>
</div>

<style>
  .actions {
    display: flex;
    flex-direction: column;
    justify-content: space-between;
  }

  .actions > div {
    display: flex;
    flex-direction: column;
    gap: 5px;
  }

  .actions-pack {
    display: flex;
    flex-direction: row;
    gap: 5px;
  }
</style>
