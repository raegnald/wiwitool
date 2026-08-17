<script lang="ts">
  import { getContext, onMount, onDestroy } from "svelte";
  import { TABS_KEY } from "./key.js";
  import type { TabInfo, TabsContext } from "./types";
  import type { Component } from "svelte";

  export let title: string;
  export let id = title;
  export let hidden: boolean = false;
  export let icon: Component | undefined = undefined;
  export let activeBackground: string | undefined = undefined;
  export let activeBackgroundDark: string | undefined = undefined;

  const { registerTab, unregisterTab, selectedTab, selectTab } = getContext(
    TABS_KEY,
  ) as TabsContext;

  const tab: TabInfo = {
    id,
    title,
    icon,
    hidden,
    activeBackground,
    activeBackgroundDark,
  };

  onMount(() => {
    registerTab(tab);
  });

  onDestroy(() => {
    unregisterTab(tab);
  });
</script>

{#if $selectedTab === id}
  <div class="tab-pane">
    <slot move={selectTab} />
  </div>
{/if}

<style>
  .tab-pane {
    height: 100%;
    /*display: flex;
    flex-direction: column;
    flex-wrap: nowrap;
    justify-content: center;*/
  }
</style>
