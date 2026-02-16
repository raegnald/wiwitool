<script lang="ts">
  import { CheckIcon, XIcon } from "svelte-feather-icons";

  export let checked: boolean;

  function toggle() {
    checked = !checked;
  }
</script>

<div
  class="checkbox-container"
  onclick={toggle}
  onkeydown={toggle}
  tabindex="0"
  aria-label="Checkbox"
  role="checkbox"
  aria-checked={checked}
>
  <button class:checked>
    {#if checked}
      <CheckIcon />
    {:else}
      <XIcon />
    {/if}
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
    color: #ccc;
  }

  button.checked {
    color: white;
    background-color: #646cff;
    border-color: blue;
  }

  button:hover {
    border-color: #646cff;
  }

  @media (prefers-color-scheme: dark) {
    button {
      background-color: #444;
      border-color: #666;
    }
  }
</style>
