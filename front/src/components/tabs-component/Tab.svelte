<script lang="ts">
  import { getContext, onMount, onDestroy } from "svelte";
  import { TABS_KEY } from "./key.js";
  import type { TabInfo, TabsContext } from "./types";

  export let title: string;
  export let id = title;
  export let hidden: boolean = false;

  const { registerTab, unregisterTab, selectedTab, selectTab } = getContext(
    TABS_KEY,
  ) as TabsContext;

  const tab: TabInfo = { id, title, hidden };

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
