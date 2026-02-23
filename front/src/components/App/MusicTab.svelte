<script lang="ts">
  import { InfoIcon } from "@lucide/svelte";
  import Wiwicheckbox from "../Wiwicheckbox.svelte";
  import DropZone from "./DropZone.svelte";
  import HelpUsingPaintingsTool from "./HelpUsingPaintingsTool.svelte";

  export let move: (id: string) => void;
  $: showingInfoMusic = false;
</script>

<div>
  <div class="title-component">
    <h2>Load your music</h2>
    <button onclick={() => (showingInfoMusic = !showingInfoMusic)}>
      Information <InfoIcon size="1em" />
    </button>
  </div>
  <div class="infoContainer app-card" class:showingInfoMusic>
    ... text here ...
  </div>
  <DropZone>
    <span>Drag and drop music here</span><br /><code>music.ogg</code>
  </DropZone>

  <div class="app-card"></div>

  <div class="pagination">
    <button class="secondary" onclick={() => move("misc")}>Previous</button>
    <button onclick={() => move("generate")}>Next</button>
  </div>
</div>

<style>
  .pagination {
    margin: 50px 0;
    display: flex;
    justify-content: center;
    gap: 10px;
  }

  .infoContainer {
    display: none;
  }

  .infoContainer.showingInfoMusic {
    display: block;
    margin-bottom: 20px;
  }

  button {
    display: flex;
    align-items: center;
    gap: 5px;
  }
</style>
