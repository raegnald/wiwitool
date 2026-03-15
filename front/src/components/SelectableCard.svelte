<script lang="ts">
  import Wiwicheckbox from "./Wiwicheckbox.svelte";

  let {
    selected = $bindable(),
    onToggle = (() => {}) as () => any,
    children,
    ...props
  } = $props();
</script>

<div class="app-card card">
  <div class="card-padding" class:selected>
    <div class="floating-checkbox">
      <Wiwicheckbox bind:checked={selected} onclick={onToggle} />
    </div>

    {@render children()}
  </div>
</div>

<style>
  .card {
    margin: 20px 0px;
    display: flex;
    align-items: center;
    gap: 1rem;
    flex-wrap: nowrap;
    justify-content: space-between;
    padding: 0;
  }

  .card-padding {
    min-height: 42px;

    position: relative;
    width: 100%;
    border: 5px solid transparent;
    padding: 15px;
    transition: 100ms;
    border-radius: 11px;
    overflow-x: scroll;
  }

  .card-padding.selected {
    border-color: #646cff;
  }

  .floating-checkbox {
    position: absolute;
    top: 15px;
    left: 15px;
  }
</style>
