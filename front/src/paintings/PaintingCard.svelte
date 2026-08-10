<script lang="ts">
  import { onMount, tick } from "svelte";
  import {
    paintingsStore,
    type PaintingWrapper,
  } from "../stores/paintingsStore";
  import { workspace } from "../stores/workspaceStore";

  import PaintingActions from "./PaintingActions.svelte";
  import PaintingCanvas from "./PaintingCanvas.svelte";
  import CropModal from "./CropModal.svelte";
  import Wiwicheckbox from "../components/Wiwicheckbox.svelte";
  import SelectableCard from "../components/SelectableCard.svelte";
  import Button from "../components/Button.svelte";
  import {
    RulerIcon,
    WrenchIcon,
    ChevronRightIcon,
    ChevronLeftIcon,
    WandSparklesIcon,
  } from "@lucide/svelte";
  import { slide } from "svelte/transition";
  import Wiwinput from "../components/Wiwinput.svelte";
  import { prettyRatio } from "./ratios";

  export let wrapper: PaintingWrapper;

  let showMoreOptions = false;

  let showCropModal = false;
  let colourPicker: HTMLInputElement;

  let originalImageCanvas: HTMLCanvasElement;
  let paintingCanvas: HTMLCanvasElement;
  let ratioTabsContainer: HTMLDivElement;

  // Local UI state
  let title: string;
  let author: string;

  let ALL_RATIOS: string[] = [];
  let activeRatios: string[] = [];
  let previewRatio: string = "";

  let selected: boolean = false;

  let isCropped = false;
  let disableFrame = false;
  let proceduralFrame = false;
  let frameTint: string;
  let frameSeed: string;
  let placeable: boolean;
  let hasStonecutterRecipe: boolean;
  let brightness: number;
  let contrast: number;

  let downscalePower: number;

  $: pullFromCpp(wrapper);

  async function pullFromCpp(w: PaintingWrapper) {
    const p = w?.painting;
    if (!w || !p) return;

    title = p.title;
    author = p.author;
    selected = w.selected;
    placeable = p.placeable;
    hasStonecutterRecipe = p.hasStonecutterRecipe;

    isCropped =
      p.cropX !== 0 || p.cropY !== 0 || p.cropW !== 1 || p.cropH !== 1;

    let canvasNeedsUpdate = false;

    const rawRatios = typeof p.getRatios === "function" ? p.getRatios() : [];
    const cppRatios =
      rawRatios instanceof Promise ? await rawRatios : rawRatios || [];
    if (
      Array.isArray(cppRatios) &&
      cppRatios.join(",") !== activeRatios.join(",")
    ) {
      activeRatios = cppRatios;
      canvasNeedsUpdate = true;
    }

    // Ensuring the preview tab is looking at a valid ratio
    if (!activeRatios.includes(previewRatio) && activeRatios.length > 0) {
      previewRatio = activeRatios[0];
      canvasNeedsUpdate = true;
      centerActiveRatio();
    }

    let currentDownscale = Math.log2(w.painting.pixelPerBlock || 16);
    if (downscalePower !== currentDownscale) {
      downscalePower = currentDownscale;
      canvasNeedsUpdate = true;
    }

    if (brightness !== w.painting.brightness) {
      brightness = w.painting.brightness;
      canvasNeedsUpdate = true;
    }

    if (contrast !== w.painting.contrast) {
      contrast = w.painting.contrast;
      canvasNeedsUpdate = true;
    }

    const settings =
      typeof p?.getProceduralSettings === "function"
        ? p.getProceduralSettings()
        : null;
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

    if (canvasNeedsUpdate) {
      updateCanvases();
    }
  }

  function syncMetadata() {
    const p = wrapper?.painting;
    if (!p) return;
    p.title = title;
    p.author = author;
    wrapper.selected = selected;
    p.placeable = placeable;
    p.hasStonecutterRecipe = hasStonecutterRecipe;
  }

  function hasWrapperChanged() {
    const p = wrapper?.painting;
    if (!p) return false;
    if (p.pixelPerBlock !== Math.pow(2, downscalePower)) return true;
    if (p.brightness !== brightness) return true;
    if (Math.abs(p.contrast - contrast) > 0.001) return true;

    const settings =
      typeof p?.getProceduralSettings === "function"
        ? p.getProceduralSettings()
        : null;
    if (settings) {
      if (String(settings.seed) !== frameSeed) return true;
      if (settings.tintHex !== frameTint) return true;
    }
    return false;
  }

  async function syncVisualsAndRefresh() {
    const p = wrapper?.painting;
    if (!p || !hasWrapperChanged()) return;

    p.pixelPerBlock = Math.pow(2, downscalePower);

    const settings =
      typeof p?.getProceduralSettings === "function"
        ? p.getProceduralSettings()
        : null;
    if (settings) {
      settings.seed = BigInt(frameSeed);
      settings.tintHex = frameTint;
    }

    await p.setBrightness?.(brightness);
    await p.setContrast?.(contrast);

    updateCanvases();
  }

  function toggleRatio(ratio: string) {
    if (activeRatios.includes(ratio)) {
      if (activeRatios.length === 1) return;
      wrapper.painting.removeRatio(ratio);
    } else {
      wrapper.painting.addRatio(ratio);
    }

    paintingsStore.set($paintingsStore);
    centerActiveRatio();
  }

  function setPreviewRatio(ratio: string) {
    if (previewRatio === ratio) return;
    previewRatio = ratio;

    renderPainting();
    centerActiveRatio();
  }

  function prevRatio() {
    if (activeRatios.length <= 1) return;
    const idx = activeRatios.indexOf(previewRatio);
    const newIdx = idx <= 0 ? activeRatios.length - 1 : idx - 1;
    setPreviewRatio(activeRatios[newIdx]);
  }

  function nextRatio() {
    if (activeRatios.length <= 1) return;
    const idx = activeRatios.indexOf(previewRatio);
    const newIdx = idx >= activeRatios.length - 1 ? 0 : idx + 1;
    setPreviewRatio(activeRatios[newIdx]);
  }

  async function centerActiveRatio() {
    await tick();

    if (!ratioTabsContainer) return;

    const activeEl = ratioTabsContainer.querySelector(
      ".ratio-pill:not(.pill-inactive)",
    ) as HTMLElement;

    if (activeEl) {
      const centerPosition =
        activeEl.offsetLeft +
        activeEl.offsetWidth / 2 -
        ratioTabsContainer.offsetWidth / 2;

      ratioTabsContainer.scrollTo({
        left: centerPosition,
        behavior: "smooth",
      });
    }
  }

  function scalePixelCanvas(c: HTMLCanvasElement, imgW: number, imgH: number) {
    const containerSize = 330;

    c.width = imgW;
    c.height = imgH;

    const scale = Math.min(containerSize / imgW, containerSize / imgH);
    const cssW = Math.floor(imgW * scale);
    const cssH = Math.floor(imgH * scale);

    c.style.width = `${cssW}px`;
    c.style.height = `${cssH}px`;
  }

  function drawDataUrlToCanvas(canvas: HTMLCanvasElement, dataUrl: string) {
    if (!canvas || !dataUrl) return;
    const img = new Image();
    img.onload = () => {
      scalePixelCanvas(canvas, img.width, img.height);
      const ctx = canvas.getContext("2d");
      if (ctx) {
        ctx.imageSmoothingEnabled = false;
        ctx.drawImage(img, 0, 0);
      }
    };
    img.src = dataUrl;
  }

  async function renderOriginalImage() {
    const p = wrapper?.painting;
    if (!p) return;
    if (typeof p.getPaintingUrl === "function") {
      const dataUrl = await p.getPaintingUrl("");
      drawDataUrlToCanvas(originalImageCanvas, dataUrl);
    }
  }

  async function renderPainting() {
    const p = wrapper?.painting;
    if (!p) return;
    if (typeof p.getPaintingUrl === "function") {
      const dataUrl = await p.getPaintingUrl(previewRatio);
      drawDataUrlToCanvas(paintingCanvas, dataUrl);
    }
  }

  async function rotateClockwise() {
    await wrapper.painting.rotateClockwise();
    await pullFromCpp(wrapper);
    updateCanvases();
    paintingsStore.set($paintingsStore);
  }

  async function rotateAnticlockwise() {
    await wrapper.painting.rotateAnticlockwise();
    await pullFromCpp(wrapper);
    updateCanvases();
    paintingsStore.set($paintingsStore);
  }

  async function remove() {
    const target = wrapper?.painting;
    if (!target) return;
    if ($workspace) await $workspace.removePainting(target.stringId());
    paintingsStore.update((list) => list.filter((p) => p !== wrapper));
    if (typeof target.dispose === "function") await target.dispose();
  }

  function crop() {
    showCropModal = true;
  }

  function handleCropConfirm() {
    showCropModal = false;
    updateCanvases();
  }

  async function clone() {
    const target = wrapper?.painting;
    if (!target || !$workspace) return;

    let newPainting: any;
    if (typeof (target as any).clone === "function") {
      newPainting = await (target as any).clone();
    } else {
      newPainting = await $workspace.addPainting(target.stringId());
    }
    if (newPainting?.setTitle) {
      await newPainting.setTitle((title || "Painting") + " (copy)");
    }

    paintingsStore.update((list) => {
      const index = list.findIndex(
        (p) => p.painting.stringId() === target.stringId(),
      );

      const newList = [...list];
      newList.splice(index + 1, 0, {
        painting: newPainting,
        selected: false,
      });

      return newList;
    });
  }

  function updateCanvases() {
    renderPainting();
    renderOriginalImage();
  }

  onMount(async () => {
    if ($workspace) {
      ALL_RATIOS = await $workspace.getReasonableRatios();
    }
    updateCanvases();
  });
