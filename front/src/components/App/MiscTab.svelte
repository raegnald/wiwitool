<script lang="ts">
  import { InfoIcon } from "@lucide/svelte";
  import { invisibleItemFramesPack } from "../../stores/invisibleItemFrameStore";
  import Wiwicheckbox from "../Wiwicheckbox.svelte";

  export let move: (id: string) => void;
  $: showingInfoIIF = false;
</script>

<div>
  <div class="app-card">
    <div class="check-option title-component">
      <Wiwicheckbox bind:checked={$invisibleItemFramesPack}>
        Invisible item frames can be crafted as recipes
      </Wiwicheckbox>
      <button onclick={() => (showingInfoIIF = !showingInfoIIF)}>
        <InfoIcon />
      </button>
    </div>
    <div class="hidden" class:showingInfoIIF>
      Adds two simple item recipes that allow you to shapelessly craft invisible
      item frames and glow item frames using glass panes, fermented spider eyes
      and golden carrots.
    </div>
  </div>

  <div class="pagination">
    <button class="secondary" onclick={() => move("paintings")}>Previous</button
    >
    <button onclick={() => move("generate")}>Next</button>
  </div>
</div>

<style>
  .check-option {
    user-select: none;
    font-size: 120%;
  }

  .pagination {
    margin: 50px 0;
    display: flex;
    justify-content: center;
    gap: 10px;
  }

  .title-component > button {
    display: flex;
    padding: 0.5em;
    font-size: 24px;
    align-items: center;
  }

  .hidden {
    display: none;
  }

  .hidden.showingInfoIIF {
    display: block;
    margin-top: 10px;
  }
</style>
