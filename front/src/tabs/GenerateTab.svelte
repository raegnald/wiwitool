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
  import { PackageOpenIcon } from "@lucide/svelte";

  export let module: MainModule;
  export let move: (id: string) => void;

  $: validGeneratableConfig =
    $paintingsStore.length > 0 ||
    $musicDiscsStore.length > 0 ||
    ($workspace && $workspace.invisibleItemFrames);

  function generatePacks() {
    if (!$workspace) return;

    const zipPath = $workspace.generateZip();
    download(zipPath);
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
        <Button large secondary icon="Disc3" onclick={() => move("discs")}>
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
        <code>wiwipacks.zip</code>. Unzip this file to find three important
        files inside:
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
          into the <code>datapacks</code> folder of your specific Minecraft
          world save. Then, type <code>/reload</code> in the game chat. This adds
          the actual items (paintings, discs, recipes) into the world.
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

  li {
    display: flex;
    align-items: top;
  }

  li.centered {
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
    background: rgba(100, 108, 255, 0.05);
    border: 1px solid rgba(100, 108, 255, 0.3);
    border-radius: 8px;
    padding: 20px;
    margin: 30px 0;
  }

  .instructions-box h3 {
    margin-top: 0;
    color: #646cff;
  }

  .instructions-box ol {
    margin-bottom: 0;
    padding-left: 20px;
    display: flex;
    flex-direction: column;
    gap: 12px;
  }

  .instructions-box li {
    display: list-item;
    line-height: 1.5;
  }

  .instructions-box code {
    background: rgba(0, 0, 0, 0.08);
    padding: 2px 6px;
    border-radius: 4px;
    font-size: 0.9em;
  }

  @media (prefers-color-scheme: dark) {
    .instructions-box code {
      background: rgba(255, 255, 255, 0.1);
    }
  }

  .empty-state {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    text-align: center;
    padding: 60px 20px;
    color: #666;
  }

  :global(.empty-icon) {
    color: #aaa;
    margin-bottom: 20px;
  }

  .empty-state h2 {
    margin: 0 0 10px 0;
    color: #333;
    font-weight: 600;
  }

  .empty-state p {
    max-width: 420px;
    margin: 0 0 30px 0;
    line-height: 1.5;
  }

  .empty-actions {
    display: flex;
    gap: 15px;
    flex-wrap: wrap;
    justify-content: center;
  }

  @media (prefers-color-scheme: dark) {
    .empty-state {
      color: #aaa;
    }
    .empty-state h2 {
      color: #eee;
    }
    :global(.empty-icon) {
      color: #555;
    }
  }
</style>
