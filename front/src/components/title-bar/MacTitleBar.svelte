<script lang="ts">
  import type { TitleBarProps } from "./titleBarProps";

  import { callNative } from "../../stores/saucerStore";

  let props: TitleBarProps = $props();

  let isHovered = $state(false);
  let isFocused = $state(true);

  $effect(() => {
    if (typeof window === "undefined") return;

    isFocused = document.hasFocus();

    const onFocus = () => (isFocused = true);
    const onBlur = () => (isFocused = false);

    window.addEventListener("focus", onFocus);
    window.addEventListener("blur", onBlur);

    return () => {
      window.removeEventListener("focus", onFocus);
      window.removeEventListener("blur", onBlur);
    };
  });

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

<div
  class="mac-titlebar"
  class:inactive={!isFocused}
  class:main-content-scrolled={props.mainContentScrolled}
>
  <!-- svelte-ignore a11y_no_static_element_interactions -->
  <div
    class="mac-traffic-lights"
    onmouseenter={() => (isHovered = true)}
    onmouseleave={() => (isHovered = false)}
  >
    <button
      class="mac-btn close"
      onclick={closeWindow}
      aria-label="Close"
      title="Close"
    >
      {#if isHovered}
        <svg width="6" height="6" viewBox="0 0 6 6">
          <path
            d="M 0.5,0.5 L 5.5,5.5 M 5.5,0.5 L 0.5,5.5"
            stroke="#4c0000"
            stroke-width="1.2"
            stroke-linecap="round"
          />
        </svg>
      {/if}
    </button>

    <button
      class="mac-btn minimize"
      onclick={minimize}
      aria-label="Minimize"
      title="Minimize"
    >
      {#if isHovered}
        <svg width="6" height="2" viewBox="0 0 6 2">
          <rect width="6" height="1.2" y="0.4" fill="#995700" rx="0.6" />
        </svg>
      {/if}
    </button>

    <button
      class="mac-btn maximize"
      onclick={maximize}
      aria-label="Full Screen"
      title="Full Screen"
    >
      {#if isHovered}
        <svg width="6" height="6" viewBox="0 0 6 6">
          <path
            d="M 0.5 2.5 L 2.5 0.5 M 2.5 0.5 H 0.5 M 2.5 0.5 V 2.5 M 5.5 3.5 L 3.5 5.5 M 3.5 5.5 H 5.5 M 3.5 5.5 V 3.5"
            stroke="#006500"
            stroke-width="1"
            stroke-linecap="round"
            stroke-linejoin="round"
          />
        </svg>
      {/if}
    </button>
  </div>

  <!-- svelte-ignore a11y_no_static_element_interactions -->
  <div
    class="mac-title-drag"
    class:main-content-scrolled={props.mainContentScrolled}
    onmousedown={startDrag}
    ondblclick={maximize}
  >
    <img src="/favicon.png" alt="Wiwitool" class="mac-app-icon" />
    <span class="mac-app-title">{props.title || "Wiwitool"}</span>
  </div>

  <div class="mac-spacer"></div>
</div>

<style>
  .mac-titlebar {
    height: 32px;
    flex-shrink: 0;
    background: #242424;
    border-bottom: 1px solid transparent;
    color: #e4e4e7;
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 0 10px;
    user-select: none;
    -webkit-user-select: none;
    cursor: default;
    -webkit-app-region: drag;
    font-family:
      -apple-system, BlinkMacSystemFont, "SF Pro Text", "SF Pro Display",
      "Helvetica Neue", sans-serif;
    font-size: 13px;
    font-weight: normal;
    z-index: 10000;
    transition:
      background 200ms ease,
      border-color 200ms ease;
  }

  .mac-titlebar.main-content-scrolled {
    background-color: white;
    border-bottom: 1px solid rgba(255, 255, 255, 0.08);
  }

  .mac-traffic-lights {
    display: flex;
    align-items: center;
    gap: 8px;
    height: 100%;
    -webkit-app-region: no-drag;
  }

  .mac-btn {
    --mac-btn-size: 14px;

    width: var(--mac-btn-size) !important;
    height: var(--mac-btn-size) !important;
    min-width: var(--mac-btn-size);
    min-height: var(--mac-btn-size);
    max-width: var(--mac-btn-size);
    max-height: var(--mac-btn-size);
    border-radius: 50%;
    border: none;
    padding: 0;
    display: flex;
    align-items: center;
    justify-content: center;
    cursor: default;
    transition:
      transform 0.1s ease,
      filter 0.15s ease;
    box-shadow: inset 0 0 1px rgba(0, 0, 0, 0.3);
  }

  .mac-btn:active {
    transform: scale(0.9);
  }

  .mac-btn.close {
    background-color: #ff5f57;
    border: 0.25px solid rgba(224, 68, 62, 0.6);
  }

  .mac-btn.minimize {
    background-color: #febc2e;
    border: 0.25px solid rgba(222, 161, 35, 0.6);
  }

  .mac-btn.maximize {
    background-color: #28c840;
    border: 0.25px solid rgba(26, 171, 41, 0.6);
  }

  .mac-title-drag {
    display: flex;
    align-items: center;
    justify-content: center;
    margin: 0 12px;
    gap: 7px;
    height: 100%;
    flex-grow: 1;
    opacity: 0;
    transition: opacity 200ms ease;
  }

  .mac-title-drag.main-content-scrolled {
    opacity: 1;
  }

  .mac-app-icon {
    width: 16px;
    height: 16px;
    opacity: 0.9;
  }

  .mac-app-title {
    color: #f4f4f5;
    font-weight: bold;
    letter-spacing: -0.1px;
    opacity: 0.88;
  }

  .mac-spacer {
    width: 60px;
    height: 100%;
  }

  .mac-titlebar.inactive:not(:hover) .mac-btn {
    background-color: #3e3e42;
    border: 0.25px solid rgba(255, 255, 255, 0.08);
  }

  .mac-titlebar.inactive:not(:hover) .mac-app-title {
    opacity: 0.55;
  }

  @media (prefers-color-scheme: light) {
    .mac-titlebar {
      background-color: #ffff80;
      color: #27272a;
    }

    .mac-titlebar.main-content-scrolled {
      border-bottom-color: rgba(0, 0, 0, 0.12);
    }

    .mac-titlebar.main-content-scrolled.inactive:not(:hover) .mac-btn {
      background-color: #dcdcdc;
    }

    .mac-titlebar.inactive:not(:hover) .mac-btn {
      background-color: #dede6f;
      border: 0.25px solid rgba(0, 0, 0, 0.12);
    }

    .mac-app-title {
      color: #18181b;
    }
  }
</style>
