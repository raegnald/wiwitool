<script lang="ts">
  import { CheckIcon, XIcon } from "@lucide/svelte";

  export let checked: boolean;
  export let onclick: () => void;

  function toggle() {
    checked = !checked;
  }
</script>

<div
  class="checkbox-container"
  onclick={() => {
    toggle();
    onclick();
  }}
  onkeydown={() => {}}
  tabindex="0"
  aria-label="Checkbox"
  role="checkbox"
  aria-checked={checked}
>
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
    cursor: pointer;
    user-select: none;
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
    color: #ddd;
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
      color: #555;
    }
  }
</style>
