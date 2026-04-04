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
  import SelectableCard from "../components/SelectableCard.svelte";
  import Button from "../components/Button.svelte";
  import { RulerIcon, SquareIcon, WrenchIcon } from "@lucide/svelte";

  export let wrapper: PaintingWrapper;

  let showMoreOptions = false;
  let colourPicker: HTMLInputElement;

  let originalImageCanvas: HTMLCanvasElement;
  let paintingCanvas: HTMLCanvasElement;

  // Local UI state
  let title: string;
  let author: string;
  let currentRatio: string;

  let selected: boolean;

  let disableFrame = false;
  let proceduralFrame = wrapper.cppPainting.isFrameProcedural();
  let frameTint: string;
  let frameSeed: string;
  let placeable: boolean;

  let downscalePower: number;

  // External changes that modify local UI state
  $: if (wrapper && wrapper.cppPainting) {
    title = wrapper.cppPainting.title;
    author = wrapper.cppPainting.author;
    currentRatio = wrapper.cppPainting.ratio;
    selected = wrapper.selected;
    placeable = wrapper.cppPainting.placeable;
    downscalePower = Math.log2(wrapper.cppPainting.pixelPerBlock);

    const settings = wrapper.cppPainting.getProceduralSettings();
    if (settings) {
      frameSeed = String(settings.seed);
      frameTint = settings.tintHex;

      wrapper = wrapper;
    }

    refresh();
  }

  function syncToCpp() {
    if (!wrapper.cppPainting) return;

    wrapper.cppPainting.title = title;
    wrapper.cppPainting.author = author;
    wrapper.cppPainting.ratio = currentRatio;
    wrapper.selected = selected;
    wrapper.cppPainting.placeable = placeable;
    wrapper.cppPainting.pixelPerBlock = Math.pow(2, downscalePower);

    const settings = wrapper.cppPainting.getProceduralSettings();
    if (settings) {
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

<SelectableCard
  bind:selected
  onToggle={() => {
    wrapper.selected = selected;
    $paintingsStore = $paintingsStore;
  }}
>
  <div class="transformation">
    <PaintingActions {rotateClockwise} {rotateAnticlockwise} {clone} {remove} />
    <PaintingCanvas bind:canvas={originalImageCanvas} />
    <PaintingParams
      bind:currentRatio
      bind:title
      bind:author
      bind:showMoreOptions
      onchange={syncToCpp}
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
          oninput={syncToCpp}
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

          refresh();
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
          oninput={syncToCpp}
          onchange={syncToCpp}
        />
      </div>

      <Button
        disabled={disableFrame}
        icon="Dices"
        onclick={() => {
          frameSeed = "" + Math.floor(Number.MAX_SAFE_INTEGER * Math.random());
          syncToCpp();
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
      <Wiwicheckbox bind:checked={placeable} onclick={syncToCpp}>
        Painting is placeable
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
