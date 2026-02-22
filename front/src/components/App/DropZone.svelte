<script lang="ts">
  import { ERROR, toast } from "../../stores/toastsStore";

  let { handler } = $props();

  let isDragOver = $state(false);

  let totalFiles = $state(0);
  let loadedFiles = $state(0);

  async function handleDrop(e: DragEvent) {
    e.preventDefault();
    isDragOver = false;

    const files = e.dataTransfer.files;
    totalFiles = files.length;

    for (const file of files) {
      try {
        await handler(file);
        loadedFiles++;
      } catch (e) {
        toast(ERROR, "Could not load image: " + e);
        totalFiles--;
      }

      if (loadedFiles == totalFiles) loadedFiles = totalFiles = 0;
    }
  }

  function handleDragOver(e: DragEvent) {
    e.preventDefault();
    isDragOver = true;
  }
  function handleDragLeave() {
    isDragOver = false;
  }
</script>

<button
  class="drop-zone"
  class:hover={isDragOver}
  ondrop={handleDrop}
  ondragover={handleDragOver}
  ondragleave={handleDragLeave}
>
  {#if totalFiles - loadedFiles > 0}
    <span>Loaded {loadedFiles} of {totalFiles}</span>
  {:else}
    <slot />
  {/if}
</button>

<style>
  .drop-zone {
    display: block;
    width: 100%;
    cursor: default;
    border: 2px dashed #aaa;
    padding: 6rem;
    text-align: center;
    margin-bottom: 2rem;
    transition: 0.2s;
  }

  .drop-zone.hover {
    border-color: #007bff;
  }

  @media (prefers-color-scheme: light) {
    .drop-zone.hover {
      background-color: #f0f8ff;
    }
    .drop-zone {
      background-color: #f1f1f1;
    }
  }
</style>
