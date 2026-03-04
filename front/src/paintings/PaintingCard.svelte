<script lang="ts">
  import { onMount } from "svelte";
  import {
    paintingsStore,
    type PaintingWrapper,
  } from "../stores/paintingsStore";
  import { workspace } from "../stores/workspaceStore";

  import PaintingActions from "./PaintingActions.svelte";
  import PaintingCanvas from "./PaintingCanvas.svelte";
  import PaintingParams from "./PaintingParams.svelte";
  import type { ImageData } from "../bindings/wiwitool";
  import Wiwicheckbox from "../components/Wiwicheckbox.svelte";

  export let wrapper: PaintingWrapper;

  let originalImageCanvas: HTMLCanvasElement;
  let paintingCanvas: HTMLCanvasElement;

  // Local UI state
  let title: string;
  let author: string;
  let currentRatio: string;

  let proceduralFrame = wrapper.cppPainting.isFrameProcedural();
  let frameTint: string;
  let frameSeed: string;

  // External changes that modify local UI state
  $: if (wrapper && wrapper.cppPainting) {
    title = wrapper.cppPainting.title;
    author = wrapper.cppPainting.author;
    currentRatio = wrapper.cppPainting.ratio;

    const settings = wrapper.cppPainting.getProceduralSettings();
    if (settings) {
      frameSeed = String(settings.seed);
      frameTint = settings.tintHex;

      wrapper = wrapper;
      refresh();
    }

    refresh();
  }

  function syncToCpp() {
    if (!wrapper.cppPainting) return;

    wrapper.cppPainting.title = title;
    wrapper.cppPainting.author = author;
    wrapper.cppPainting.ratio = currentRatio;

    const settings = wrapper.cppPainting.getProceduralSettings();
    if (settings) {
      console.warn("Modifying frame cpp state from svelte");
      settings.seed = BigInt(frameSeed);
      settings.tintHex = frameTint;
    }
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
    paintingsStore.set($paintingsStore);
  }

  function rotateAnticlockwise() {
    wrapper.cppPainting.rotateAnticlockwise();
    paintingsStore.set($paintingsStore);
  }

  function toggleFrameType() {
    if (proceduralFrame) {
      wrapper.cppPainting.useProceduralFrame();
    } else {
      wrapper.cppPainting.useDefaultFrame();
    }
    refresh();
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
    wrapper.cppPainting.refresh();

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
        onchange={syncToCpp}
      />
      <PaintingCanvas bind:canvas={paintingCanvas} />
    </div>

    <details>
      <summary>More options for this painting</summary>
      <div class="procedural-frame-card">
        <Wiwicheckbox bind:checked={proceduralFrame} onclick={toggleFrameType}>
          Use procedurally generated frames
        </Wiwicheckbox>

        {#if proceduralFrame}
          <div class="procedural-frame-params">
            <span>
              <label for="frame-colour">Frame colour</label>
              <input
                name="frame-colour"
                type="color"
                bind:value={frameTint}
                oninput={syncToCpp}
                onchange={syncToCpp}
              />
            </span>

            <span>
              <label for="frame-seed">Frame seed</label>
              <input
                name="frame-seed"
                type="number"
                bind:value={frameSeed}
                oninput={syncToCpp}
                onchange={syncToCpp}
              />
            </span>
          </div>
        {/if}
      </div>
    </details>
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

  details {
    margin-top: 20px;
    opacity: 0.7;
  }

  summary {
    margin-bottom: 10px;
  }

  .procedural-frame-card {
    display: flex;
    align-items: stretch;
    justify-content: space-between;
  }

  .procedural-frame-params {
    display: flex;
    gap: 25px;
    font-size: 90%;
  }

  .procedural-frame-params > span {
    display: flex;
    align-items: center;
    gap: 8px;
  }

  input[type="color"] {
    padding: 0;
    border: 0;
    border-radius: 100%;
    width: 40px;
    height: 40px;
  }
</style>
