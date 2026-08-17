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

  import {
    House,
    FileClock,
    Image,
    Disc3,
    GripHorizontal,
    Download,
    HouseIcon,
  } from "@lucide/svelte";

  let mainContentScrolled = $state(false);

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
  <DynamicTitleBar {mainContentScrolled} />

  <div class="main-content">
    <Tabs {resetApp} {mainContentScrolled}>
      <Tab hidden title="Start" id="start" icon={House} let:move>
        <StartTab {move} />
      </Tab>

      <Tab hidden title="Changelog" id="changelog" icon={FileClock} let:move>
        <ChangelogTab {move} />
      </Tab>

      <Tab
        title="Paintings"
        id="paintings"
        icon={Image}
        activeBackground="#f7f8ff"
        activeBackgroundDark="#222"
        let:move
      >
        <PaintingsTab {move} />
      </Tab>

      <Tab title="Music discs" id="discs" icon={Disc3} let:move>
        <MusicDiscsTab {move} />
      </Tab>

      <Tab title="Misc" id="misc" icon={GripHorizontal} let:move>
        <MiscTab {move} />
      </Tab>

      <Tab title="Generate" id="generate" icon={Download} let:move>
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

  .main-content {
    flex: 1;
    overflow: hidden;
  }
</style>
