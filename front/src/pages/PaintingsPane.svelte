<script>
  import PaneHeaderBar from '../lib/PaneHeaderBar.svelte';
  import { wasmModule, packInstance } from '../wasmStore.js';

  // Svelte 5 State
  let files = $state([]);
  let isDragging = $state(false);
  let fileInput;

  /**
   * Process the FileList from drop or input events
   */
  async function handleFiles(incomingFiles) {
    // Check if WASM is ready
    if (!$wasmModule || !$packInstance) {
      alert("System is still loading...");
      return;
    }

    const validImages = Array.from(incomingFiles).filter(file =>
      file.type.startsWith('image/')
    );

    for (const file of validImages) {
      // 1. Read file into JS memory
      const arrayBuffer = await file.arrayBuffer();
      const uint8View = new Uint8Array(arrayBuffer);

      // 2. Write file to Emscripten Virtual File System (MEMFS)
      // We store it in '/uploads/' to match the C++ constructor logic
      const safeName = file.name.replace(/\s+/g, '_'); // sanitize name
      const fsPath = '/uploads/' + safeName;

      try {
        $wasmModule.FS.writeFile(fsPath, uint8View);

        // 3. Register in C++ Class
        // We pass the filename, title, and dummy dimensions (you might want to read real dims)
        $packInstance.add_painting(file.name);

        // 4. Update UI state
        files = [...files, {
          id: crypto.randomUUID(),
          name: file.name,
          size: formatBytes(file.size),
          previewUrl: URL.createObjectURL(file),
        }];

      } catch (err) {
        console.error("Error sending file to WASM:", err);
      }
    }
  }

  /**
   * Helper to format file size
   */
  function formatBytes(bytes, decimals = 1) {
    if (bytes === 0) return '0 B';
    const k = 1024;
    const sizes = ['B', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(decimals)) + ' ' + sizes[i];
  }

  // --- Event Handlers ---

  function onDragOver(e) {
    e.preventDefault();
    isDragging = true;
  }

  function onDragLeave(e) {
    // Only disable dragging if we are actually leaving the container
    // (This prevents flickering when hovering over children)
    if (e.relatedTarget && !e.currentTarget.contains(e.relatedTarget)) {
      isDragging = false;
    }
  }

  function onDrop(e) {
    e.preventDefault();
    isDragging = false;
    if (e.dataTransfer.files && e.dataTransfer.files.length > 0) {
      handleFiles(e.dataTransfer.files);
    }
  }

  function onInputChange(e) {
    if (e.target.files && e.target.files.length > 0) {
      handleFiles(e.target.files);
    }
    // Reset value so the same file can be selected again if needed
    e.target.value = '';
  }

  function triggerUpload() {
    fileInput.click();
  }
</script>

<main>
  <PaneHeaderBar />

  <div class="painting-pane">

    <div
      class="left-side"
      class:dragging={isDragging}
      ondragover={onDragOver}
      ondragleave={onDragLeave}
      ondrop={onDrop}
      role="region"
      aria-label="Image Upload Area"
      >
      <input
        type="file"
        multiple
        accept="image/*"
        bind:this={fileInput}
        onchange={onInputChange}
        style="display: none;"
        />

      <div class="list-header">
        <h3>Files ({files.length})</h3>
        <button class="add-btn" onclick={triggerUpload}>+ Upload</button>
      </div>

      <div class="file-list-container">
        {#if files.length === 0}
          <button class="empty-state" onclick={triggerUpload}>
            <div class="dashed-box">
              <span class="icon">📂</span>
              <p>Drag images here<br/>or click to browse</p>
            </div>
          </button>
        {:else}
          <div class="file-list">
            {#each files as item (item.id)}
              <div class="file-row">
                <img src={item.previewUrl} alt={item.name} class="thumbnail" />
                <div class="file-info">
                  <div class="file-name" title={item.name}>{item.name}</div>
                  <div class="file-size">{item.size}</div>
                </div>
              </div>
            {/each}
          </div>
        {/if}
      </div>

      {#if isDragging}
        <div class="drag-overlay">
          Drop files to add
        </div>
      {/if}
    </div>

    <div class="right-side">
      Metadata and preview side
    </div>
  </div>
</main>

<style>
  main {
    min-height: 100%;
    display: flex;
    flex-direction: column;
  }

  .painting-pane {
    height: 100%;
    display: flex;
    align-items: stretch;
    /* Ensure the pane fills remaining vertical space if needed */
    flex-grow: 1;
  }

  .left-side, .right-side {
    background: rgba(255, 255, 255, 0.9);
    border: 1px solid #888;
    padding: 1em;
  }

  /* --- LEFT SIDE STYLING --- */
  .left-side {
    margin-right: 0.5em;
    min-width: 400px;
    min-height: 600px;

    /* Flex layout for header + list */
    display: flex;
    flex-direction: column;
    position: relative; /* For overlay positioning */
    transition: background-color 0.2s, border-color 0.2s;
  }

  /* Visual cue when dragging over */
  .left-side.dragging {
    background-color: #f0f4ff;
    border-color: #978BE5;
  }

  .list-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 1em;
  }

  .list-header h3 {
    margin: 0;
    font-size: 1.1em;
    color: #444;
  }

  .add-btn {
    padding: 0.4em 0.8em;
    cursor: pointer;
    background: #eee;
    border: 1px solid #ccc;
    border-radius: 4px;
  }
  .add-btn:hover {
    background: #ddd;
  }

  .file-list-container {
    flex-grow: 1;
    overflow-y: auto; /* Scroll if list gets long */
    position: relative;
  }

  /* Empty State */
  .empty-state {
    width: 100%;
    height: 100%;
    background: none;
    border: none;
    padding: 0;
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
    color: #777;
  }

  .dashed-box {
    border: 2px dashed #ccc;
    border-radius: 8px;
    padding: 2em;
    text-align: center;
    width: 80%;
  }

  .dashed-box:hover {
    border-color: #978BE5;
    color: #978BE5;
    background-color: rgba(151, 139, 229, 0.05);
  }

  .icon {
    font-size: 2em;
    display: block;
    margin-bottom: 0.2em;
  }

  /* File List Rows */
  .file-list {
    display: flex;
    flex-direction: column;
    gap: 0.5em;
  }

  .file-row {
    display: flex;
    align-items: center;
    gap: 0.75em;
    padding: 0.5em;
    background: #fff;
    border: 1px solid #eee;
    border-radius: 4px;
  }

  .thumbnail {
    width: 40px;
    height: 40px;
    object-fit: cover;
    border-radius: 4px;
    background-color: #eee;
  }

  .file-info {
    display: flex;
    flex-direction: column;
    overflow: hidden;
  }

  .file-name {
    font-weight: 500;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
    font-size: 0.9em;
  }

  .file-size {
    font-size: 0.75em;
    color: #888;
  }

  /* Drag Overlay */
  .drag-overlay {
    position: absolute;
    top: 0; left: 0; right: 0; bottom: 0;
    background: rgba(151, 139, 229, 0.2);
    border: 2px dashed #978BE5;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 1.5em;
    color: #5548a5;
    pointer-events: none; /* Let events fall through to the container */
    backdrop-filter: blur(2px);
    z-index: 10;
  }

  .right-side {
    flex-grow: 1;
  }
</style>
