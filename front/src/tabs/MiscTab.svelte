<script lang="ts">
  import {
    formatWorkspaceName,
    MAX_WORKSPACE_NAME_LENGTH,
    workspace,
  } from "../stores/workspaceStore";

  import Button from "../components/Button.svelte";
  import Wiwicheckbox from "../components/Wiwicheckbox.svelte";
  import ButtonListOptions from "../components/button-list/ButtonListOptions.svelte";
  import ListOption from "../components/button-list/ListOption.svelte";
  import { onMount } from "svelte";
  import type { INamespaceOverride } from "../bindings/interfaces";
  import Wiwinput from "../components/Wiwinput.svelte";

  export let move: (id: string) => void;

  let override: INamespaceOverride | null = null;

  let iifEnabled = false;
  let iifNameOverride: string = "";
  let iifNamespaceOverrideDialog: HTMLDialogElement;

  let showingInfoIIF = false;

  onMount(async () => {
    if ($workspace) {
      if (typeof ($workspace as any).getInvisibleItemFrames === "function") {
        iifEnabled = await ($workspace as any).getInvisibleItemFrames();
      } else {
        iifEnabled = $workspace.invisibleItemFrames;
      }
      override = await $workspace.getIIFNamespaceOverride();
    }
  });

  $: override?.get().then((name) => (iifNameOverride = name));
</script>

<dialog bind:this={iifNamespaceOverrideDialog}>
  <div class="modal-content">
    {#if iifNameOverride.length > 0}
      <span>Custom namespace will be used</span>
    {:else}
      <span>Using default namespace</span>
    {/if}

    <div>
      <Wiwinput
        type="text"
        placeholder="Invisible item frames namespace override"
        bind:value={iifNameOverride}
        oninput={() => (iifNameOverride = formatWorkspaceName(iifNameOverride))}
        onchange={() => {
          $workspace
            ?.getIIFNamespaceOverride()
            .then((override) => override?.set(iifNameOverride));
        }}
      />

      <span
        style={"display: inline-block; width: 60px;" +
          (iifNameOverride.length >= MAX_WORKSPACE_NAME_LENGTH
            ? "color: orange"
            : "")}
      >
        {#if iifNameOverride.length > 0}
          <span style="display: inline-block; width: 25px; text-align: right;">
            {iifNameOverride.length}
          </span>
          / {MAX_WORKSPACE_NAME_LENGTH}
        {/if}
      </span>
    </div>

    <div class="modal-actions">
      <Button
        secondary
        onclick={() => {
          iifNameOverride = "";
          $workspace
            ?.getIIFNamespaceOverride()
            .then((override) => override?.set(iifNameOverride));
        }}
        disabled={iifNameOverride === ""}
      >
        Clear
      </Button>

      <Button
        primary
        onclick={() => {
          iifNamespaceOverrideDialog.close();
        }}
      >
        Close
      </Button>
    </div>
  </div>
</dialog>

<div>
  <div class="app-card">
    <div style="display: flex; gap: 8px;">
      <div class="check-option title-component">
        <Wiwicheckbox
          bind:checked={iifEnabled}
          onclick={() => {
            $workspace?.setInvisibleItemFrames(iifEnabled);
          }}
        >
          Invisible item frames can be crafted as recipes
        </Wiwicheckbox>

        <Button
          transparent
          onclick={() => (showingInfoIIF = !showingInfoIIF)}
          icon="Info"
        />
      </div>

      <ButtonListOptions rightAligned icon="Ellipsis" secondary>
        <ListOption
          disabled={!iifEnabled}
          onclick={() => iifNamespaceOverrideDialog.showModal()}
        >
          Set custom namespace
        </ListOption>
      </ButtonListOptions>
    </div>

    <div class="hidden" class:showingInfoIIF>
      <p>
        Adds two simple item recipes that allow you to shapelessly craft
        invisible item frames and glow item frames using glass panes, fermented
        spider eyes and golden carrots.
      </p>

      <img
        src="/invisible-item-frame-recipe.png"
        alt="Recipe to create an invisible item frame"
      />
    </div>
  </div>
</div>

<style>
  .check-option {
    font-size: 120%;
    flex: 1;
  }

  .hidden {
    display: none;
  }

  .hidden.showingInfoIIF {
    display: block;
    margin-top: 10px;
  }
</style>
