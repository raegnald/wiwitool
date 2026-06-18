<script lang="ts">
  import { onMount } from "svelte";
  import type { Painting } from "../bindings/wiwitool";
  import Button from "../components/Button.svelte";

  let {
    painting,
    onConfirm,
    onCancel,
  }: {
    painting: Painting;
    onConfirm: () => void;
    onCancel: () => void;
  } = $props();

  let dialog: HTMLDialogElement;
  let canvas: HTMLCanvasElement;

  let originalW: number;
  let originalH: number;

  let cropX = $state(painting.cropX);
  let cropY = $state(painting.cropY);
  let cropW = $state(painting.cropW);
  let cropH = $state(painting.cropH);

  let isDragging = $state(false);
  let dragStart = $state({ x: 0, y: 0 });
  let currentPos = $state({ x: 0, y: 0 });

  function handleMouseDown(e: MouseEvent) {
    const rect = canvas.getBoundingClientRect();
    isDragging = true;
    dragStart = {
      x: (e.clientX - rect.left) / rect.width,
      y: (e.clientY - rect.top) / rect.height,
    };
    currentPos = { ...dragStart };
  }

  function handleMouseMove(e: MouseEvent) {
    if (!isDragging) return;
    const rect = canvas.getBoundingClientRect();
    currentPos = {
      x: (e.clientX - rect.left) / rect.width,
      y: (e.clientY - rect.top) / rect.height,
    };

    updateCropFromDrag();
  }

  function handleMouseUp() {
    isDragging = false;
  }

  function updateCropFromDrag() {
    const x1 = Math.min(dragStart.x, currentPos.x);
    const y1 = Math.min(dragStart.y, currentPos.y);
    const x2 = Math.max(dragStart.x, currentPos.x);
    const y2 = Math.max(dragStart.y, currentPos.y);

    cropX = Math.max(0, Math.min(1, x1));
    cropY = Math.max(0, Math.min(1, y1));
    cropW = Math.max(0.01, Math.min(1 - cropX, x2 - x1));
    cropH = Math.max(0.01, Math.min(1 - cropY, y2 - y1));
  }

  function resetCrop() {
    cropX = 0;
    cropY = 0;
    cropW = 1;
    cropH = 1;
  }

  function confirm() {
    painting.setCrop(cropX, cropY, cropW, cropH);
    onConfirm();
    dialog.close();
  }

  function cancel() {
    onCancel();
    dialog.close();
  }

  onMount(() => {
    const imgData = painting.originalData();
    originalW = imgData.width;
    originalH = imgData.height;

    canvas.width = originalW;
    canvas.height = originalH;

    const ctx = canvas.getContext("2d");
    if (ctx) {
      const pixels = imgData.getPixels();
      const clamped = new Uint8ClampedArray(pixels);
      const jsImgData = new ImageData(clamped, originalW, originalH);
      ctx.putImageData(jsImgData, 0, 0);
    }
    imgData.delete();

    dialog.showModal();
  });

  let cropRectStyle = $derived(`
    left: ${cropX * 100}%;
    top: ${cropY * 100}%;
    width: ${cropW * 100}%;
    height: ${cropH * 100}%;
  `);
</script>

<dialog bind:this={dialog}>
  <div class="modal-content">
    <h3>Crop Painting</h3>
    <p>Click and drag on the image to select the area you want to use.</p>

    <div class="cropper-container">
      <div class="canvas-wrapper">
        <canvas
          bind:this={canvas}
          onmousedown={handleMouseDown}
          onmousemove={handleMouseMove}
          onmouseup={handleMouseUp}
          onmouseleave={handleMouseUp}
        ></canvas>

        <div class="overlay">
          <div class="crop-rect" style={cropRectStyle}>
            <div class="handle top-left"></div>
            <div class="handle top-right"></div>
            <div class="handle bottom-left"></div>
            <div class="handle bottom-right"></div>
          </div>
        </div>
      </div>
    </div>

    <div class="modal-actions">
      <Button icon="RotateCcw" onclick={resetCrop}>Reset</Button>
      <div style="flex: 1"></div>
      <Button transparent onclick={cancel} icon="X">Cancel</Button>
      <Button primary onclick={confirm} icon="Check">Confirm crop</Button>
    </div>
  </div>
</dialog>

<style>
  dialog {
    padding: 0;
    border: none;
    border-radius: 8px;
    background: white;
    max-width: 90vw;
    max-height: 90vh;
    box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
  }

  @media (prefers-color-scheme: dark) {
    dialog {
      background: #222;
      color: white;
    }
  }

  .modal-content {
    display: flex;
    flex-direction: column;
    padding: 20px;
    gap: 15px;
  }

  h3 {
    margin: 0;
  }

  p {
    margin: 0;
    font-size: 0.9em;
    opacity: 0.8;
  }

  .cropper-container {
    user-select: none;
    background: #f0f0f0;
    display: flex;
    justify-content: center;
    align-items: center;
    overflow: auto;
    max-height: 60vh;
    border: 1px solid #ccc;
  }

  @media (prefers-color-scheme: dark) {
    .cropper-container {
      background: #111;
      border-color: #444;
    }
  }

  .canvas-wrapper {
    position: relative;
    display: flex;
  }

  canvas {
    display: block;
    max-width: 100%;
    max-height: 60vh;
    height: auto;
    cursor: crosshair;
  }

  .overlay {
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    pointer-events: none;
    overflow: hidden;
  }

  .crop-rect {
    position: absolute;
    border: 2px solid #fff;
    box-shadow: 0 0 0 9999px rgba(0, 0, 0, 0.5);
    box-sizing: border-box;
  }

  .handle {
    position: absolute;
    width: 8px;
    height: 8px;
    background: #fff;
    border: 1px solid #000;
  }

  .top-left {
    top: -4px;
    left: -4px;
  }
  .top-right {
    top: -4px;
    right: -4px;
  }
  .bottom-left {
    bottom: -4px;
    left: -4px;
  }
  .bottom-right {
    bottom: -4px;
    right: -4px;
  }

  .modal-actions {
    display: flex;
    gap: 10px;
    margin-top: 10px;
  }
</style>
