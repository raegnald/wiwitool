<script lang="ts">
  import Button from "../components/Button.svelte";

  type ButtonClickHandler = () => void;

  export let rotateClockwise: ButtonClickHandler;
  export let rotateAnticlockwise: ButtonClickHandler;
  export let clone: ButtonClickHandler;
  export let remove: ButtonClickHandler;
  export let crop: ButtonClickHandler;
  export let isCropped = false;

  let deletePaintingDialog: HTMLDialogElement;
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
  <div class="first"></div>

  <div>
    <Button
      grow
      onclick={crop}
      title="Crop image"
      icon="Crop"
      hugeBorder={isCropped}
    />

    <div class="actions-pack">
      <Button
        onclick={rotateClockwise}
        title="Rotate clockwise"
        icon="RotateCw"
      />

      <Button
        onclick={rotateAnticlockwise}
        title="Rotate anticlockwise"
        icon="RotateCcw"
      />
    </div>

    <Button grow onclick={clone} title="Clone image" icon="Copy" />

    <Button
      grow
      destructive
      onclick={() => deletePaintingDialog.showModal()}
      title="Delete painting"
      icon="Trash2"
    />
  </div>
</div>

<style>
  .actions {
    display: flex;
    flex-direction: column;
    justify-content: space-between;
  }

  .actions .first {
    flex: 1;
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
