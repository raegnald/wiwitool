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
  import CropModal from "./CropModal.svelte";
  import type { ImageData } from "../bindings/wiwitool";
  import Wiwicheckbox from "../components/Wiwicheckbox.svelte";
  import SelectableCard from "../components/SelectableCard.svelte";
  import Button from "../components/Button.svelte";
  import { RulerIcon, SquareIcon, WrenchIcon } from "@lucide/svelte";

  export let wrapper: PaintingWrapper;

  let showMoreOptions = false;
  let showCropModal = false;
  let colourPicker: HTMLInputElement;

  let originalImageCanvas: HTMLCanvasElement;
  let paintingCanvas: HTMLCanvasElement;

  // Local UI state
  let title: string;
  let author: string;
  let currentRatio: string;

  let selected: boolean;

  let isCropped = false;
  let disableFrame = false;
  let proceduralFrame = wrapper.cppPainting.isFrameProcedural();
  let frameTint: string;
  let frameSeed: string;
  let placeable: boolean;
  let hasStonecutterRecipe: boolean;

  let downscalePower: number;

  // React to wrapper changes
  $: pullFromCpp(wrapper);

  function pullFromCpp(w: PaintingWrapper) {
    if (!w || !w.cppPainting) return;

    title = w.cppPainting.title;
    author = w.cppPainting.author;
    selected = w.selected;
    placeable = w.cppPainting.placeable;
    hasStonecutterRecipe = w.cppPainting.hasStonecutterRecipe;

    isCropped =
      w.cppPainting.cropX !== 0 ||
      w.cppPainting.cropY !== 0 ||
      w.cppPainting.cropW !== 1 ||
      w.cppPainting.cropH !== 1;

    let canvasNeedsUpdate = false;

    if (currentRatio !== w.cppPainting.ratio) {
      currentRatio = w.cppPainting.ratio;
      canvasNeedsUpdate = true;
    }

    let currentDownscale = Math.log2(w.cppPainting.pixelPerBlock);
    if (downscalePower !== currentDownscale) {
      downscalePower = currentDownscale;
      canvasNeedsUpdate = true;
    }

    const settings = w.cppPainting.getProceduralSettings();
    if (settings) {
      if (frameSeed !== String(settings.seed)) {
        frameSeed = String(settings.seed);
        canvasNeedsUpdate = true;
      }
      if (frameTint !== settings.tintHex) {
        frameTint = settings.tintHex;
        canvasNeedsUpdate = true;
      }
    }

    if (!w.cachedOriginalJS || !w.cachedPaintingJS) canvasNeedsUpdate = true;

    if (canvasNeedsUpdate) {
      w.cachedPaintingJS = undefined;
      updateCanvases();
    }
  }

  // Fast sync for typing metadata
  function syncMetadata() {
    if (!wrapper.cppPainting) return;
    wrapper.cppPainting.title = title;
    wrapper.cppPainting.author = author;
    wrapper.selected = selected;
    wrapper.cppPainting.placeable = placeable;
    wrapper.cppPainting.hasStonecutterRecipe = hasStonecutterRecipe;
  }

  // Heavy sync for visual parameters (slider, ratio, colors)
  function syncVisualsAndRefresh() {
    if (!wrapper.cppPainting) return;

    wrapper.cppPainting.pixelPerBlock = Math.pow(2, downscalePower);

    const settings = wrapper.cppPainting.getProceduralSettings();
    if (settings) {
      settings.seed = BigInt(frameSeed);
      settings.tintHex = frameTint;
    }

    // Since setting the ratio in C++ automatically triggers a C++ refresh(),
    // we do this last.
    wrapper.cppPainting.ratio = currentRatio;

    // Safety net in case ratio didn't change but the seed/scale did
    wrapper.cppPainting.refresh();

    wrapper.cachedPaintingJS = undefined;
    updateCanvases();
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

  function extractJSImageData(img: ImageData) {
    const width = img.width;
    const height = img.height;

    const pixelView = img.getPixels();
    const clampedArray = new Uint8ClampedArray(pixelView);

    return new globalThis.ImageData(clampedArray, width, height);
  }

  function drawJSImageData(
    canvas: HTMLCanvasElement,
    jsImageData: globalThis.ImageData,
  ) {
    if (!canvas) return;

    scalePixelCanvas(canvas, jsImageData.width, jsImageData.height);
    const ctx = canvas.getContext("2d");
    if (ctx) ctx.putImageData(jsImageData, 0, 0);
  }

  function renderOriginalImage() {
    if (!wrapper.cachedOriginalJS) {
      const img = wrapper.cppPainting.croppedOriginalData();
      wrapper.cachedOriginalJS = extractJSImageData(img);
      img.delete();
    }
    drawJSImageData(originalImageCanvas, wrapper.cachedOriginalJS);
  }

  function renderPainting() {
    if (!wrapper.cachedPaintingJS) {
      const img = wrapper.cppPainting.paintingData();
      wrapper.cachedPaintingJS = extractJSImageData(img);
      img.delete();
    }
    drawJSImageData(paintingCanvas, wrapper.cachedPaintingJS);
  }

  function rotateClockwise() {
    wrapper.cppPainting.rotateClockwise();
    wrapper.cachedOriginalJS = wrapper.cachedPaintingJS = undefined;
    paintingsStore.set($paintingsStore); // This triggers pullFromCpp
  }

  function rotateAnticlockwise() {
    wrapper.cppPainting.rotateAnticlockwise();
    wrapper.cachedOriginalJS = wrapper.cachedPaintingJS = undefined;
    paintingsStore.set($paintingsStore); // This triggers pullFromCpp
  }

  function remove() {
    const idToRemove = wrapper.cppPainting.stringId();

    $workspace.removePainting(idToRemove);

    paintingsStore.update((list) => list.filter((p) => p !== wrapper));

    wrapper.cppPainting.delete();
  }

  function crop() {
    showCropModal = true;
  }

  function handleCropConfirm() {
    showCropModal = false;
    wrapper.cachedPaintingJS = undefined;
    wrapper.cachedOriginalJS = undefined;
    updateCanvases();
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

  function updateCanvases() {
    renderPainting();
    renderOriginalImage();
  }

  onMount(updateCanvases);
</script>

{#if showCropModal}
  <CropModal
    painting={wrapper.cppPainting}
    onConfirm={handleCropConfirm}
    onCancel={() => (showCropModal = false)}
  />
{/if}

<SelectableCard
  bind:selected
  onToggle={() => {
    wrapper.selected = selected;
    $paintingsStore = $paintingsStore;
  }}
>
  <div class="transformation">
    <PaintingActions
      {rotateClockwise}
      {rotateAnticlockwise}
      {clone}
      {remove}
      {crop}
      {isCropped}
    />
    <PaintingCanvas bind:canvas={originalImageCanvas} />
    <PaintingParams
      bind:currentRatio
      bind:title
      bind:author
      bind:showMoreOptions
      onMetadataChange={syncMetadata}
      onVisualChange={syncVisualsAndRefresh}
    />
    <PaintingCanvas bind:canvas={paintingCanvas} />
  </div>

  <div
    style={`display: ${showMoreOptions ? "flex" : "none"}`}
    class="more-options"
  >
    <div class="more-options-section">
      <span class="more-options-section-label">
        <RulerIcon size="1.5em" />
        Resolution
      </span>

      <div
        style="
          display: flex;
          flex-direction: row;
          justify-content: center;
          align-items: center;
          gap: 10px"
      >
        <input
          id="range"
          type="range"
          min="3"
          max="8"
          bind:value={downscalePower}
          oninput={syncVisualsAndRefresh}
          step="1"
        />
        <span class="pill">
          {Math.pow(2, downscalePower)} pixels / block
        </span>
      </div>
    </div>

    <div class="more-options-section">
      <span class="more-options-section-label">
        <SquareIcon size="1.5em" />
        Frame options
      </span>
      <Wiwicheckbox
        bind:checked={disableFrame}
        onclick={() => {
          if (disableFrame) wrapper.cppPainting.useNoFrame();
          else wrapper.cppPainting.useProceduralFrame();

          wrapper.cachedPaintingJS = undefined;
          updateCanvases();
        }}
      >
        Disable frame
      </Wiwicheckbox>

      <div class="colour-container">
        <Button
          disabled={disableFrame}
          onclick={() => colourPicker.showPicker()}
        >
          Frame colour
          <div
            style={`width: 15px; height: 15px; background: ${frameTint}; border-radius: 100%; border: 1px solid #ccc`}
          ></div>
        </Button>

        <input
          bind:this={colourPicker}
          name="frame-colour"
          type="color"
          bind:value={frameTint}
          oninput={syncVisualsAndRefresh}
          onchange={syncVisualsAndRefresh}
        />
      </div>

      <Button
        disabled={disableFrame}
        icon="Dices"
        onclick={() => {
          frameSeed = "" + Math.floor(Number.MAX_SAFE_INTEGER * Math.random());
          syncVisualsAndRefresh();
        }}
      >
        Frame seed
      </Button>
    </div>

    <div class="more-options-section">
      <span class="more-options-section-label">
        <WrenchIcon size="1.5em" />
        Other options
      </span>
      <Wiwicheckbox bind:checked={placeable} onclick={syncMetadata}>
        Painting is placeable
      </Wiwicheckbox>
      <Wiwicheckbox bind:checked={hasStonecutterRecipe} onclick={syncMetadata}>
        Painting has stonecutter recipe
      </Wiwicheckbox>
    </div>
  </div>
</SelectableCard>

<style>
  .transformation {
    display: flex;
    align-items: stretch;
    gap: 10px;
  }

  .more-options {
    display: flex;
    flex-direction: column;
    gap: 10px;
    margin-top: 15px;
    padding-top: 15px;
    border-top: 1px solid #ccc;
  }

  @media (prefers-color-scheme: dark) {
    .more-options {
      border-top: 1px solid #555;
    }
  }

  .more-options-section {
    display: flex;
    gap: 10px;
    align-items: center;
    font-size: 90%;
  }

  .more-options-section-label {
    font-weight: 500;
    color: #333;
    width: 150px;
    display: flex;
    align-items: center;
    gap: 10px;
  }

  @media (prefers-color-scheme: dark) {
    .more-options-section-label {
      color: #eee;
    }
  }

  .colour-container {
    position: relative;
  }

  input[type="color"] {
    position: absolute;
    opacity: 0;
    top: 0;
    width: 100%;
    height: 100%;
    padding: 0;
    border: 0;
    pointer-events: none;
    z-index: -1;
  }
</style>
