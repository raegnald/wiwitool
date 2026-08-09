<script lang="ts">
  import type { HTMLInputAttributes } from "svelte/elements";
  import { fly, slide } from "svelte/transition";

  interface Props extends HTMLInputAttributes {
    placeholder?: string;
    error?: string;
    value?: string | number;
  }

  let { placeholder, error, value = $bindable(), ...rest }: Props = $props();
</script>

<div class="field">
  {#if placeholder && value !== ""}
    <span transition:fly={{ duration: 50, y: 20 }}>
      {placeholder}
    </span>
  {/if}

  <input bind:value {placeholder} {...rest} />

  {#if error}
    <p class="error">{error}</p>
  {/if}
</div>

<style>
  .field {
    position: relative;
    display: inline-block;
  }

  span {
    position: absolute;
    top: -0.5rem;
    left: 0.7rem;
    font-size: 0.75rem;
    color: #888;
    text-transform: uppercase;
    font-weight: 500;
  }

  input {
    padding: 0.6rem 0.8rem;
    border: 1px solid #ccc;
    font: inherit;
    background-color: transparent;
    border-radius: 0;
    border: none;
    border-bottom: 1px solid #ccc;
  }

  input:focus {
    outline: none;
    border-color: #646cff;
  }

  .error {
    color: crimson;
    font-size: 0.8rem;
  }

  @media (prefers-color-scheme: dark) {
    input {
      border-color: #666;
    }
  }
</style>
