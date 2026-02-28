<script lang="ts">
  import { BadgeInfo } from "@lucide/svelte";

  const { children = undefined } = $props();
</script>

<div class="list-element">
  <div class="icon">
    <BadgeInfo />
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
    color: steelblue;
  }
</style>
