<script lang="ts">
  import * as icons from "@lucide/svelte";

  let {
    disabled = false,
    icon = "",
    onclick = (_: MouseEvent) => {},
    destructive = false,
    hovered = false,
    children,
    ...props
  } = $props();

  let Icon = $derived((icons as Record<string, any>)[icon]);

  function clickfun(event: MouseEvent) {
    if (!disabled) onclick(event);
  }
</script>

<button
  type="button"
  class={disabled ? "disabled" : "enabled"}
  class:destructive
  class:hovered
  onclick={clickfun}
  {...props}
>
  {#if children}
    <span>{@render children()}</span>
  {/if}

  {#if Icon}
    <Icon size="16px" class="option-icon" />
  {/if}
</button>

<style>
  button {
    display: flex;
    justify-content: space-between;
    align-items: center;
    gap: 12px;
    width: 100%;
    padding: 0.6em 1.2em;
    background: transparent;
    border: none;
    cursor: pointer;
    text-align: left;
    white-space: nowrap;
    color: #333;
    transition: background-color 100ms ease-in;
  }

  button.destructive {
    color: #e21f1f;
  }

  button.destructive:hover {
    background-color: #e21f1f !important;
    color: white !important;
  }

  @media (prefers-color-scheme: dark) {
    button.destructive {
      background-color: #e21f1f;
      color: #f4d9d9;
    }
  }

  button.disabled {
    color: #999;
    cursor: default;
  }

  button.enabled:hover,
  button.hovered {
    background-color: rgba(100, 108, 255, 0.1);
    color: #646cff;
  }

  :global(.option-icon) {
    opacity: 0.7;
  }
  button:hover :global(.option-icon) {
    opacity: 1;
  }

  @media (prefers-color-scheme: dark) {
    button {
      color: #e0e0e0;
    }
    button:hover {
      background-color: rgba(100, 108, 255, 0.2);
      color: #fff;
    }
  }
</style>
