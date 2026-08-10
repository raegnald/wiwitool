<script lang="ts">
  import type { TitleBarProps } from "./titleBarProps";

  import { callNative } from "../../stores/saucerStore";

  let props: TitleBarProps = $props();

  function startDrag(e: MouseEvent) {
    if (e.button === 0) {
      callNative("window_start_drag");
    }
  }

  function minimize() {
    callNative("window_minimize");
  }

  function maximize() {
    callNative("window_maximize");
  }

  function closeWindow() {
    callNative("window_close");
  }
</script>

<div class="titlebar">
  <!-- svelte-ignore a11y_no_static_element_interactions -->
  <div class="titlebar-drag" onmousedown={startDrag} ondblclick={maximize}>
    <img src="/favicon.png" alt="Icon" class="app-icon" />
    <span class="app-title">{props.title || "Wiwitool"}</span>
  </div>
  <div class="titlebar-controls">
    <button
      class="control-btn minimize"
      onclick={minimize}
      aria-label="Minimize"
    >
      <svg width="10" height="1" viewBox="0 0 10 1">
        <rect width="10" height="1" fill="currentColor" />
      </svg>
    </button>
    <button
      class="control-btn maximize"
      onclick={maximize}
      aria-label="Maximize"
    >
      <svg width="10" height="10" viewBox="0 0 10 10">
        <rect
          width="9"
          height="9"
          x="0.5"
          y="0.5"
          fill="none"
          stroke="currentColor"
        />
      </svg>
    </button>
    <button class="control-btn close" onclick={closeWindow} aria-label="Close">
      <svg width="10" height="10" viewBox="0 0 10 10">
        <path
          d="M 1,1 L 9,9 M 9,1 L 1,9"
          stroke="currentColor"
          stroke-width="1.2"
        />
      </svg>
    </button>
  </div>
</div>

<style>
  .titlebar {
    height: 38px;
    flex-shrink: 0;
    background: #18181b;
    color: #e4e4e7;
    display: flex;
    align-items: center;
    justify-content: space-between;
    user-select: none;
    -webkit-user-select: none;
    cursor: default;
    -webkit-app-region: drag;
    font-family:
      system-ui,
      -apple-system,
      BlinkMacSystemFont,
      "Segoe UI",
      Roboto,
      sans-serif;
    font-size: 13px;
    font-weight: 500;
    z-index: 10000;
  }

  .titlebar-drag {
    display: flex;
    align-items: center;
    gap: 8px;
    padding-left: 12px;
    height: 100%;
    flex-grow: 1;
  }

  .app-icon {
    width: 18px;
    height: 18px;
  }

  .app-title {
    opacity: 0.9;
    letter-spacing: 0.3px;
    user-select: none;
    cursor: default;
  }

  .titlebar-controls {
    display: flex;
    height: 100%;
    -webkit-app-region: no-drag;
  }

  .control-btn {
    background: transparent;
    border: none;
    border-radius: 0;
    color: #a1a1aa;
    width: 44px;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
    cursor: pointer;
    transition:
      background-color 0.15s ease,
      color 0.15s ease;
  }

  .control-btn:hover {
    background: #27272a;
    color: #ffffff;
  }

  .control-btn.close:hover {
    background: #ef4444;
    color: #ffffff;
  }
</style>
