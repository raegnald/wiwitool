<script lang="ts">
  import Wiwicheckbox from "../components/Wiwicheckbox.svelte";
  import {
    paintingsStore,
    type PaintingWrapper,
  } from "../stores/paintingsStore";
  import Button from "../components/Button.svelte";

  type ButtonClickHandler = () => void;

  export let rotateClockwise: ButtonClickHandler;
  export let rotateAnticlockwise: ButtonClickHandler;
  export let clone: ButtonClickHandler;
  export let remove: ButtonClickHandler;
</script>

<div class="actions">
  <div class="first"></div>

  <div>
    <Button
      grow
      onclick={() => alert("Crop!")}
      title="Crop image"
      icon="Crop"
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
      onclick={remove}
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
