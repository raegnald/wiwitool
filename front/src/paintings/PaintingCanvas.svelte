<script lang="ts">
  export let canvas: HTMLCanvasElement;
</script>

<div class="canvas-transformation-container">
  <canvas bind:this={canvas}></canvas>
</div>

<style>
  .canvas-transformation-container {
    width: 330px;
    height: 330px;
    min-width: 330px;
    min-height: 330px;
    flex-shrink: 0;
    padding: 10px;
    display: flex;
    justify-content: center;
    align-items: center;
    background: #eee;
    border-radius: 8px;
    z-index: 1;
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
