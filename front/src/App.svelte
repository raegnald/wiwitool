<script lang="ts">
  import { onMount } from "svelte";
  import { getWasmModule, wasmReady } from "./stores/wasmStore";
  import type { MainModule } from "./bindings/wiwitool";

  import Tabs from "./components/Tabs/Tabs.svelte";
  import Tab from "./components/Tabs/Tab.svelte";

  import PaintingsTab from "./components/App/PaintingsTab.svelte";
  import MiscTab from "./components/App/MiscTab.svelte";

  import Toasts from "./components/Toasts.svelte";
  import { INFO, toast } from "./stores/toastsStore";
  import GenerateTab from "./components/App/GenerateTab.svelte";
  import StartTab from "./components/App/StartTab.svelte";
  import MusicTab from "./components/App/MusicTab.svelte";

  let module: MainModule | null = null;

  onMount(async () => {
    module = await getWasmModule();
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
    <Tabs>
      <Tab hidden title="Start" id="start" let:move>
        <StartTab {module} {move} />
      </Tab>

      <Tab title="Paintings" id="paintings" let:move>
        <PaintingsTab {module} {move} />
      </Tab>

      <Tab title="Miscellaneous" id="misc" let:move>
        <MiscTab {move} />
      </Tab>

      <Tab title="Music (WIP)" id="music" let:move>
        <MusicTab {move} />
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
