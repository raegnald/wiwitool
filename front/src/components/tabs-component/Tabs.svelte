<script lang="ts">
  import { setContext, tick } from "svelte";
  import { writable, type Writable } from "svelte/store";
  import { TABS_KEY } from "./key.js";
  import type { TabInfo } from "./types";
  import { DownloadIcon, HouseIcon, Icon } from "@lucide/svelte";
  import Button from "../Button.svelte";
  import { getSafeTitlebarSpacePx } from "../title-bar/titleBar";
  import { pageTitle } from "../../stores/titleStore.js";

  export let resetApp: () => void;
  export let mainContentScrolled: boolean = false;

  const selectedTab = writable<string | null>(null);
  const tabs: Writable<TabInfo[]> = writable([]);

  let showSidebar = Boolean(window.saucer);

  const PEEK_THRESHOLD = 20;
  const SIDEBAR_HOVER_WIDTH = 75;
  let isSidebarPeek = false;
  let isTabSwitchPeek = false;
  let tabSwitchTimer: ReturnType<typeof setTimeout>;

  function handleMouseMove(e: MouseEvent) {
    if (showSidebar) {
      isSidebarPeek = false;
      return;
    }

    if (e.clientX <= PEEK_THRESHOLD) {
      isSidebarPeek = true;
    } else if (e.clientX > SIDEBAR_HOVER_WIDTH) {
      isSidebarPeek = false;
    }
  }

  let homeDialog: HTMLDialogElement;
  let sidebarRef: HTMLElement;
  let indicatorY = 0;

  // Object shared with children
  const context = {
    selectedTab,

    registerTab: (tab: TabInfo) => {
      tabs.update((current) => [...current, tab]);
      if ($selectedTab === null) $selectedTab = tab.id;
    },

    unregisterTab: (tab: TabInfo) => {
      tabs.update((current) => current.filter((t) => t.id !== tab.id));
    },

    selectTab: (id: string) => {
      selectedTab.set(id);

      const tab = $tabs.find((tab) => tab.id === id);
      $pageTitle = tab?.title && tab.title !== "" ? tab.title : "Wiwitool";

      if (!tab?.hidden && !showSidebar) {
        isTabSwitchPeek = true;
        clearTimeout(tabSwitchTimer);

        tabSwitchTimer = setTimeout(() => {
          isTabSwitchPeek = false;
        }, 500);
      }

      // Scroll content inside the main container to top
      const scrollable = document.querySelector(".content-inner");
      if (scrollable) {
        scrollable.scrollTo({
          top: 0,
          behavior: "smooth",
        });
      }
    },
  };

  setContext(TABS_KEY, context);

  $: normalTabs = $tabs.filter(
    (tab) => !tab.hidden && tab.id !== "start" && tab.id !== "generate",
  );

  // Track layout changes and update the indicator's physical position
  $: updateIndicator($selectedTab, normalTabs);

  let isResizing = false;
  let resizeTimer: ReturnType<typeof setTimeout>;

  function handleResize() {
    isResizing = true;
    updateIndicator($selectedTab, normalTabs);

    // Clear the previous timer
    clearTimeout(resizeTimer);

    // Re-enable transitions 100ms after the user stops resizing the window
    resizeTimer = setTimeout(() => {
      isResizing = false;
    }, 100);
  }

  async function updateIndicator(tabId: string | null, tabsArray: TabInfo[]) {
    if (typeof window === "undefined") return;

    // Wait for Svelte to apply the `.active` class to the new button
    await tick();

    if (!sidebarRef) return;

    // Find the currently active button and get its exact Y position
    const activeBtn = sidebarRef.querySelector(
      ".nav-item.active",
    ) as HTMLElement;
    if (activeBtn) {
      indicatorY = activeBtn.offsetTop;
    }
  }

  function handleMainContentScroll(event: Event) {
    const element = event.currentTarget as HTMLDivElement;
    mainContentScrolled = element.scrollTop > 0;
  }

  let activeBgColour: string | undefined;
  let activeBgColourDark: string | undefined;

  $: if (selectedTab) {
    activeBgColour = $tabs.find(
      (tab) => tab.id == $selectedTab,
    )?.activeBackground;

    activeBgColourDark = $tabs.find(
      (tab) => tab.id == $selectedTab,
    )?.activeBackgroundDark;
  }
