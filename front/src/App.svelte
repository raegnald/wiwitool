<script lang="ts">
  import { onMount } from "svelte";
  import { getWasmModule, wasmReady } from "./stores/wasmStore";
  import { workspace } from "./stores/workspaceStore";
  import type { MainModule } from "./bindings/wiwitool";

  import Toasts from "./components/Toasts.svelte";

  import Tabs from "./components/tabs/Tabs.svelte";
  import Tab from "./components/tabs/Tab.svelte";

  import PaintingsTab from "./tabs/PaintingsTab.svelte";
  import MiscTab from "./tabs/MiscTab.svelte";
  import GenerateTab from "./tabs/GenerateTab.svelte";
  import StartTab from "./tabs/StartTab.svelte";
  import ComponentsShowcaseTab from "./tabs/ComponentsShowcaseTab.svelte";

  import { paintingsStore } from "./stores/paintingsStore";

  let module: MainModule | null = null;

  function resetApp() {
    paintingsStore.set([]);
    $workspace.delete();
    $workspace = new module.Wiwiworkspace();
  }

  onMount(async () => {
    module = await getWasmModule();
    $workspace = new module.Wiwiworkspace();
  });
</script>

<main>
  <div id="wiwi-logo-wide">
    <img src="/logo-wide.svg" alt="Wide logo" />
  </div>

  {#if !$wasmReady}
    <center>
      <p>Loading webassembly module...</p>
    </center>
  {:else}
    <Tabs {resetApp}>
      <Tab title="Components showcase" id="dummy">
        <ComponentsShowcaseTab />
      </Tab>

      <Tab hidden title="Start" id="start" let:move>
        <StartTab {module} {move} />
      </Tab>

      <Tab title="Paintings" id="paintings" let:move>
        <PaintingsTab {module} {move} />
      </Tab>

      <Tab title="Miscellaneous" id="misc" let:move>
        <MiscTab {move} />
      </Tab>

      <Tab hidden title="Generate" id="generate">
        <GenerateTab {module} />
      </Tab>
    </Tabs>

    <Toasts />
  {/if}
</main>

<style>
  main {
    margin-bottom: 200px;
  }

  #wiwi-logo-wide {
    display: flex;
    justify-content: center;
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
