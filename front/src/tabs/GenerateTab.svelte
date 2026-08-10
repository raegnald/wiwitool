<script lang="ts">
  import { paintingsStore } from "../stores/paintingsStore";
  import { formatWorkspaceName, workspace } from "../stores/workspaceStore";
  import Button from "../components/Button.svelte";
  import { musicDiscsStore } from "../stores/musicDiscsStore";
  import { ImageUpIcon, PackageOpenIcon } from "@lucide/svelte";
  import { onDestroy, onMount } from "svelte";
  import DropZone from "../components/DropZone.svelte";

  import { fileService } from "../services/backendProvider";
  import Wiwinput from "../components/Wiwinput.svelte";
  import { prettyRatio } from "../paintings/ratios";

  export let move: (id: string) => void;

  let workspaceName: string = "";
  let workspaceIconSrc: string = "";

  let firstTimePack: boolean = true;
  let generating = false;

  $: if (workspaceName && $workspace) {
    const formatted = formatWorkspaceName(workspaceName);
    if (formatted !== workspaceName) {
      workspaceName = formatted;
    }
    $workspace.setWorkspaceName(workspaceName);
  }

  $: validGeneratableConfig =
    $paintingsStore.length > 0 ||
    $musicDiscsStore.length > 0 ||
    ($workspace && $workspace.invisibleItemFrames);

  let iifNamespace: string = "";

  async function updateWorkspaceIcon() {
    if (!$workspace) return;
    try {
      workspaceIconSrc = (await $workspace.getWorkspaceImageUrl()) || "";
    } catch (e) {
      console.error("Failed to load workspace icon:", e);
    }
  }

  async function updateIIFNamespace() {
    if (!$workspace) return;
    try {
      const override = await $workspace.getIIFNamespaceOverride();
      if (override) {
        iifNamespace = (await override.get()) || "";
      }
    } catch (e) {
      console.error("Failed to load IIF namespace:", e);
    }
  }

  onMount(async () => {
    if ($workspace) {
      const currentName =
        typeof ($workspace as any).getWorkspaceName === "function"
          ? await ($workspace as any).getWorkspaceName()
          : $workspace.workspaceName;
      workspaceName = currentName || "";

      if (typeof ($workspace as any).isWorkspaceNew === "function") {
        firstTimePack = await ($workspace as any).isWorkspaceNew();
      }

      await updateWorkspaceIcon();
      await updateIIFNamespace();
    }
  });

  onDestroy(() => {
    if (workspaceIconSrc && workspaceIconSrc.startsWith("blob:")) {
      URL.revokeObjectURL(workspaceIconSrc);
    }
  });

  async function generatePacks() {
    generating = true;
    try {
      if ($workspace) {
        if (workspaceName) await $workspace.setWorkspaceName(workspaceName);
        const zipDataOrPath = await $workspace.generateZip();
        await $fileService.savePackFile(
          ($workspace.workspaceName || "wiwitool_pack") + ".zip",
          zipDataOrPath,
        );
        firstTimePack = false;
      }
    } catch (e: any) {
      console.error("Error generating packs:", e);
    } finally {
      generating = false;
    }
  }
</script>

