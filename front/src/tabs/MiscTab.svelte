<script lang="ts">
  import { workspace } from "../stores/workspaceStore";

  import Button from "../components/Button.svelte";
  import Wiwicheckbox from "../components/Wiwicheckbox.svelte";

  export let move: (id: string) => void;

  let iifEnabled = $workspace ? $workspace.invisibleItemFrames : false;

  $: if ($workspace) {
    $workspace.invisibleItemFrames = iifEnabled;
  }

  let showingInfoIIF = false;
</script>

<div>
  <div class="app-card">
    <div class="check-option title-component">
      <Wiwicheckbox bind:checked={iifEnabled}>
        Invisible item frames can be crafted as recipes
      </Wiwicheckbox>

      <Button
        transparent
        onclick={() => (showingInfoIIF = !showingInfoIIF)}
        icon="Info"
      />
    </div>

    <div class="hidden" class:showingInfoIIF>
      Adds two simple item recipes that allow you to shapelessly craft invisible
      item frames and glow item frames using glass panes, fermented spider eyes
      and golden carrots.
    </div>
  </div>

  <div class="pagination">
    <Button
      class="secondary"
      onclick={() => move("paintings")}
      icon="ArrowLeft"
    >
      Previous
    </Button>
    <Button onclick={() => move("generate")} icon="ArrowRight">Next</Button>
  </div>
</div>

<style>
  .check-option {
    font-size: 120%;
  }

  .pagination {
    margin: 50px 0;
    display: flex;
    justify-content: center;
    gap: 10px;
  }

  .hidden {
    display: none;
  }

  .hidden.showingInfoIIF {
    display: block;
    margin-top: 10px;
  }
</style>
