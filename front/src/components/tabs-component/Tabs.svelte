<script lang="ts">
  import { setContext } from "svelte";
  import { writable, type Writable } from "svelte/store";
  import { TABS_KEY } from "./key.js";
  import type { TabInfo } from "./types";
  import { DownloadIcon, HouseIcon } from "@lucide/svelte";
  import Button from "../Button.svelte";

  export let resetApp: () => void;

  const selectedTab = writable(null);
  const tabs: Writable<TabInfo[]> = writable([]);

  let homeDialog: HTMLDialogElement;

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
    },
  };

  setContext(TABS_KEY, context);
</script>

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

<div class="tabs-container">
  {#if $tabs.find((tab) => tab.id == $selectedTab && !tab.hidden)}
    <div class="tabs-header">
      <div class="blur"></div>

      <div class="tabs-header-container app-width">
        <div class="coloured">
          <button
            class:active={$selectedTab === "start"}
            class="icon-tab"
            onclick={() => homeDialog.showModal()}
          >
            <HouseIcon size="1.2em" />
          </button>
        </div>

        <div class="normal-tabs-container">
          {#each $tabs as tab}
            {#if !tab.hidden && tab.id != "start" && tab.id != "generate"}
              <button
                class:active={$selectedTab === tab.id}
                onclick={() => context.selectTab(tab.id)}
              >
                {tab.title}
              </button>
            {/if}
          {/each}
        </div>

        <div class="coloured">
          <button
            class:active={$selectedTab === "generate"}
            class="icon-tab"
            onclick={() => context.selectTab("generate")}
          >
            <DownloadIcon size="1.2em" />
          </button>
        </div>
      </div>
    </div>
  {/if}

  <div class="tabs-content" id="main-content">
    <slot />
  </div>
</div>

<style>
  .tabs-header {
    position: sticky;
    top: 0;
    z-index: 100;
    /*background-color: #ffff80;*/
    /*border-bottom: 1px solid #dede6f;*/
    padding-top: 10px;
    margin-bottom: 1rem;
  }
  .blur {
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    mask: linear-gradient(black, black, transparent);
    backdrop-filter: blur(5px);
    z-index: 1;
  }
  .tabs-header-container {
    position: relative;
    display: flex;
    justify-content: space-between;
    z-index: 10;
  }
  .normal-tabs-container,
  .coloured {
    margin: 0 10px;
    background-color: white;
    border-radius: 23px;
    border: 1px solid #ccc;
    box-shadow: 0px 2px 6px rgba(0, 0, 0, 0.1);
  }
  .normal-tabs-container {
    display: flex;
    justify-content: center;
  }
  button {
    padding: 10px 20px;
    background: transparent;
    color: #666;
    border: none;
    cursor: pointer;
    border-radius: 0;
    border: 2px solid transparent;
    border-radius: 23px;
  }
  button.active {
    color: hsl(237 100% 55.5%);
    opacity: 1;
    border: 2px solid #646cff;
    background-color: #f7f8ff;
    box-shadow: 0px 2px 5px rgba(0, 0, 0, 0.05);
  }

  @media (prefers-color-scheme: dark) {
    .normal-tabs-container,
    .coloured {
      background-color: #333;
      border-radius: 23px;
      border: 1px solid #666;
    }

    button {
      color: #aaa;
    }

    button.active {
      color: white;
      background-color: #191919;
    }
  }
</style>
