<script>
  import { onMount } from "svelte";
  import { paintingsStore } from "../stores";
  import {
    CopyIcon,
    RotateCcwIcon,
    RotateCwIcon,
    Trash2Icon,
  } from "svelte-feather-icons";

  export let wrapper;

  let originalImageCanvas;
  let paintingCanvas;

  // 2. Update references here
  let title = wrapper.title;
  let author = wrapper.author;

  let currentRatio;

  // Reactive: Update C++ object when inputs change
  // 3. Update references inside the reactive block
  $: if (wrapper.cppPainting) {
    wrapper.cppPainting.title = title;
    wrapper.cppPainting.author = author;
  }

  $: if (wrapper.cppPainting && currentRatio) {
    wrapper.cppPainting.ratio = currentRatio;
    refresh();
  }

  function scalePixelCanvas(c, imgW, imgH) {
    const containerSize = 256;

    c.width = imgW;
    c.height = imgH;

    const scale = Math.min(containerSize / imgW, containerSize / imgH);
    const cssW = Math.floor(imgW * scale);
    const cssH = Math.floor(imgH * scale);

    // Change the CSS
    c.style.width = `${cssW}px`;
    c.style.height = `${cssH}px`;
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
    scalePixelCanvas(originalImageCanvas, width, height);
    const ctx = originalImageCanvas.getContext("2d");
    ctx.putImageData(imageData, 0, 0);

    // 5. IMPORTANT: Delete the C++ object to free memory
    img.delete();
  }

  function renderPainting() {
    // 4. Update references
    if (!wrapper.cppPainting || !paintingCanvas) return;

    // const iconData = wrapper.cppPainting.iconData();
    const iconData = wrapper.cppPainting.paintingData();
    const width = iconData.width;
    const height = iconData.height;

    const pixelView = iconData.getPixels();
    const clampedArray = new Uint8ClampedArray(pixelView);
    const imageData = new ImageData(clampedArray, width, height);

    console.log("Painting canvas");
    scalePixelCanvas(paintingCanvas, width, height);
    const ctx = paintingCanvas.getContext("2d");
    ctx.putImageData(imageData, 0, 0);

    iconData.delete();
  }

  function rotateClockwise() {
    wrapper.cppPainting.rotateClockwise();
    refresh();

    // Trigger Svelte update
    paintingsStore.update((items) => items);
  }

  function rotateAnticlockwise() {
    wrapper.cppPainting.rotateAnticlockwise();
    refresh();

    // Trigger Svelte update
    paintingsStore.update((items) => items);
  }

  function remove() {
    // 6. Update references
    wrapper.cppPainting.delete();
    paintingsStore.update((list) => list.filter((p) => p.id !== wrapper.id));
  }

  onMount(() => {
    refresh();
  });

  function refresh() {
    renderPainting();
    renderOriginalImage();
    currentRatio = wrapper.cppPainting.ratio;
  }
</script>

<div class="app-card card">
  <div class="actions">
    <button onclick={rotateClockwise} title="Rotate clockwise"
      ><RotateCwIcon /></button
    >
    <button onclick={rotateAnticlockwise} title="Rotate anticlockwise"
      ><RotateCcwIcon /></button
    >
    <button onclick={cloneImage} title="Clone image"><CopyIcon /></button>
    <button onclick={remove} class="delete" title="Delete painting"
      ><Trash2Icon /></button
    >
  </div>

  <div class="transformation">
    <div class="canvas-transformation-container">
      <canvas bind:this={originalImageCanvas} class="original-image"></canvas>
    </div>

    <div class="transformation-ratio-selection">
      <div>
        <span>Aspect ratio </span>
        <select id="painting-ratio" bind:value={currentRatio}>
          <optgroup label="Square">
            <option value="ONE_ONE">1:1</option>
            <option value="TWO_TWO">2:2</option>
            <option value="THREE_THREE">3:3</option>
            <option value="FOUR_FOUR">4:4</option>
          </optgroup>

          <optgroup label="Wide">
            <option value="TWO_ONE" selected>2:1</option>
            <option value="THREE_TWO">3:2</option>
            <option value="FOUR_TWO">4:2</option>
            <option value="FOUR_THREE">4:3</option>
          </optgroup>

          <optgroup label="Tall">
            <option value="ONE_TWO">1:2</option>
            <option value="TWO_THREE">2:3</option>
            <option value="TWO_FOUR">2:4</option>
            <option value="THREE_FOUR">3:4</option>
          </optgroup>
        </select>
      </div>

      <img class="rightarrow" src="rightarrow.svg" alt="Right arrow" />

      <div class="inputs">
        <label>
          <span>Title </span><input type="text" bind:value={title} />
        </label>
        <label>
          <span>Author </span><input type="text" bind:value={author} />
        </label>
      </div>
    </div>

    <div class="canvas-transformation-container">
      <canvas bind:this={paintingCanvas} class="pixel-art"></canvas>
    </div>
  </div>
</div>

<style>
  .card {
    margin: 20px 0px;
    display: flex;
    align-items: center;
    gap: 1rem;
    flex-wrap: nowrap;
    justify-content: space-between;
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
    width: 100%;
    padding: 10px;
  }
  .pixel-art {
    image-rendering: pixelated;
  }
  .inputs {
    display: flex;
    flex-direction: column;
    gap: 5px;
    margin-top: 30px;
  }
  .inputs label span {
    display: inline-block;
    min-width: 60px;
    user-select: none;
  }
  .rightarrow {
    width: 100%;
  }
  .actions {
    display: flex;
    flex-direction: column;
    gap: 5px;
  }
  .actions button {
    padding: 0.5em;
    line-height: 0.9;
  }
  .delete {
    background-color: #ffcccc;
    border: 2px solid #ffaaaa;
  }
  .delete:hover {
    border-color: #ff5555;
  }
  button {
    cursor: pointer;
    padding: 4px 8px;
  }
  .canvas-transformation-container {
    width: 256px;
    height: 256px;
    min-width: 256px;
    min-height: 256px;
    flex-shrink: 0;
    padding: 10px;
    display: flex;
    justify-content: center;
    align-items: center;
    background: #eee;
  }
</style>
