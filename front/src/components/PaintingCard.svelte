<script>
  import { onMount } from "svelte";
  import { paintingsStore } from "../stores";

  // 1. CHANGE THIS NAME to match what you pass in App.svelte
  export let wrapper;

  let canvas;
  // 2. Update references here
  let title = wrapper.title;
  let author = wrapper.author;

  // Reactive: Update C++ object when inputs change
  // 3. Update references inside the reactive block
  $: if (wrapper.cppPainting) {
    wrapper.cppPainting.title = title;
    wrapper.cppPainting.author = author;
  }

  function renderIcon() {
    // 4. Update references
    if (!wrapper.cppPainting || !canvas) return;

    const iconData = wrapper.cppPainting.iconData();
    const width = iconData.width;
    const height = iconData.height;

    const pixelView = iconData.getPixels();
    const clampedArray = new Uint8ClampedArray(pixelView);
    const imageData = new ImageData(clampedArray, width, height);

    canvas.width = width;
    canvas.height = height;
    const ctx = canvas.getContext("2d");
    ctx.putImageData(imageData, 0, 0);

    iconData.delete();
  }

  function rotate() {
    // 5. Update references
    wrapper.cppPainting.rotateClockwise();
    renderIcon();
  }

  function remove() {
    // 6. Update references
    wrapper.cppPainting.delete();
    paintingsStore.update((list) => list.filter((p) => p.id !== wrapper.id));
  }

  onMount(() => {
    renderIcon();
  });
</script>

<div class="card">
  <canvas bind:this={canvas} class="pixel-art"></canvas>

  <div class="inputs">
    <label>
      <span>Title: </span><input type="text" bind:value={title} />
    </label>
    <label>
      <span>Author: </span><input type="text" bind:value={author} />
    </label>
  </div>

  <div class="actions">
    <button on:click={rotate}>Rotate ↻</button>
    <button on:click={remove} class="delete">Remove</button>
  </div>
</div>

<style>
  .card {
    border: 1px solid #ccc;
    padding: 10px;
    margin: 10px;
    display: flex;
    align-items: center;
    gap: 1rem;
    background: white;
    border-radius: 8px;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  }
  .pixel-art {
    image-rendering: pixelated;
    width: 64px;
    height: 64px;
    /*border: 1px solid #333;*/
    /*background: #eee;*/
  }
  .inputs {
    display: flex;
    flex-direction: column;
    gap: 5px;
  }
  .inputs label span {
    display: inline-block;
    min-width: 60px;
    user-select: none;
  }
  .actions {
    display: flex;
    flex-direction: column;
    gap: 5px;
  }
  .delete {
    background-color: #ffcccc;
    border: 1px solid #ffaaaa;
  }
  button {
    cursor: pointer;
    padding: 4px 8px;
  }
</style>
