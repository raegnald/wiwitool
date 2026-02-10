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

  // Reactive update
  $: if (wrapper.cppPainting) {
    wrapper.cppPainting.title = title;
    wrapper.cppPainting.author = author;
    currentRatio = wrapper.cppPainting.ratio;
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

    c.style.width = `${cssW}px`;
    c.style.height = `${cssH}px`;
  }

  function renderCppImage(canvas, img) {
    if (!wrapper.cppPainting || !canvas) return;

    const width = img.width;
    const height = img.height;

    const pixelView = img.getPixels();
    const clampedArray = new Uint8ClampedArray(pixelView); // copies data
    const imageData = new ImageData(clampedArray, width, height);

    // Draw to Canvas
    scalePixelCanvas(canvas, width, height);
    const ctx = canvas.getContext("2d");
    ctx.putImageData(imageData, 0, 0);

    img.delete();
  }

  function renderOriginalImage() {
    renderCppImage(originalImageCanvas, wrapper.cppPainting.originalData());
  }

  function renderPainting() {
    renderCppImage(paintingCanvas, wrapper.cppPainting.paintingData());
  }

  function rotateClockwise() {
    wrapper.cppPainting.rotateClockwise();
    paintingsStore.update((items) => items);
  }

  function rotateAnticlockwise() {
    wrapper.cppPainting.rotateAnticlockwise();
    paintingsStore.update((items) => items); // trigger Svelte update
  }

  function remove() {
    wrapper.cppPainting.delete();
    paintingsStore.update((list) => list.filter((p) => p.id !== wrapper.id));
  }

  function clonePainting() {
    const newCppPainting = wrapper.cppPainting.clone();

    const newWrapper = {
      id: Math.random(), // Unique ID for Svelte's #each key
      title: wrapper.title + " (copy)",
      author: wrapper.author,
      cppPainting: newCppPainting,
    };

    // 3. Add to store immediately after the current item
    paintingsStore.update((list) => {
      const index = list.findIndex((p) => p.id === wrapper.id);
      const newList = [...list];
      newList.splice(index + 1, 0, newWrapper);
      return newList;
    });
  }

  function refresh() {
    renderPainting();
    renderOriginalImage();
    wrapper.cppPainting.ratio = currentRatio;
  }

  onMount(refresh);
</script>

<div class="app-card card">
  <div class="actions">
    <button onclick={rotateClockwise} title="Rotate clockwise"
      ><RotateCwIcon /></button
    >
    <button onclick={rotateAnticlockwise} title="Rotate anticlockwise"
      ><RotateCcwIcon /></button
    >
    <button onclick={clonePainting} title="Clone image"><CopyIcon /></button>
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
