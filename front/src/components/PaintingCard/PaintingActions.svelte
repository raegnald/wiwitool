<script lang="ts">
  import {
    CopyIcon,
    CropIcon,
    RotateCcwIcon,
    RotateCwIcon,
    Trash2Icon,
  } from "@lucide/svelte";

  import Wiwicheckbox from "../Wiwicheckbox.svelte";
  import {
    paintingsStore,
    type PaintingWrapper,
  } from "../../stores/paintingsStore";

  type ButtonClickHandler = () => void;

  export let id: number;
  export let selected: boolean;
  export let rotateClockwise: ButtonClickHandler;
  export let rotateAnticlockwise: ButtonClickHandler;
  export let clone: ButtonClickHandler;
  export let remove: ButtonClickHandler;

  function toggleSelection() {
    paintingsStore.update((current) =>
      current.map((painting: PaintingWrapper) =>
        painting.id === id
          ? { ...painting, selected: selected } // Create a new object to trigger reactivity
          : painting,
      ),
    );
  }
</script>

<div class="actions">
  <div class="first">
    <Wiwicheckbox bind:checked={selected} onclick={toggleSelection} />
  </div>

  <div>
    <!-- <button onclick={() => alert("Crop!")} title="Crop image"
      ><CropIcon /></button
    > -->

    <div class="actions-pack">
      <button onclick={rotateClockwise} title="Rotate clockwise"
        ><RotateCwIcon /></button
      >
      <button onclick={rotateAnticlockwise} title="Rotate anticlockwise"
        ><RotateCcwIcon /></button
      >
    </div>

    <button onclick={clone} title="Clone image"><CopyIcon /></button>
    <button onclick={remove} class="delete" title="Delete painting"
      ><Trash2Icon /></button
    >
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
  .actions button {
    padding: 0.5em;
    line-height: 0.9;
  }
  .delete {
    background-color: #ffcccc;
    border: 2px solid #ffaaaa;
  }
  .delete:hover {
    border-color: #ff5555;
  }
  button {
    cursor: pointer;
    padding: 4px 8px;
  }

  .actions-pack {
    display: flex;
    flex-direction: row;
    gap: 5px;
  }

  @media (prefers-color-scheme: dark) {
    .delete {
      background-color: #e21f1f;
      color: #f4d9d9;
      border: 2px solid #f4d9d9;
    }
  }
</style>
