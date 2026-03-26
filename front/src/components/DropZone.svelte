<script lang="ts">
  import { ERROR, toast } from "../stores/toastsStore";
  import { LoaderCircleIcon } from "@lucide/svelte";

  export let handler: (file: File) => Promise<void>;

  let isDragOver = false;
  let totalFiles = 0;
  let loadedFiles = 0;
  $: isProcessing = totalFiles - loadedFiles > 0;

  let fileInput: HTMLInputElement;

  function openFilePicker() {
    if (!isProcessing) fileInput.click();
  }

  async function processFiles(files: FileList | null | undefined) {
    if (!files || files.length === 0 || isProcessing) return;

    totalFiles = files.length;

    for (const file of files) {
      try {
        await handler(file);
        loadedFiles++;
      } catch (e) {
        toast(ERROR, "Could not load file " + file.name);
        totalFiles--;
      }
    }

    if (loadedFiles === totalFiles) {
      loadedFiles = 0;
      totalFiles = 0;
    }

    if (fileInput) fileInput.value = "";
  }

  function handleDrop(e: DragEvent) {
    e.preventDefault();
    isDragOver = false;
    processFiles(e.dataTransfer?.files);
  }

  function handleFileInput(e: Event) {
    const target = e.target as HTMLInputElement;
    processFiles(target.files);
  }

  function handleDragOver(e: DragEvent) {
    e.preventDefault();
    if (!isProcessing) isDragOver = true;
  }

  function handleDragLeave() {
    isDragOver = false;
  }
</script>

<div
  class="drop-zone"
  class:hover={isDragOver}
  class:processing={isProcessing}
  on:drop={handleDrop}
  on:dragover={handleDragOver}
  on:dragleave={handleDragLeave}
  {...$$restProps}
>
  <input
    type="file"
    multiple
    hidden
    bind:this={fileInput}
    on:change={handleFileInput}
  />

  <div class="content-wrapper">
    <div
      class="default-content"
      style:visibility={isProcessing ? "hidden" : "visible"}
    >
      <slot filePicker={openFilePicker} />
    </div>

    {#if isProcessing}
      <div class="processing-content">
        <LoaderCircleIcon size="32" class="spinner" />
        <span>Processing {loadedFiles} of {totalFiles}...</span>
      </div>
    {/if}
  </div>
</div>

<style>
  .drop-zone {
    display: block;
    width: 100%;
    cursor: default;
    border: 2px dashed #aaa;
    border-radius: 12px;
    text-align: center;
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

  .drop-zone.processing {
    cursor: not-allowed;
    border-color: #646cff;
    background-color: rgba(100, 108, 255, 0.05);
  }

  .content-wrapper {
    display: grid;
    place-items: center;
    width: 100%;
  }

  .default-content,
  .processing-content {
    grid-area: 1 / 1;
    width: 100%;
    height: 100%;
  }

  .processing-content {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 15px;
    color: #646cff;
    font-weight: 500;
  }
</style>
