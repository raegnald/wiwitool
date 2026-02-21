<script lang="ts">
  import { ProportionsIcon, TextCursorIcon, UserIcon } from "@lucide/svelte";
  import RatioSelect from "./RatioSelect.svelte";

  export let currentRatio: string, title: string, author: string;
</script>

<div class="transformation-ratio-selection">
  <div class="inputs">
    <label>
      <ProportionsIcon />
      <span> Aspect ratio&nbsp; </span>
      <RatioSelect bind:value={currentRatio} />
    </label>
  </div>
  <img class="rightarrow" src="rightarrow.svg" alt="Right arrow" />

  <div class="inputs">
    <label>
      <TextCursorIcon />
      <span> Title </span><input type="text" bind:value={title} />
    </label>
    <label>
      <UserIcon />
      <span> Author </span><input type="text" bind:value={author} />
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
    user-select: none;
  }
  .inputs {
    display: flex;
    flex-direction: column;
    gap: 5px;
    margin-top: 30px;
  }
  .inputs label {
    display: flex;
    align-items: center;
    gap: 5px;
  }
  .inputs label input {
    max-width: 200px;
  }
  .inputs label span {
    display: inline-block;
    min-width: 60px;
  }
  .rightarrow {
    width: 100%;
  }
</style>