</script>

<svelte:window onresize={handleResize} onmousemove={handleMouseMove} />

<dialog bind:this={homeDialog}>
  <div class="modal-content">
    <span>
      Are you sure you want to go back to the start page? <br />
      You will lose all of your progress.
    </span>

    <div class="modal-actions">
      <Button transparent onclick={() => homeDialog.close()}>Cancel</Button>
      <Button
        destructive
        onclick={() => {
          homeDialog.close();
          resetApp();
          context.selectTab("start");
        }}
      >
        Start over
      </Button>
    </div>
  </div>
</dialog>

<div
  class="app-layout"
  style="--safe-titlebar-space: {getSafeTitlebarSpacePx()}px"
>
  <aside
    class="sidebar-nav"
    class:hidden={!showSidebar && !isSidebarPeek && !isTabSwitchPeek}
    bind:this={sidebarRef}
    style="--indicator-y: {indicatorY}px;"
  >
    <!-- transition:slide={{ duration: 100, axis: "x" }} -->
    <!-- The animated shape moved to the root of the sidebar -->
    <div
      class="active-indicator"
      style={(activeBgColour ? `--active-bg-colour: ${activeBgColour};` : "") +
        (activeBgColourDark
          ? `--active-bg-colour-dark: ${activeBgColourDark};`
          : "")}
      class:first-element={$selectedTab === "start"}
      class:last-element={$selectedTab === "generate"}
      class:no-transition={isResizing}
    >
      <!-- Top Concave Corner -->
      <svg class="corner top" viewBox="0 0 20 20">
        <path class="fill-path" d="M 0,20 A 20,20 0 0,0 20,0 L 20,20 Z" />
        <path class="stroke-path" d="M 0,19 A 19,19 0 0,0 19,0" />
      </svg>

      <!-- Bottom Concave Corner -->
      <svg class="corner bottom" viewBox="0 0 20 20">
        <path class="fill-path" d="M 0,0 A 20,20 0 0,1 20,20 L 20,0 Z" />
        <path class="stroke-path" d="M 0,1 A 19,19 0 0,1 19,20" />
      </svg>
    </div>

    <div class="aside-top">
      <button
        class="nav-item"
        class:active={$selectedTab === "start"}
        title="Start"
        onclick={() => homeDialog.showModal()}
      >
        <span class="icon"><HouseIcon size="1.2em" /></span>
      </button>
    </div>

    <div class="nav-wrapper">
      <!-- Normal Dynamic Tabs -->
      {#each normalTabs as tab}
        <button
          class="nav-item"
          class:active={$selectedTab === tab.id}
          title={tab.title}
          onclick={() => context.selectTab(tab.id)}
        >
          <span class="icon">
            {#if tab.icon}
              {@const Icon = tab.icon}
              <Icon size="1.2em" />
            {:else}
              {tab.title.charAt(0).toUpperCase()}
            {/if}
          </span>
        </button>
      {/each}
    </div>

    <!-- Generate Tab -->
    <button
      class="nav-item"
      class:active={$selectedTab === "generate"}
      title="Generate"
      onclick={() => context.selectTab("generate")}
    >
      <span class="icon"><DownloadIcon size="1.2em" /></span>
    </button>
  </aside>

  {#if showSidebar || isSidebarPeek || isTabSwitchPeek}
    <div class="hide-show-sidebar-container">
      <Button
        small
        transparent
        icon={showSidebar ? "ChevronsLeft" : "ChevronsRight"}
        title={showSidebar ? "Hide side bar" : "Show side bar"}
        classes="hide-show-sidebar-btn"
        onclick={() => (showSidebar = !showSidebar)}
      />
    </div>
  {/if}

  <main
    class="main-container"
    id="main-content"
    class:sidebar-shown={showSidebar || isSidebarPeek || isTabSwitchPeek}
    class:start-active={$selectedTab === "start"}
    class:generate-active={$selectedTab === "generate"}
    onscroll={handleMainContentScroll}
  >
    <div class="content-inner">
      <slot />
    </div>
  </main>
</div>

<style>
  :root {
    --layout-bg: #ffff7e;
    --border-color: #6366f1;
    --bg-white: #ffffff;
    --border-width: 2px;

    --tab-width: 50px;
    --tab-height: 50px;
    --sidebar-width: calc(var(--tab-width) + 10px);

    --gap: 16px;
  }

  @media (prefers-color-scheme: dark) {
    :root {
      --layout-bg: #242424;
      --bg-white: #292929;
      --border-color: #484848;
    }
  }

  #main-content {
    position: relative;
  }

  .hide-show-sidebar-container {
    position: absolute;
    top: calc(var(--safe-titlebar-space));
    left: 16px;
    display: flex;
    justify-content: center;
    z-index: 100000;

    & :global(.hide-show-sidebar-btn) {
      /*position: absolute;
    bottom: 0;
    left: 0;*/
    }
  }

  .aside-top {
    display: flex;
    flex-direction: column;
    align-items: flex-end;
    gap: 0px;
  }

  .app-layout {
    display: flex;
    width: 100%;
    height: calc(100vh - var(--safe-titlebar-space));
    margin: 0;
    padding: 0 10px 10px 0;
    box-sizing: border-box;
    background-color: var(--layout-bg);
    overflow: none;
  }

  .sidebar-nav {
    width: var(--sidebar-width);
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    align-items: flex-end;
    padding: 0 0 4px;
    margin-top: 50px;
    position: relative;
    z-index: 2;
    opacity: 1;
    transition:
      100ms margin ease,
      100ms opacity linear;

    &.hidden {
      margin-left: calc(-1 * var(--sidebar-width) + 10px);
      opacity: 0;
    }
  }

  .nav-wrapper {
    display: flex;
    flex-direction: column;
    gap: var(--gap);
  }

  .nav-item {
    width: var(--tab-width);
    height: var(--tab-height);
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 10px;
    padding: 0 16px;

    appearance: none;
    background: transparent;
    border: none;
    font-size: 18px;
    border-radius: 12px;
    cursor: pointer;

    position: relative;
    z-index: 11;
    transition: color 200s ease;
    color: #666;
  }

  .nav-item.active {
    color: var(--border-color);
  }

  .main-container {
    flex: 1;
    background-color: var(--bg-white);
    border: var(--border-width) solid var(--border-color);
    border-radius: 16px;
    position: relative;
    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);

    display: flex;
    flex-direction: column;
    overflow: hidden;

    transition: border-radius 300ms cubic-bezier(0.4, 0, 0.2, 1);
  }

  /*.main-container.sidebar-shown.start-active {
    border-top-left-radius: 0px;
  }*/

  .main-container.sidebar-shown.generate-active {
    border-bottom-left-radius: 0px;
  }

  .content-inner {
    flex: 1;
    overflow-y: auto;
  }

  .active-indicator {
    position: absolute;
    top: 0;
    right: 0;
    z-index: 10;

    width: calc(var(--tab-width) + var(--border-width));
    height: var(--tab-height);
    background: var(--active-bg-colour, var(--bg-white));
    border: var(--border-width) solid var(--border-color);
    border-right: none;
    border-radius: 16px 0 0 16px;

    margin-right: calc(var(--border-width) * -1);
    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
    clip-path: inset(-100px 0px -100px -100px);

    /* Smoothly slide using the exact offsetTop of the active button */
    transform: translateY(var(--indicator-y, 0px));
    transition: transform 200ms cubic-bezier(0.4, 0, 0.2, 1);

    overflow: visible !important;

    @media (prefers-color-scheme: dark) {
      & {
        background: var(--active-bg-colour-dark, var(--bg-white));
      }
    }
  }

  .active-indicator.no-transition {
    transition: none !important;
  }

  .active-indicator .corner {
    position: absolute;
    right: 0;
    width: 20px;
    height: 20px;
    pointer-events: none;
  }

  .active-indicator .corner.top {
    top: -20px;
  }

  .active-indicator .corner.bottom {
    bottom: -20px;
  }

  /*.active-indicator.first-element .corner.top,*/
  .active-indicator.last-element .corner.bottom {
    display: none;
  }

  .active-indicator .fill-path {
    fill: var(--active-bg-colour, var(--bg-white));
    transition: fill 200ms ease;
  }

  @media (prefers-color-scheme: dark) {
    .active-indicator .fill-path {
      fill: var(--active-bg-colour-dark, var(--bg-white));
      transition: fill 200ms ease;
    }
  }

  .active-indicator .stroke-path {
    fill: none;
    stroke: var(--border-color);
    stroke-width: 2;
  }
</style>
