<script lang="ts">
  const { title = "", children = undefined } = $props();
</script>

<div class="showcase">
  {#if title}
    <span class="title">{title}</span>
  {/if}
  {@render children?.()}
</div>

<style>
  .showcase {
    margin: 5px 0;
    margin-left: 10px;

    padding: 10px 0;
    padding-left: 20px;
    border-left: 3px solid rgba(127, 127, 127, 0.3);

    display: flex;
    align-items: center;
    flex-wrap: wrap;
    gap: 10px;
  }

  .showcase .title {
    width: 100%;
    font-weight: 600;
    opacity: 0.9;
  }
</style>
