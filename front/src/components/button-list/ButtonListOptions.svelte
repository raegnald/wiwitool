<script lang="ts">
  import Button from "../Button.svelte";
  import { clickOutside } from "../clickOutside.svelte";

  let {
    children,
    leftAligned = true,
    rightAligned = false,
    ...buttonProps
  } = $props();

  let isOpen = $state(false);

  // If rightAligned is explicitly passed, alignment shifts to the right
  let alignmentClass = $derived(rightAligned ? "align-right" : "align-left");

  function toggle(e: MouseEvent) {
    e.stopPropagation();
    isOpen = !isOpen;
  }
</script>

<div class="options-container" use:clickOutside={() => (isOpen = false)}>
  <Button {...buttonProps} onclick={toggle} />

  {#if isOpen}
    <div
      class="dropdown-list {alignmentClass}"
      onclick={() => (isOpen = false)}
    >
      {@render children?.()}
    </div>
  {/if}
</div>

<style>
  .options-container {
    position: relative;
    display: inline-block;
  }

  .dropdown-list {
    position: absolute;
    top: calc(100% + 5px);
    min-width: 180px;
    z-index: 100;

    background: white;
    border: 1px solid #ccc;
    /* 1. Rounded corners matching standard modern buttons */
    border-radius: 8px;
    /* Prevents child hover backgrounds from breaking the border radius */
    overflow: hidden;

    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
    padding: 4px;
    display: flex;
    flex-direction: column;
  }

  /* 2. Alignment CSS configurations */
  .align-left {
    left: 0;
    right: auto;
  }

  .align-right {
    right: 0;
    left: auto;
  }

  @media (prefers-color-scheme: dark) {
    .dropdown-list {
      background: #1a1a1a;
      border-color: #777;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.5);
    }
  }
</style>
