<script lang="ts">
  import {
    CopyIcon,
    RotateCcwIcon,
    RotateCwIcon,
    Trash2Icon,
  } from "svelte-feather-icons";

  type ButtonClickHandler = () => void;

  export let rotateClockwise: ButtonClickHandler;
  export let rotateAnticlockwise: ButtonClickHandler;
  export let clone: ButtonClickHandler;
  export let remove: ButtonClickHandler;
</script>

<div class="actions">
  <button onclick={rotateClockwise} title="Rotate clockwise"
    ><RotateCwIcon /></button
  >
  <button onclick={rotateAnticlockwise} title="Rotate anticlockwise"
    ><RotateCcwIcon /></button
  >
  <button onclick={clone} title="Clone image"><CopyIcon /></button>
  <button onclick={remove} class="delete" title="Delete painting"
    ><Trash2Icon /></button
  >
</div>

<style>
  .actions {
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

  @media (prefers-color-scheme: dark) {
    .delete {
      background-color: #e21f1f;
      color: #f4d9d9;
      border: 2px solid #f4d9d9;
    }
  }
</style>
