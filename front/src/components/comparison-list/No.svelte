<script lang="ts">
  import { XIcon } from "@lucide/svelte";

  const { children = undefined } = $props();
</script>

<div class="list-element">
  <div class="icon">
    <XIcon />
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
    color: indianred;
  }
</style>
