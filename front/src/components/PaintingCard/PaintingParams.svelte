<script lang="ts">
  import RatioSelect from "./RatioSelect.svelte";

  export let currentRatio: string, title: string, author: string;
</script>

<div class="transformation-ratio-selection">
  <div>
    <span>Aspect ratio </span>
    <RatioSelect bind:value={currentRatio} />
  </div>
  <img class="rightarrow" src="rightarrow.svg" alt="Right arrow" />

  <div class="inputs">
    <label>
      <span>Title </span><input type="text" bind:value={title} />
    </label>
    <label>
      <span>Author </span><input type="text" bind:value={author} />
    </label>
  </div>
</div>

<style>
  .transformation-ratio-selection {
    display: flex;
    flex-direction: column;
    align-items: center;
    width: 100%;
    padding: 10px;
  }
  .inputs {
    display: flex;
    flex-direction: column;
    gap: 5px;
    margin-top: 30px;
  }
  .inputs label span {
    display: inline-block;
    min-width: 60px;
    user-select: none;
  }
  .rightarrow {
    width: 100%;
  }
</style>
