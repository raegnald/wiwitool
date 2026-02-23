<script lang="ts">
  import { onMount } from "svelte";
  import {
    paintingsStore,
    type PaintingWrapper,
  } from "../stores/paintingsStore";
  import { workspace } from "../stores/workspaceStore";

  import PaintingActions from "./PaintingCard/PaintingActions.svelte";
  import PaintingCanvas from "./PaintingCard/PaintingCanvas.svelte";
  import PaintingParams from "./PaintingCard/PaintingParams.svelte";
  import type { ImageData } from "../bindings/wiwitool";

  export let wrapper: PaintingWrapper;

  let originalImageCanvas: HTMLCanvasElement;
  let paintingCanvas: HTMLCanvasElement;

  // Local UI state
  let title: string;
  let author: string;
  let currentRatio: string;

  // External changes that modify local UI state
  $: if (wrapper && wrapper.cppPainting) {
    title = wrapper.cppPainting.title;
    author = wrapper.cppPainting.author;
    currentRatio = wrapper.cppPainting.ratio;

    refresh();
  }

  function syncToCpp() {
    if (!wrapper.cppPainting) return;

    wrapper.cppPainting.title = title;
    wrapper.cppPainting.author = author;
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
    const idToRemove = wrapper.cppPainting.stringId();

    $workspace.removePainting(idToRemove);

    paintingsStore.update((list) => list.filter((p) => p !== wrapper));

    wrapper.cppPainting.delete();
  }

  function clone() {
    const newCppPainting = wrapper.cppPainting.clone();
    newCppPainting.title = title + " (copy)";

    $workspace.addPainting(newCppPainting);

    paintingsStore.update((list) => {
      const index = list.findIndex(
        (p) => p.cppPainting.stringId() === wrapper.cppPainting.stringId(),
      );

      const newList = [...list];
      newList.splice(index + 1, 0, {
        cppPainting: newCppPainting,
        selected: false,
      });

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
        id={wrapper.cppPainting.stringId()}
        {rotateClockwise}
        {rotateAnticlockwise}
        {clone}
        {remove}
      />
      <PaintingCanvas bind:canvas={originalImageCanvas} />
      <PaintingParams
        bind:currentRatio
        bind:title
        bind:author
        oninput={syncToCpp}
        onchange={syncToCpp}
      />
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
    padding: 0;
  }

  .card-padding {
    width: 100%;
    border: 5px solid transparent;
    padding: 15px;
    transition: 100ms;
    border-radius: 11px;
    overflow: scroll;
  }

  .card-padding.selected {
    border-color: #646cff;
  }

  .transformation {
    display: flex;
    align-items: stretch;
    gap: 10px;
  }
</style>
