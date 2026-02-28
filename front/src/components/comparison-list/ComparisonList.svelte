<script module lang="ts">
  export { default as Yes } from "./Yes.svelte";
  export { default as No } from "./No.svelte";
  export { default as Info } from "./Info.svelte";

  export { default as Showcase } from "./Showcase.svelte";

  export { default as Space } from "./Space.svelte";
</script>

<script lang="ts">
  let { children = undefined } = $props();
</script>

<div class="comparison-list">
  {@render children?.()}
</div>

<style>
  .comparison-list {
    padding-left: 1em;
    display: flex;
    flex-direction: column;
    gap: 5px;
    margin: 20px 0;
  }
</style>