</script>

{#if showCropModal}
  <CropModal
    painting={wrapper.painting}
    onConfirm={handleCropConfirm}
    onCancel={() => (showCropModal = false)}
  />
{/if}

<SelectableCard
  bind:selected
  showCheckbox={false}
  onToggle={() => {
    wrapper.selected = selected;
    $paintingsStore = $paintingsStore;
  }}
>
  <div class="card-header">
    <div class="header-left">
      <div class="inputs">
        <Wiwinput
          type="text"
          placeholder="Title"
          bind:value={title}
          oninput={syncMetadata}
          onchange={syncMetadata}
        />

        <!-- &mdash; -->

        <Wiwinput
          type="text"
          placeholder="Author"
          bind:value={author}
          oninput={syncMetadata}
          onchange={syncMetadata}
        />
      </div>
    </div>

    <div class="header-right">
      <Button
        secondary
        hugeBorder={showMoreOptions}
        onclick={() => (showMoreOptions = !showMoreOptions)}
        title="More options for this painting"
        icon="SlidersHorizontal"
      >
        Options
      </Button>

      <PaintingActions
        bind:selected
        {clone}
        {remove}
        updateSelected={() => {
          wrapper.selected = selected;
          $paintingsStore = $paintingsStore;
        }}
      />
    </div>
  </div>

  <div class="transformation">
    <div class="painting-card-main-content-container">
      <div class="original-painting-container">
        <PaintingCanvas bind:canvas={originalImageCanvas} />

        <div class="original-painting-actions">
          <Button
            onclick={crop}
            title="Crop image"
            icon="Crop"
            hugeBorder={isCropped}
          />

          <Button
            onclick={rotateAnticlockwise}
            title="Rotate anticlockwise"
            icon="RotateCcw"
          />

          <Button
            onclick={rotateClockwise}
            title="Rotate clockwise"
            icon="RotateCw"
          />
        </div>
      </div>

      <div class="arrow-wrapper">
        <div class="transformation-ratio-selection">
          <img class="rightarrow" src="rightarrow.svg" alt="Right arrow" />
        </div>
      </div>

      <div class="preview-container">
        <PaintingCanvas bind:canvas={paintingCanvas} />

        <div class="ratio-tabs-container">
          <button
            class="chevron-btn"
            onclick={prevRatio}
            disabled={activeRatios.length <= 1}
            aria-label="Previous Ratio"
            title="Previous Ratio"
          >
            <ChevronLeftIcon size="1.2em" />
          </button>

          <div class="ratio-tabs" bind:this={ratioTabsContainer}>
            {#each activeRatios as ratio}
              <!-- svelte-ignore a11y_click_events_have_key_events -->
              <!-- svelte-ignore a11y_no_static_element_interactions -->
              <span
                class="pill ratio-pill"
                class:pill-inactive={previewRatio !== ratio}
                onclick={() => setPreviewRatio(ratio)}
              >
                {prettyRatio(ratio)}
              </span>
            {/each}
          </div>

          <button
            class="chevron-btn"
            onclick={nextRatio}
            disabled={activeRatios.length <= 1}
            aria-label="Next Ratio"
            title="Next Ratio"
          >
            <ChevronRightIcon size="1.2em" />
          </button>
        </div>
      </div>
    </div>
  </div>

  {#if showMoreOptions}
    <div class="more-options" transition:slide={{ duration: 100 }}>
      <div class="extra-option-card">
        <div class="title">
          <RulerIcon size="1.2em" />
          <span>Sizes and resolution</span>
        </div>

        <div class="extra-option-card-sections">
          <div class="section">
            <div class="section-title">
              <span>Sizes</span>
            </div>

            <div class="section-body section-sizes">
              {#each ALL_RATIOS as r}
                <!-- svelte-ignore a11y_click_events_have_key_events -->
                <!-- svelte-ignore a11y_no_static_element_interactions -->
                <span
                  class="pill ratio-pill"
                  class:pill-inactive={!activeRatios.includes(r)}
                  onclick={() => toggleRatio(r)}
                >
                  {prettyRatio(r)}
                </span>
              {/each}
            </div>
          </div>

          <div class="section">
            <div class="section-title">
              <span>Resolution</span>
            </div>

            <div class="section-body section-resolution">
              <Wiwinput
                type="range"
                min="3"
                max="8"
                bind:value={downscalePower}
                oninput={syncVisualsAndRefresh}
                step="1"
              />
              <span>
                <span
                  style="
                    display: inline-block;
                    min-width: 30px;
                    text-align: right;"
                >
                  {Math.pow(2, downscalePower)}
                </span> px/block
              </span>
            </div>
          </div>
        </div>
      </div>

      <div class="extra-option-card">
        <div class="title">
          <WandSparklesIcon size="1.2em" />
          <span>Appearance</span>
        </div>

        <div class="extra-option-card-sections">
          <div class="section">
            <div class="section-title">
              <span>Brightness</span>
            </div>

            <div class="section-body section-brightness">
              <Wiwinput
                type="range"
                min="-255"
                max="255"
                bind:value={brightness}
                oninput={syncVisualsAndRefresh}
                step="1"
              />

              <Button
                small
                transparent
                disabled={brightness == 0}
                icon="Undo"
                onclick={() => {
                  brightness = 0;
                  syncVisualsAndRefresh();
                }}
              >
                Reset
              </Button>
            </div>
          </div>

          <div class="section">
            <div class="section-title">
              <span>Contrast</span>
            </div>

            <div class="section-body section-contrast">
              <Wiwinput
                type="range"
                min="0"
                max="10"
                bind:value={contrast}
                oninput={syncVisualsAndRefresh}
                step="0.01"
              />

              <Button
                small
                transparent
                disabled={contrast == 1}
                icon="Undo"
                onclick={() => {
                  contrast = 1;
                  syncVisualsAndRefresh();
                }}
              >
                Reset
              </Button>
            </div>
          </div>

          <div class="section">
            <div class="section-title">
              <span>Frame</span>
            </div>

            <div class="section-body section-frame">
              <Wiwicheckbox
                small
                bind:checked={disableFrame}
                onclick={() => {
                  if (disableFrame) wrapper.painting.useNoFrame();
                  else wrapper.painting.useProceduralFrame();

                  updateCanvases();
                }}
              >
                Disable frame
              </Wiwicheckbox>

              <div class="when-frame-enabled">
                <div class="colour-container">
                  <Button
                    small
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
                  small
                  disabled={disableFrame}
                  icon="Dices"
                  onclick={() => {
                    frameSeed =
                      "" + Math.floor(Number.MAX_SAFE_INTEGER * Math.random());
                    syncVisualsAndRefresh();
                  }}
                >
                  Frame seed
                </Button>
              </div>
            </div>
          </div>
        </div>
      </div>

      <div class="extra-option-card">
        <div class="title">
          <WrenchIcon size="1.2em" />
          <span>Behaviour in game</span>
        </div>

        <div class="extra-option-card-sections">
          <Wiwicheckbox small bind:checked={placeable} onclick={syncMetadata}>
            Painting is placeable
          </Wiwicheckbox>

          <Wiwicheckbox
            small
            bind:checked={hasStonecutterRecipe}
            onclick={syncMetadata}
          >
            Painting has stonecutter recipe
          </Wiwicheckbox>
        </div>
      </div>
    </div>
  {/if}
</SelectableCard>

<style>
  .ratio-pill {
    cursor: pointer;
    background-color: #646cff;
    color: white;
    flex-shrink: 0;
    white-space: nowrap;
  }

  .pill-inactive {
    background-color: transparent !important;
    color: #888 !important;
  }

  .transformation {
    display: flex;
    align-items: stretch;
    gap: 10px;
  }

  .painting-card-main-content-container {
    display: flex;
    align-items: stretch;
    gap: 10px;
    flex: 1;
  }

  .preview-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 8px;
    justify-content: space-between;
  }

  .ratio-tabs-container {
    display: flex;
    align-items: center;
    gap: 4px;
    width: 100%;
    max-width: calc(330px + 10px);
    height: 48px;
  }

  .chevron-btn {
    background: transparent;
    border: none;
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
    padding: 2px;
    color: #666;
    border-radius: 4px;
    transition:
      background-color 0.1s ease,
      color 0.1s ease;
  }

  .chevron-btn:hover:not(:disabled) {
    color: #333;
    background-color: rgba(0, 0, 0, 0.05);
  }

  .chevron-btn:disabled {
    opacity: 0.3;
    cursor: not-allowed;
  }

  @media (prefers-color-scheme: dark) {
    .chevron-btn {
      color: #aaa;
    }
    .chevron-btn:hover:not(:disabled) {
      color: #fff;
      background-color: rgba(255, 255, 255, 0.1);
    }
  }

  .ratio-tabs {
    position: relative;
    width: 100%;
    min-width: 0;
    padding: 4px;
    flex: 1;
    display: flex;
    justify-content: safe center;
    overflow-x: auto;
    scroll-behavior: smooth;
    scrollbar-width: none;
    gap: 4px;
    border-radius: 8px;
    z-index: 0;
  }

  .ratio-tabs::-webkit-scrollbar {
    display: none;
  }

  .more-options {
    display: flex;
    flex-direction: column;
    gap: 10px;
    margin-top: 15px;
  }

  .colour-container {
    position: relative;
    display: inline-flex;
  }

  .colour-container input[type="color"] {
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

  .original-painting-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 10px;
  }

  .original-painting-actions {
    display: flex;
    gap: 10px;
  }

  .card-header {
    display: flex;
    justify-content: space-between;
    margin-bottom: 10px;

    & .header-right {
      display: flex;
      gap: 12px;
      align-items: center;
    }
  }

  .inputs {
    display: flex;
    gap: 10px;
  }

  .extra-option-card {
    border: 1px solid #ddd;
    border-radius: 10px;
    padding: 12px;

    & .title {
      font-weight: 600;
      margin-bottom: 20px;
      display: flex;
      align-items: center;
      gap: 6px;
    }
  }

  @media (prefers-color-scheme: dark) {
    .extra-option-card {
      border-color: #444;
    }
  }

  .extra-option-card-sections {
    display: flex;
    flex-direction: column;
    gap: 12px;

    & .section {
      display: flex;

      & .section-title {
        width: 150px;
      }

      & .section-body {
        flex: 1;
      }
    }
  }

  .section-sizes {
    display: flex;
    flex-wrap: wrap;
    gap: 6px;
  }

  .section-resolution {
    display: flex;
    align-items: center;
  }

  .section-frame {
    display: flex;
    flex-direction: row;
    align-items: center;
    gap: 20px;

    & .when-frame-enabled {
      display: flex;
      flex-wrap: wrap;
      gap: 6px;
    }
  }

  .section-brightness,
  .section-contrast {
    display: flex;
    align-items: center;
  }

  .arrow-wrapper {
    display: flex;
    justify-content: center;
    align-items: center;
    flex: 1;
  }
  .transformation-ratio-selection {
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    align-items: center;
    width: 100%;
    padding: 0 10px;
    user-select: none;
  }

  .rightarrow {
    width: 100%;
    margin-bottom: 40px;
  }
</style>
