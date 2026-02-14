<script>
  import { onMount } from "svelte";
  import { paintingsStore } from "../stores";

  import PaintingActions from "./PaintingCard/PaintingActions.svelte";
  import PaintingCanvas from "./PaintingCard/PaintingCanvas.svelte";
  import PaintingParams from "./PaintingCard/PaintingParams.svelte";

  export let wrapper;

  let originalImageCanvas;
  let paintingCanvas;

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

  function clone() {
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
  <div class="transformation">
    <PaintingActions {rotateClockwise} {rotateAnticlockwise} {clone} {remove} />
    <PaintingCanvas bind:canvas={originalImageCanvas} />

    <PaintingParams bind:currentRatio bind:title bind:author />

    <PaintingCanvas bind:canvas={paintingCanvas} />
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
    overflow: scroll;
  }
  .transformation {
    display: flex;
    align-items: center;
    gap: 10px;
  }
</style>
