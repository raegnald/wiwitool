<script lang="ts">
  import { getContext, onMount, onDestroy } from "svelte";
  import { TABS_KEY } from "../../key.js";
  import type { TabInfo, TabsContext } from "./types";

  export let title: string;
  // Optional: Allow user to set a custom ID, otherwise use title
  export let id = title;

  const { registerTab, unregisterTab, selectedTab, selectTab } = getContext(
    TABS_KEY,
  ) as TabsContext;

  const tab: TabInfo = { id, title };

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
