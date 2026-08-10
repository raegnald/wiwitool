<script lang="ts">
  import { onMount } from "svelte";
  import { workspace } from "./stores/workspaceStore";

  import Toasts from "./components/Toasts.svelte";
  import DynamicTitleBar from "./components/title-bar/DynamicTitleBar.svelte";

  import Tabs from "./components/tabs-component/Tabs.svelte";
  import Tab from "./components/tabs-component/Tab.svelte";

  import PaintingsTab from "./tabs/PaintingsTab.svelte";
  import MiscTab from "./tabs/MiscTab.svelte";
  import GenerateTab from "./tabs/GenerateTab.svelte";
  import StartTab from "./tabs/StartTab.svelte";

  import { paintingsStore } from "./stores/paintingsStore";
  import ChangelogTab from "./tabs/ChangelogTab.svelte";
  import MusicDiscsTab from "./tabs/MusicDiscsTab.svelte";
  import { musicDiscsStore } from "./stores/musicDiscsStore";

  import { backendType, initialiseBackend } from "./services/backendProvider";

  let mainContentScrolled = $state(false);

  function handleMainContentScroll(event: Event) {
    const element = event.currentTarget as HTMLDivElement;
    mainContentScrolled = element.scrollTop > 0;
  }

  async function resetApp() {
    paintingsStore.set([]);
    musicDiscsStore.set([]);
    if ($workspace) {
      await $workspace.dispose();
    }
    $workspace = await initialiseBackend();
  }

  onMount(async () => {
    try {
      const activeWs = await initialiseBackend();
      $workspace = activeWs;
    } catch (e) {
      console.error("[App] Backend initialisation failed:", e);
    }
  });
</script>

<div class="app-container">
  {#if $backendType === "native"}
    <DynamicTitleBar {mainContentScrolled} />
  {/if}

  <div class="main-scroll-content" onscroll={handleMainContentScroll}>
    <div id="wiwi-logo-wide">
      <img src="/logo-wide.svg" alt="Wide logo" />
    </div>

    <Tabs {resetApp}>
      <Tab hidden title="Start" id="start" let:move>
        <StartTab {move} />
      </Tab>

      <Tab hidden title="Changelog" id="changelog" let:move>
        <ChangelogTab {move} />
      </Tab>

      <Tab title="Paintings" id="paintings" let:move>
        <PaintingsTab {move} />
      </Tab>

      <Tab title="Music discs" id="discs" let:move>
        <MusicDiscsTab {move} />
      </Tab>

      <Tab title="Miscellaneous" id="misc" let:move>
        <MiscTab {move} />
      </Tab>

      <Tab title="Generate" id="generate" let:move>
        <GenerateTab {move} />
      </Tab>
    </Tabs>

    <Toasts />
  </div>
</div>

<style>
  .app-container {
    display: flex;
    flex-direction: column;
    height: 100vh;
    width: 100vw;
    overflow: hidden;
  }

  .main-scroll-content {
    flex: 1;
    overflow-y: auto;
    overflow-x: hidden;
  }

  #wiwi-logo-wide {
    display: flex;
    justify-content: center;
    padding: 0 10px;
  }

  #wiwi-logo-wide img {
    margin: 50px 0;
    width: 100%;
    max-width: 768px;
  }

  @media (prefers-color-scheme: dark) {
    #wiwi-logo-wide img {
      content: url("/logo-wide-dark.svg");
    }
  }
</style>
