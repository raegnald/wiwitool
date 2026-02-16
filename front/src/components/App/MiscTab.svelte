<script lang="ts">
  import { invisibleItemFramesPack } from "../../stores/invisibleItemFrameStore";

  export let move: (id: string) => void;
</script>

<div>
  <div class="app-card">
    <div class="check-option">
      <input
        type="checkbox"
        id="invisible-item-frames"
        checked={$invisibleItemFramesPack}
        onclick={(e: Event) => invisibleItemFramesPack.set(e.target.checked)}
      />
      <label for="invisible-item-frames"
        >Invisible item frames can be crafted as recipes</label
      >
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
