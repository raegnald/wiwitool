<script lang="ts">
  import { paintingsStore } from "../stores/paintingsStore";
  import {
    type ImageData,
    type MainModule,
    type MinecraftPacker,
  } from "../bindings/wiwitool";
  import { workspace } from "../stores/workspaceStore";
  import Button from "../components/Button.svelte";
  import { formatRatio } from "../paintings/ratios";
  import { musicDiscsStore } from "../stores/musicDiscsStore";
  import { formatSeconds } from "../music/seconds";
  import { ImageUpIcon, PackageOpenIcon } from "@lucide/svelte";
  import { onDestroy, onMount } from "svelte";
  import DropZone from "../components/DropZone.svelte";

  export let module: MainModule;
  export let move: (id: string) => void;

  let workspaceName: string = null;
  let workspaceIconSrc: string = "";

  let firstTimePack: boolean = false;
  let generating = false;

  $: if (workspaceName != null) {
    $workspace.workspaceName = workspaceName = workspaceName
      .replace(/[^(\w|\s)]/g, "")
      .replace(/[\s]/g, "_");
  }

  $: validGeneratableConfig =
    $paintingsStore.length > 0 ||
    $musicDiscsStore.length > 0 ||
    ($workspace && $workspace.invisibleItemFrames);

  onMount(() => {
    workspaceName = $workspace.workspaceName;
    firstTimePack = $workspace.isWorkspaceNew();

    updateWorkspaceIcon();
  });

  onDestroy(() => {
    if (workspaceIconSrc) URL.revokeObjectURL(workspaceIconSrc);
  });

  async function generatePacks() {
    generating = true;
    await new Promise((resolve) => setTimeout(resolve, 50));

    $workspace.workspaceName = workspaceName;

    try {
      const zipPath = $workspace.generateZip();
      download(zipPath);
    } catch (e: any) {
      // Check if the error is a C++ memory pointer
      if (typeof e === "number" && module.getExceptionMessage) {
        const [type, message] = module.getExceptionMessage(e);
        console.error(`C++ Exception (${type}): ${message}`);
        // alert(`Failed to generate packs!\nType: ${type}\nMessage: ${message}`);
      } else {
        console.error("Unknown JS Error generating packs:", e);
        // alert("Failed to generate packs! Check console for details.");
      }
    } finally {
      generating = false;
    }
  }

  function updateWorkspaceIcon() {
    const icon = $workspace.workspaceImage;

    if (icon && !icon.empty()) {
      const pngBytes = icon.encodePng();
      const blob = new Blob([pngBytes], { type: "image/png" });

      if (workspaceIconSrc) URL.revokeObjectURL(workspaceIconSrc);
      workspaceIconSrc = URL.createObjectURL(blob);
      icon.delete();
    } else {
      if (workspaceIconSrc) URL.revokeObjectURL(workspaceIconSrc);
      workspaceIconSrc = "";
    }
  }

  async function loadWorkspaceIcon(file: File) {
    const buffer = await file.arrayBuffer();
    const uint8View = new Uint8Array(buffer);

    $workspace.setWorkspaceIconFromBytes(uint8View);
    updateWorkspaceIcon();
  }

  function download(zipPath: string) {
    if (!module) return;

    try {
      const fileContent = module.FS.readFile(zipPath);

      const blob = new Blob([fileContent], { type: "application/zip" });
      const url = URL.createObjectURL(blob);
      const a = document.createElement("a");

      a.href = url;
      a.download = "wiwipacks.zip";
      a.click();

      URL.revokeObjectURL(url);

      module.FS.unlink(zipPath);
    } catch (e) {
      console.error("Error reading generated file:", e);
      alert("Generation finished, but failed to read zip");
    }
  }

  function encodeImage(imgData: ImageData) {
    const blob = new Blob([imgData.encodePng()], { type: "image/png" });
    return URL.createObjectURL(blob);
  }
</script>

{#if validGeneratableConfig}
  <div class="app-card project-details">
    <div
      style={`
        display: flex;
        flex-direction: column;
        height: 100%;
        justify-content: space-between;
      `}
    >
      <div style="flex: 1">
        <span>Project name &nbsp;</span>
        <input
          type="text"
          bind:value={workspaceName}
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
            <div style="display: flex; gap: 10px; align-items: center;">
              <span>
                The pack name should not be changed after your first export.
              </span>
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
            onclick={() => {
              $workspace.resetWorkspaceImage();
              updateWorkspaceIcon();
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
        handler={loadWorkspaceIcon}
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
          <b class="bold-section">
            {$paintingsStore.length}
            painting{$paintingsStore.length != 1 ? "s" : ""}
          </b>
          <ul>
            {#each [...$paintingsStore].reverse() as wrapper (wrapper.cppPainting.stringId())}
              <li class="centered">
                <img
                  class="item-image"
                  src={encodeImage(wrapper.cppPainting.iconData())}
                  alt="Painting icon"
                />
                {#if wrapper.cppPainting.author}
                  {wrapper.cppPainting.author} &ndash;
                {/if}
                {wrapper.cppPainting.title}
                ({formatRatio(wrapper.cppPainting.ratio)} ratio)
              </li>
            {/each}
          </ul>
        </li>
      {/if}

      {#if $musicDiscsStore.length > 0}
        <li class="pack-content-one">
          <b class="bold-section">
            {$musicDiscsStore.length}
            music disc{$musicDiscsStore.length != 1 ? "s" : ""}
          </b>
          <ul>
            {#each [...$musicDiscsStore].reverse() as wrapper (wrapper.cppDisc.stringId())}
              <li class="centered">
                <img
                  class="item-image"
                  src={encodeImage(wrapper.cppDisc.getDiscItemImage())}
                  alt="Painting icon"
                />
                {#if wrapper.cppDisc.artist}
                  {wrapper.cppDisc.artist} &ndash;
                {/if}
                {wrapper.cppDisc.title}
                ({formatSeconds(wrapper.cppDisc.getTrimmedDuration())})
              </li>
            {/each}
          </ul>
        </li>
      {/if}

      {#if $workspace && $workspace.invisibleItemFrames}
        <li class="pack-content-one">
          <b class="bold-section">Invisible item frames</b>
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
          Add Paintings
        </Button>
        <Button large icon="Disc3" onclick={() => move("discs")}>
          Add Music Discs
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