{#if validGeneratableConfig}
  <div class="app-card project-details">
    <div style="flex: 1">
      <div style="flex: 1">
        <Wiwinput
          type="text"
          placeholder="Project name"
          value={workspaceName}
          disabled={!firstTimePack}
        />

        <div style="margin-top: 20px">
          {#if firstTimePack}
            <span>
              The name of the pack is permanent. Once set, every item from this
              pack that you place in your world will have the project name as
              part of its identifier. Changing it afterwards would break your
              existing items. Choose wisely as it can't be changed later.
            </span>
          {:else}
            <div>
              <p>
                The name of your pack should not be changed after the first
                export.
              </p>
              <Button small secondary onclick={() => (firstTimePack = true)}>
                I know what I'm doing
              </Button>
            </div>
          {/if}
        </div>
      </div>

      <div style="margin-top: 20px">
        {#if workspaceIconSrc}
          <Button
            transparent
            small
            destructive
            onclick={async () => {
              if ($workspace) {
                await $workspace.resetWorkspaceImage();
                await updateWorkspaceIcon();
              }
            }}
          >
            Reset project icon
          </Button>
        {/if}
      </div>
    </div>

    <div>
      <DropZone
        style={`
        width: 256px;
        height: 256px;
        padding: 0;
        margin: 0;
        display: flex;
        justify-content: center;
        align-items: center;
        font-size:120%;
        ${workspaceIconSrc ? "background-color: transparent;" : ""}
        overflow: hidden;
      `}
        handler={async (file) => {
          if ($workspace) {
            const input = (file as any).path || file;
            await $workspace.setWorkspaceImage(input);
            await updateWorkspaceIcon();
          }
        }}
        let:filePicker
      >
        {#if workspaceIconSrc}
          <img
            src={workspaceIconSrc}
            class="workspace-icon"
            alt="Workspace icon"
            style={`
              width: 100%;
              height: 100%;
            `}
          />
        {:else}
          <div>
            <ImageUpIcon
              size="90"
              strokeWidth="1.5"
              class="empty-icon"
              style="margin-bottom: 10px;"
            />

            <center>
              <Button icon="FolderInput" onclick={filePicker}>
                Select pack icon
              </Button>
            </center>
          </div>
        {/if}
      </DropZone>
    </div>
  </div>
{/if}

<div class="app-card">
  {#if validGeneratableConfig}
    <h3 style="margin-top: 0">Summary of your pack</h3>

    <p>Your pack will contain these new items:</p>

    <ul class="pack-contents">
      {#if $paintingsStore.length > 0}
        <li class="pack-content-one">
          <span class="bold-section">
            <b>
              {$paintingsStore.length}
              {$paintingsStore.length != 1 ? "paintings" : "painting"}
            </b>
          </span>
          <ul>
            {#each [...$paintingsStore].reverse() as wrapper (wrapper.painting.stringId())}
              {@const p = wrapper.painting}
              <li class="centered">
                {#await p.getIconUrl() then iconUrl}
                  {#if iconUrl}
                    <img class="item-image" src={iconUrl} alt={p.title} />
                  {/if}
                {/await}
                <span>
                  {#if p.author}
                    {p.author} &ndash;
                  {/if}
                  {p.title}
                </span>
                {#await p.getRatios() then ratios}
                  <div class="pill-container">
                    {#each ratios as ratio, index}
                      <span class="pill">{prettyRatio(ratio)}</span>
                    {/each}
                  </div>
                {/await}
              </li>
            {/each}

            <li style="margin-top: 15px">
              <Button
                secondary
                small
                icon="Image"
                onclick={() => move("paintings")}
              >
                Add more paintings
              </Button>
            </li>
          </ul>
        </li>
      {/if}

      {#if $musicDiscsStore.length > 0}
        <li class="pack-content-one">
          <span class="bold-section">
            <b>
              {$musicDiscsStore.length}
              music disc{$musicDiscsStore.length != 1 ? "s" : ""}
            </b>
          </span>
          <ul>
            {#each [...$musicDiscsStore].reverse() as wrapper (wrapper.disc.stringId())}
              {@const d = wrapper.disc}
              <li class="centered">
                {#await d.getCoverUrl() then coverUrl}
                  {#if coverUrl}
                    <img class="item-image" src={coverUrl} alt={d.title} />
                  {/if}
                {/await}
                <span>
                  {#if d.artist}
                    {d.artist} &ndash;
                  {/if}
                  {d.title}
                </span>
              </li>
            {/each}

            <li style="margin-top: 15px">
              <Button
                secondary
                small
                icon="Disc3"
                onclick={() => move("discs")}
              >
                Add more discs
              </Button>
            </li>
          </ul>
        </li>
      {/if}

      {#if $workspace && $workspace.invisibleItemFrames}
        <li class="pack-content-one">
          <span class="bold-section">
            <b>Invisible item frames</b>
            {#if iifNamespace}
              <br />
              <span>{iifNamespace}</span>
            {/if}
          </span>
          <ul>
            <li class="centered">
              <img
                class="item-image"
                src="/item-frame.png"
                alt="Painting icon"
              />
              Invisible item frame
            </li>

            <li class="centered">
              <img
                class="item-image"
                src="/item-frame-glow.png"
                alt="Painting icon"
              />
              Invisible glowing item frame
            </li>
          </ul>
        </li>
      {/if}
    </ul>
  {:else}
    <div class="empty-state">
      <PackageOpenIcon size="80" strokeWidth="1.5" class="empty-icon" />

      <h2>Your pack is empty!</h2>
      <p>
        It looks like you haven't added any custom content yet. Head over to the
        other tabs to start building your custom Minecraft items.
      </p>

      <div class="empty-actions">
        <Button large icon="Image" onclick={() => move("paintings")}>
          Add paintings
        </Button>
        <Button large icon="Disc3" onclick={() => move("discs")}>
          Add music discs
        </Button>
      </div>
    </div>
  {/if}

  {#if validGeneratableConfig}
    <div class="instructions-box">
      <h3>How to install your custom pack</h3>
      <p>
        When you click download, you will get a file named
        <code>wiwipacks.zip</code>. Unzip this file to find three files inside:
      </p>

      <ol>
        <li>
          <strong><code>respack.zip</code> (Resource Pack):</strong> Move this
          file into your Minecraft <code>resourcepacks</code> folder. You must
          enable it in-game via <em>Options &gt; Resource Packs</em>. This makes
          your custom images and audio visible/audible.
        </li>
        <li>
          <strong><code>datapack.zip</code> (Data Pack):</strong> Move this file
          into the <code>datapacks</code> folder of your specific Minecraft world
          save. Then, exit and re-enter the world. This adds the actual items (paintings,
          discs, recipes) into the world.
        </li>
        <li>
          <strong>
            <code>Wiwitool Project Save file</code> (Project Save):
          </strong>
          Keep this file safe! If you ever want to edit these paintings or add more
          music discs later, simply drag and drop this file into the
          <i>Import an existing pack</i> option located in the start page of the Wiwitool
          to load your workspace back up.
        </li>
      </ol>
    </div>

    <center>
      <Button
        large
        intense
        loading={generating}
        disabled={!validGeneratableConfig}
        onclick={generatePacks}
        icon="Download"
      >
        Download pack
      </Button>
    </center>

    <p style="text-align: center; margin-top: 20px; opacity: 0.8;">
      Thank you for using the Wiwitool!
    </p>
  {/if}
</div>

<style>
  .pill-container {
    display: flex;
    flex-wrap: wrap;
    gap: 6px;
    margin: 0 10px;
    font-size: 85%;
  }

  .project-details {
    margin-bottom: 12px;
    display: flex;
    flex-direction: row;
  }

  h3 {
    margin-bottom: 0;
  }

  ul.pack-contents {
    display: flex;
    flex-direction: column;
    gap: 20px;
    padding-left: 0;
  }

  ul.pack-contents li.pack-content-one {
    width: 100%;
  }

  ul.pack-contents li.pack-content-one ul {
    margin-left: 15px;
    border-left: 2px solid #eee;
    padding-left: 20px;
  }

  @media (prefers-color-scheme: dark) {
    ul.pack-contents li.pack-content-one ul {
      border-left-color: #444;
    }
  }

  .pack-contents li {
    display: flex;
    align-items: top;
  }

  .pack-contents li.centered {
    align-items: center;
  }

  .bold-section {
    min-width: 250px;
    text-align: right;
    padding-top: 12px;
  }

  .item-image {
    width: 48px;
    height: 48px;
    margin-right: 5px;
  }

  .instructions-box {
    margin: 30px 0;
  }
</style>
