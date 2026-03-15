<script lang="ts">
  import { ERROR, toast } from "../stores/toastsStore";
  import { LoaderCircleIcon } from "@lucide/svelte";

  let { handler, children, ...other } = $props();

  let isDragOver = $state(false);

  let totalFiles = $state(0);
  let loadedFiles = $state(0);
  let isProcessing = $derived(totalFiles - loadedFiles > 0);

  async function handleDrop(e: DragEvent) {
    e.preventDefault();
    isDragOver = false;

    if (isProcessing) return;

    const files = e.dataTransfer?.files;
    totalFiles = files.length;

    for (const file of files) {
      try {
        await handler(file);
        loadedFiles++;
      } catch (e) {
        toast(ERROR, "Could not load file: " + e);
        totalFiles--;
      }

      if (loadedFiles == totalFiles) loadedFiles = totalFiles = 0;
    }
  }

  function handleDragOver(e: DragEvent) {
    e.preventDefault();
    if (!isProcessing) isDragOver = true;
    isDragOver = true;
  }

  function handleDragLeave() {
    isDragOver = false;
  }
</script>

<button
  class="drop-zone"
  class:hover={isDragOver}
  class:processing={isProcessing}
  ondrop={handleDrop}
  ondragover={handleDragOver}
  ondragleave={handleDragLeave}
  disabled={isProcessing}
  {...other}
>
  {#if isProcessing}
    <div class="processing-content">
      <LoaderCircleIcon size="32" class="spinner" />
      <span>Processing {loadedFiles} of {totalFiles}...</span>
    </div>
  {:else}
    {@render children()}
  {/if}
</button>

<style>
  .drop-zone {
    display: block;
    width: 100%;
    cursor: default;
    border: 2px dashed #aaa;
    padding: 0 6rem;
    height: 15rem;
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

  .drop-zone:disabled {
    cursor: not-allowed;
    border-color: #646cff;
    background-color: rgba(100, 108, 255, 0.05);
  }

  .processing-content {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 15px;
    color: #646cff;
    font-weight: 500;
  }
</style>
