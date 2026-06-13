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

  export let move: (id: string) => void;

  let iifEnabled = $workspace ? $workspace.invisibleItemFrames : false;

  let iifNamespaceOverride: string = "";
  let namespaceOverrideDialog: HTMLDialogElement;

  onMount(() => {
    iifNamespaceOverride = $workspace.getIIFNamespaceOverride().get();
  });

  $: if ($workspace) {
    $workspace.invisibleItemFrames = iifEnabled;
  }

  let showingInfoIIF = false;
</script>

<dialog bind:this={namespaceOverrideDialog}>
  <div class="modal-content">
    {#if iifNamespaceOverride.length > 0}
      <span>Custom namespace will be used</span>
    {:else}
      <span>Using default namespace</span>
    {/if}

    <div>
      <input
        type="text"
        placeholder="Invisible item frames namespace override"
        bind:value={iifNamespaceOverride}
        on:input={() => {
          iifNamespaceOverride = formatWorkspaceName(iifNamespaceOverride);
        }}
        on:change={() =>
          $workspace.getIIFNamespaceOverride().set(iifNamespaceOverride)}
      />

      <span
        style={"display: inline-block; width: 60px;" +
          (iifNamespaceOverride.length >= MAX_WORKSPACE_NAME_LENGTH
            ? "color: orange"
            : "")}
      >
        {#if iifNamespaceOverride.length > 0}
          <span style="display: inline-block; width: 25px; text-align: right;">
            {iifNamespaceOverride.length}
          </span>
          / {MAX_WORKSPACE_NAME_LENGTH}
        {/if}
      </span>
    </div>

    <div class="modal-actions">
      <Button
        secondary
        onclick={() => {
          iifNamespaceOverride = "";
          $workspace.getIIFNamespaceOverride().set(iifNamespaceOverride);
        }}
        disabled={iifNamespaceOverride == ""}
      >
        Clear
      </Button>

      <Button
        primary
        onclick={() => {
          namespaceOverrideDialog.close();
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
        <Wiwicheckbox bind:checked={iifEnabled}>
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
          onclick={() => namespaceOverrideDialog.showModal()}
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

  <div class="pagination">
    <Button class="secondary" onclick={() => move("discs")} icon="ArrowLeft">
      Previous
    </Button>
    <Button onclick={() => move("generate")} icon="ArrowRight">Next</Button>
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
