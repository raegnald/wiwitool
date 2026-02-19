<script lang="ts">
  import { onMount } from "svelte";
  import {
    paintingsStore,
    type PaintingWrapper,
  } from "../stores/paintingsStore";

  import PaintingActions from "./PaintingCard/PaintingActions.svelte";
  import PaintingCanvas from "./PaintingCard/PaintingCanvas.svelte";
  import PaintingParams from "./PaintingCard/PaintingParams.svelte";
  import type { ImageData } from "../bindings/wiwitool";

  export let wrapper: PaintingWrapper;

  let originalImageCanvas: HTMLCanvasElement;
  let paintingCanvas: HTMLCanvasElement;

  let title = wrapper.title;
  let author = wrapper.author;

  let currentRatio: string;

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

  function scalePixelCanvas(c: HTMLCanvasElement, imgW: number, imgH: number) {
    const containerSize = 256;

    c.width = imgW;
    c.height = imgH;

    const scale = Math.min(containerSize / imgW, containerSize / imgH);
    const cssW = Math.floor(imgW * scale);
    const cssH = Math.floor(imgH * scale);

    c.style.width = `${cssW}px`;
    c.style.height = `${cssH}px`;
  }

  function renderCppImage(canvas: HTMLCanvasElement, img: ImageData) {
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

    const newWrapper: PaintingWrapper = {
      id: Math.random(),
      title: wrapper.title + " (copy)",
      author: wrapper.author,
      cppPainting: newCppPainting,
      originalImageBytes: wrapper.originalImageBytes,
      selected: false,
      crop: null,
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
  <div class="card-padding" class:selected={wrapper.selected}>
    <div class="transformation">
      <PaintingActions
        bind:selected={wrapper.selected}
        id={wrapper.id}
        {rotateClockwise}
        {rotateAnticlockwise}
        {clone}
        {remove}
      />
      <PaintingCanvas bind:canvas={originalImageCanvas} />
      <PaintingParams bind:currentRatio bind:title bind:author />
      <PaintingCanvas bind:canvas={paintingCanvas} />
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
    overflow: scroll;
    padding: 0;
  }

  .card-padding {
    width: 100%;
    border: 5px solid transparent;
    padding: 15px;
    transition: 100ms;
    border-radius: 11px;
  }

  .card-padding.selected {
    border-color: #646cff;
  }

  .transformation {
    display: flex;
    align-items: center;
    gap: 10px;
    align-items: stretch;
    align-items: center;
  }
</style>
