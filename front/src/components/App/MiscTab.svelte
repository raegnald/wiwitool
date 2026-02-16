<script lang="ts">
  export let move: (id: string) => void;
</script>

<div>
  <div class="app-card">
    <div class="check-option">
      <input type="checkbox" id="invisible-item-frames" />
      <label for="invisible-item-frames"
        >Invisible item frames can be crafted as recipes</label
      >
    </div>

    <details>
      <summary>What does this option enable?</summary>
      ...
    </details>
  </div>

  <div class="next">
    <button onclick={() => move("generate")}>Next</button>
  </div>
</div>

<style>
  .check-option {
    margin-bottom: 20px;
    user-select: none;
    font-size: 120%;
  }

  .next {
    margin: 50px 0;
    display: flex;
    justify-content: center;
  }
</style>
