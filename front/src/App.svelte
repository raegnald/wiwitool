<script lang="ts">
  import { onMount } from "svelte";
  import { getWasmModule, wasmReady } from "./wasm";
  import type { MainModule } from "./bindings/wiwitool";

  import Tabs from "./components/Tabs/Tabs.svelte";
  import Tab from "./components/Tabs/Tab.svelte";

  import PaintingsTab from "./components/App/PaintingsTab.svelte";
  import MiscTab from "./components/App/MiscTab.svelte";

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
    <p>Loading webassembly module...</p>
  {:else}
    <Tabs>
      <Tab title="Paintings" id="paintings" let:move>
        <PaintingsTab {module} {move} />
      </Tab>

      <Tab title="Miscellaneous" id="misc">
        <MiscTab />
      </Tab>

      <Tab title="Generate" id="generate">
        <p>Extra configs and download packs</p>
      </Tab>
    </Tabs>
  {/if}
</main>

<style>
  main {
    margin-bottom: 200px;
  }

  #wiwi-logo-wide {
    display: flex;
    justify-content: center;
    margin-bottom: 25px;
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
