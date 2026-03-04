<script lang="ts">
  import * as icons from "@lucide/svelte";
  import { onMount } from "svelte";

  let {
    icon = "",

    disabled = false,

    primary = false,
    secondary = false,
    transparent = false,
    destructive = false,

    large = false,
    small = false,

    grow = false,

    hugeBorder = false,

    onclick = (_: MouseEvent) => {},
    children = undefined,
    ...props
  } = $props();

  let Icon = $derived(icons[icon]);

  let buttonType: "primary" | "secondary" | "transparent" | "destructive" =
    $derived(
      destructive
        ? "destructive"
        : transparent
          ? "transparent"
          : secondary
            ? "secondary"
            : "primary",
    );

  let buttonShape: "normal" | "icon" = $derived(
    children == undefined ? "icon" : "normal",
  );

  let buttonSize: "normalSize" | "largeSize" | "smallSize" = $derived(
    large ? "largeSize" : small ? "smallSize" : "normalSize",
  );

  function performAction(e: MouseEvent) {
    if (!disabled) onclick(e);
  }

  function iconSize() {
    if (large) {
      return children ? "22px" : "28px";
    }

    if (small) {
      return children ? "14px" : "16px";
    }

    return children ? "16px" : "24px";
  }
</script>

<button
  class="{buttonType} {buttonShape} {buttonSize}"
  class:grow
  class:hugeBorder
  {disabled}
  onclick={performAction}
  {...props}
>
  {#if children}
    {@render children()}
  {/if}
  {#if Icon}
    <Icon size={iconSize()} />
  {/if}
</button>

<style>
  button {
    display: flex;
    justify-content: center;
    align-items: center;
    gap: 5px;
    white-space: nowrap;

    padding: 0.6em 1.2em;
    cursor: pointer;
    border: 1px solid #ccc;

    transition: 120ms ease-in;
  }

  button:not(disabled):hover {
    border-color: #646cff;
  }
  button:not(disabled):focus,
  button:not(disabled):focus-visible {
    outline: 4px auto -webkit-focus-ring-color;
  }

  @media (prefers-color-scheme: dark) {
    button {
      background-color: #1a1a1a;
      border: 1px solid #777;
    }
  }

  /* Large and small buttons */

  button.largeSize {
    font-size: 120%;
  }

  button.smallSize {
    font-size: 90%;
    padding: 0.5em 0.8em;
  }

  /* Grow buttons */

  button.grow {
    width: 100% !important;
  }

  button:disabled {
    opacity: 0.5;
    z-index: 1;
    cursor: not-allowed;
  }

  /* Primary buttons */

  button.primary {
  }

  /* Huge border for all button types */

  button.hugeBorder {
    border-color: #646cff;
    box-shadow: 0 0 0 2px #646cff inset;
  }

  /* Secondary buttons */

  button.secondary {
    background-color: transparent;
  }

  /* Transparent buttons */

  button.transparent {
    background-color: rgba(127, 127, 127, 0);
    border-color: transparent;
  }

  button:not(disabled).transparent:hover {
    background-color: rgba(127, 127, 127, 0.1);
    border-color: transparent;
  }

  /* Huge border for transparent buttons */

  button.transparent.hugeBorder {
    background-color: rgba(127, 127, 127, 0.05);
    border-color: #646cff;
  }

  button.transparent.hugeBorder:hover {
    border-color: #646cff;
  }

  /* Destructive buttons */

  button.destructive {
    background-color: #ffcccc;
    border: 2px solid #ffaaaa;
  }

  button.destructive:hover {
    border-color: #ff5555;
  }

  @media (prefers-color-scheme: dark) {
    button.destructive {
      background-color: #e21f1f;
      color: #f4d9d9;
      border: 2px solid #f4d9d9;
    }
  }

  button.normal {
  }

  /* Icon buttons */

  button.icon {
    width: 48px;
    height: 48px;
    padding: 10px;
  }

  button.icon.largeSize {
    width: 64px;
    height: 64px;
    padding: 10px;
  }

  button.icon.smallSize {
    width: 36px;
    height: 36px;
    padding: 2px;
  }
</style>
