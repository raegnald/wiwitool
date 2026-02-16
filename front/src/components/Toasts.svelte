<script lang="ts">
  import { flip } from "svelte/animate";
  import { slide } from "svelte/transition";
  import { toasts } from "../stores/toastsStore";
</script>

<div class="toast-container">
  {#each $toasts as t (t.id)}
    <div
      class="toast {t.type}"
      animate:flip
      transition:slide={{ duration: 100, axis: "y" }}
    >
      {t.message}
    </div>
  {/each}
</div>

<style>
  .toast-container {
    position: fixed;
    bottom: 20px;
    right: 20px;
    z-index: 9999;
    display: flex;
    flex-direction: column;
    gap: 10px;
    pointer-events: none;
  }

  .toast {
    padding: 12px 20px;
    border-radius: 12px;
    color: white;
    font-weight: 500;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
    pointer-events: auto;
    min-width: 250px;
  }

  .info {
    background-color: #646cff;
  }
  .warning {
    background-color: #ff9800;
  }
  .error {
    background-color: #f44336;
  }
</style>
