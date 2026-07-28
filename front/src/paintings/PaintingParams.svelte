<script lang="ts">
  import { ProportionsIcon, TextCursorIcon, UserIcon } from "@lucide/svelte";
  import RatioSelect from "./RatioSelect.svelte";
  import Button from "../components/Button.svelte";

  export let title: string, author: string;
  export let showMoreOptions: boolean;

  export let onMetadataChange: (e?: Event) => void = () => {};
</script>

<div class="wrapper">
  <div class="transformation-ratio-selection">
    <div class="inputs">
      <label>
        <TextCursorIcon />
        <span>Title</span>
        <input
          type="text"
          bind:value={title}
          oninput={onMetadataChange}
          onchange={onMetadataChange}
        />
      </label>
      <label>
        <UserIcon />
        <span>Author</span>
        <input
          type="text"
          bind:value={author}
          oninput={onMetadataChange}
          onchange={onMetadataChange}
        />
      </label>
    </div>

    <img class="rightarrow" src="rightarrow.svg" alt="Right arrow" />

    <Button
      secondary
      hugeBorder={showMoreOptions}
      onclick={() => (showMoreOptions = !showMoreOptions)}
      icon="Cog"
    >
      More options
    </Button>
  </div>
</div>

<style>
  .wrapper {
    display: flex;
    justify-content: center;
    align-items: center;
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
  .inputs {
    display: flex;
    flex-direction: column;
    gap: 5px;
    margin-bottom: 10px;
  }
  .inputs label {
    display: flex;
    align-items: center;
    gap: 5px;
  }
  .inputs label input {
    max-width: 160px;
  }
  .inputs label span {
    display: inline-block;
    min-width: 60px;
  }
  .rightarrow {
    width: 100%;
    margin-bottom: 40px;
  }
</style>
