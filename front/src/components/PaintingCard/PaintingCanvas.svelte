<script lang="ts">
  export let canvas: HTMLCanvasElement;
</script>

<div class="canvas-transformation-container">
  <canvas bind:this={canvas}></canvas>
</div>

<style>
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

  canvas {
    image-rendering: pixelated;
  }

  @media (prefers-color-scheme: dark) {
    .canvas-transformation-container {
      background: #222;
    }
  }
</style>
