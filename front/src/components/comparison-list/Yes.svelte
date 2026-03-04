<script lang="ts">
  import { CheckIcon } from "@lucide/svelte";

  const { children = undefined } = $props();
</script>

<div class="list-element">
  <div class="icon">
    <CheckIcon />
  </div>
  <div class="element-content">
    {@render children?.()}
  </div>
</div>

<style>
  .list-element {
    display: flex;
    gap: 10px;
  }

  .icon {
    color: green;
  }
</style>
