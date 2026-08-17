<script lang="ts">
  let { children, ...restProps } = $props();
</script>

<span {...restProps}>
  <!-- {@render children?.()} -->
</span>

<style>
  span {
    margin-top: 5px;
    margin-bottom: 5px;
    /*padding-top: 10px;
    padding-left: 20px;*/
    color: grey;
    border-top: 1px solid lightgrey;
    cursor: default;

    @media (prefers-color-scheme: dark) {
      & {
        border-color: #444;
      }
    }
  }
</style>
