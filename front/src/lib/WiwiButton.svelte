<script>
	let props = $props();
</script>

<button class:disabled={props.disabled}>
  {props.text}
</button>

<style>
  .disabled {
    opacity: 0.5;
    pointer-events: none;
  }
</style>
