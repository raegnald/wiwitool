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

  export let module: MainModule;

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
                {wrapper.cppPainting.title} ({formatRatio(
                  wrapper.cppPainting.ratio,
                )} ratio)
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
                {wrapper.cppDisc.title} ({Math.floor(
                  wrapper.cppDisc.getDurationSeconds(),
                )} seconds)
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
    <p>Configure at least one pack to download.</p>
  {/if}

  <Button
    large
    disabled={!validGeneratableConfig}
    onclick={generatePacks}
    icon="Download"
  >
    Download pack
  </Button>
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
</style>
