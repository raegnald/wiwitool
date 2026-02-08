<script>
  import { onMount } from "svelte";
  import { paintingsStore } from "../stores";

  // 1. CHANGE THIS NAME to match what you pass in App.svelte
  export let wrapper;

  let originalImageCanvas;
  let canvas;

  // 2. Update references here
  let title = wrapper.title;
  let author = wrapper.author;

  // Reactive: Update C++ object when inputs change
  // 3. Update references inside the reactive block
  $: if (wrapper.cppPainting) {
    wrapper.cppPainting.title = title;
    wrapper.cppPainting.author = author;
  }

  function renderOriginalImage() {
    if (!wrapper.cppPainting || !originalImageCanvas) return;

    // 1. Get the Image_data from C++
    // This returns the RAW pixels (RGBA), already decoded and potentially rotated
    const img = wrapper.cppPainting.originalData();

    const width = img.width;
    const height = img.height;

    // 2. Get the view into WASM memory
    const pixelView = img.getPixels();

    // 3. Create HTML ImageData
    // We create a Uint8ClampedArray from the view.
    // This copies the data, which is safe and prevents issues if WASM memory grows.
    const clampedArray = new Uint8ClampedArray(pixelView);
    const imageData = new ImageData(clampedArray, width, height);

    // 4. Draw to Canvas
    originalImageCanvas.width = width;
    originalImageCanvas.height = height;
    const ctx = originalImageCanvas.getContext("2d");
    ctx.putImageData(imageData, 0, 0);

    // 5. IMPORTANT: Delete the C++ object to free memory
    img.delete();
  }

  function renderPainting() {
    // 4. Update references
    if (!wrapper.cppPainting || !canvas) return;

    // const iconData = wrapper.cppPainting.iconData();
    const iconData = wrapper.cppPainting.paintingData();
    const width = iconData.width;
    const height = iconData.height;

    const pixelView = iconData.getPixels();
    const clampedArray = new Uint8ClampedArray(pixelView);
    const imageData = new ImageData(clampedArray, width, height);

    canvas.width = width;
    canvas.height = height;
    const ctx = canvas.getContext("2d");
    ctx.putImageData(imageData, 0, 0);

    iconData.delete();
  }

  function rotate() {
    wrapper.cppPainting.rotateClockwise();
    renderPainting();
    renderOriginalImage();

    // Trigger Svelte update
    paintingsStore.update((items) => items);
  }

  function remove() {
    // 6. Update references
    wrapper.cppPainting.delete();
    paintingsStore.update((list) => list.filter((p) => p.id !== wrapper.id));
  }

  onMount(() => {
    renderPainting();
    renderOriginalImage();
  });
</script>

<div class="app-card card">
  <div class="actions">
    <button on:click={rotate}>Rotate ↻</button>
    <button on:click={remove} class="delete">Remove</button>
  </div>

  <div class="transformation">
    <div class="canvas-transformation-container">
      <canvas bind:this={originalImageCanvas} class="original-image"></canvas>
    </div>

    <div class="transformation-ratio-selection">
      <span>
        Ratio&nbsp;
        <select id="painting-ratio">
          <option value="dog">1:1</option>
          <option value="dog" selected>2:1</option>
        </select>
      </span>
      <svg
        class="rightarrow"
        width="129"
        height="32"
        viewBox="0 0 129 32"
        fill="none"
        xmlns="http://www.w3.org/2000/svg"
      >
        <path
          d="M2 16H127M127 16L113 30M127 16L113 2"
          stroke="#A4A4A4"
          stroke-width="4"
          stroke-linecap="round"
          stroke-linejoin="round"
        />
      </svg>
    </div>

    <div class="canvas-transformation-container">
      <canvas bind:this={canvas} class="pixel-art"></canvas>
    </div>
  </div>

  <div class="inputs">
    <label>
      <span>Title </span><input type="text" bind:value={title} />
    </label>
    <label>
      <span>Author </span><input type="text" bind:value={author} />
    </label>
  </div>
</div>

<style>
  .card {
    margin: 20px 0px;
    display: flex;
    align-items: center;
    gap: 1rem;
  }
  .transformation {
    display: flex;
    align-items: center;
    gap: 10px;
  }
  .transformation-ratio-selection {
    display: flex;
    flex-direction: column;
    align-items: center;
  }
  .pixel-art {
    image-rendering: pixelated;
    width: 100%;
  }
  .inputs {
    display: flex;
    flex-direction: column;
    gap: 5px;
  }
  .inputs label span {
    display: inline-block;
    min-width: 60px;
    user-select: none;
  }
  .actions {
    display: flex;
    flex-direction: column;
    gap: 5px;
  }
  .delete {
    background-color: #ffcccc;
    border: 1px solid #ffaaaa;
  }
  button {
    cursor: pointer;
    padding: 4px 8px;
  }
  .canvas-transformation-container {
    width: 200px;
    height: 200px;
    display: flex;
    justify-content: center;
    align-items: center;
  }
</style>
