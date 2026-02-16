<script lang="ts">
  import { invisibleItemFramesPack } from "../../stores/invisibleItemFrameStore";
  import Wiwicheckbox from "../Wiwicheckbox.svelte";

  export let move: (id: string) => void;
</script>

<div>
  <div class="app-card">
    <div class="check-option">
      <Wiwicheckbox bind:checked={$invisibleItemFramesPack}>
        Invisible item frames can be crafted as recipes
      </Wiwicheckbox>
    </div>

    <details>
      <summary>What does this option enable?</summary>
      ...
    </details>
  </div>

  <div class="pagination">
    <button class="secondary" onclick={() => move("paintings")}>Previous</button
    >
    <button onclick={() => move("generate")}>Next</button>
  </div>
</div>

<style>
  .check-option {
    margin-bottom: 20px;
    user-select: none;
    font-size: 120%;
  }

  .pagination {
    margin: 50px 0;
    display: flex;
    justify-content: center;
    gap: 10px;
  }
</style>
