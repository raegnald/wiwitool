<script lang="ts">
  import { setContext, onMount } from "svelte";
  import { writable, type Writable } from "svelte/store";
  import { TABS_KEY } from "../../key.js";
  import type { TabInfo } from "./types";
  import { DownloadIcon, HouseIcon } from "@lucide/svelte";

  // Stores for state management
  const selectedTab = writable(null);
  const tabs: Writable<TabInfo[]> = writable([]);

  let homeDialog: HTMLDialogElement;

  // Context object to share with children
  const context = {
    selectedTab,
    // Function for children to register themselves
    registerTab: (tab: TabInfo) => {
      tabs.update((current) => [...current, tab]);

      // Select the first tab automatically if none is selected
      if ($selectedTab === null) {
        $selectedTab = tab.id;
      }
    },
    // Function for children to remove themselves
    unregisterTab: (tab: TabInfo) => {
      tabs.update((current) => current.filter((t) => t.id !== tab.id));
    },
    // The main function to switch tabs
    selectTab: (id: string) => {
      selectedTab.set(id);
    },
  };

  setContext(TABS_KEY, context);
</script>

<dialog bind:this={homeDialog}>
  <div class="modal-content">
    <span
      >Are you sure you want to go back to the start?<br /> You will lose all of your
      progress.</span
    >

    <div class="modal-actions">
      <button class="secondary" onclick={() => homeDialog.close()}>
        Cancel
      </button>
      <button
        class="primary"
        onclick={() => {
          homeDialog.close();
          context.selectTab("start");
        }}
      >
        Start over
      </button>
    </div>
  </div>
</dialog>

<div class="tabs-container">
  {#if $selectedTab != "start"}
    <div class="tabs-header">
      <button
        class:active={$selectedTab === "start"}
        class="icon-tab tab-button"
        onclick={() => homeDialog.showModal()}
      >
        <HouseIcon size="1.2em" />
      </button>

      <div class="normal-tabs-container">
        {#each $tabs as tab}
          {#if !tab.hidden}
            <button
              class="tab-button"
              class:active={$selectedTab === tab.id}
              onclick={() => context.selectTab(tab.id)}
            >
              {tab.title}
            </button>
          {/if}
        {/each}
      </div>

      <button
        class:active={$selectedTab === "generate"}
        class="icon-tab tab-button"
        onclick={() => context.selectTab("generate")}
      >
        <DownloadIcon size="1.2em" />
      </button>
    </div>
  {/if}

  <div class="tabs-content">
    <slot />
  </div>
</div>

<style>
  .tabs-header {
    position: sticky;
    top: 0;
    background-color: #ffff80;
    display: flex;
    justify-content: center;
    /*align-items: center;*/
    border-bottom: 1px solid #ccc;
    margin-bottom: 1rem;
  }
  .normal-tabs-container {
    flex: 1;
    display: flex;
    justify-content: center;
  }
  .tab-button {
    padding: 10px 20px;
    background: transparent;
    border: none;
    cursor: pointer;
    border-radius: 0;
    border-bottom: 2px solid transparent;
    opacity: 0.8;
  }
  .tab-button.active {
    /*font-weight: bold;*/
    border-color: #646cff;
    opacity: 1;
  }
  @media (prefers-color-scheme: dark) {
    .tabs-header {
      border-bottom: 1px solid #666;
      background-color: #242424;
    }
  }
</style>
