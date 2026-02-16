<script lang="ts">
  import { CheckIcon } from "svelte-feather-icons";

  export let checked: boolean;
</script>

<div class="checkbox-container" onclick={() => (checked = !checked)}>
  <button class:checked>
    <CheckIcon />
  </button>
  <slot />
</div>

<style>
  .checkbox-container {
    display: flex;
    align-items: center;
    gap: 0.5em;
  }

  button {
    border-radius: 8px;
    border: 1px solid transparent;
    background-color: #f9f9f9;
    transition: border-color 0.25s;
    cursor: pointer;
    border: 1px solid #ccc;
    display: flex;
    justify-content: center;
    padding: 0.5em;
    line-height: 0.9;
    color: #f9f9f9;
  }

  button.checked {
    color: white;
    background-color: #646cff;
    border-color: blue;
  }

  button:hover {
    border-color: #646cff;
  }
</style>
