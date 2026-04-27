<script lang="ts">
  import { onMount } from "svelte";
  import DropZone from "../components/DropZone.svelte";
  import type { MainModule } from "../bindings/wiwitool";
  import { ERROR, INFO, toast, toasts } from "../stores/toastsStore";
  import { workspace } from "../stores/workspaceStore";
  import {
    paintingsStore,
    type PaintingWrapper,
  } from "../stores/paintingsStore";
  import Button from "../components/Button.svelte";
  import { wasmReady } from "../stores/wasmStore";
  import {
    musicDiscsStore,
    type MusicDiscWrapper,
  } from "../stores/musicDiscsStore";
  import { firstTimeUser } from "../stores/firstTimeUserStore";
  import Carousel from "../components/Carousel.svelte";
  import {
    ArchiveRestore,
    CpuIcon,
    Disc2Icon,
    GithubIcon,
    ImageIcon,
    WandSparklesIcon,
  } from "@lucide/svelte";

  export let move: (id: string) => void;
  export let module: MainModule;

  const isInitialVisit = $firstTimeUser;

  $: showDropZone = false;

  async function importPacks(file: File) {
    if (!module || !$workspace) return;
    toast(INFO, "Importing configuration...");

    try {
      const buffer = await file.arrayBuffer();
      const uint8View = new Uint8Array(buffer);

      $workspace.deserialise(uint8View as any);

      retrievePaintings();
      retrieveMusicDiscs();

      toasts.set([]);
      toast(INFO, "Configuration was imported");
      move("paintings");
    } catch (e: any) {
      console.error("Deserialisation failed:", e);
      toast(ERROR, e.message || "Unknown error during import", 8000);
    }
  }

  function retrievePaintings() {
    const cppPaintingsVector = $workspace.getPaintings();
    const newWrappers: PaintingWrapper[] = [];

    for (let i = 0; i < cppPaintingsVector.size(); i++) {
      newWrappers.push({
        cppPainting: cppPaintingsVector.get(i),
        selected: false,
      });
    }

    cppPaintingsVector.delete();

    paintingsStore.set(newWrappers);
  }

  function retrieveMusicDiscs() {
    const cppDiscsVector = $workspace.getMusicDiscs();
    const newWrappers: MusicDiscWrapper[] = [];

    for (let i = 0; i < cppDiscsVector.size(); i++) {
      newWrappers.push({
        cppDisc: cppDiscsVector.get(i),
        selected: false,
      });
    }

    cppDiscsVector.delete();

    musicDiscsStore.set(newWrappers);
  }

  onMount(() => {
    if ($firstTimeUser && !import.meta.env.DEV) firstTimeUser.set(false);
  });
</script>

<div class="app-card">
  <Carousel
    intervalMs={6000}
    items={[
      {
        content: feature1,
        image: "/geese.png",
        alt: "Minecraft painting in game",
      },
      {
        content: feature2,
        image: "/stonecutter-discs.png",
        alt: "Minecraft discs in stonecutter",
      },
      {
        content: feature3,
        image: "/invisible-item-frame-comparison.png",
        alt: "Normal and invisible item frames holding the same music disc",
      },
      { content: feature4 },
    ]}
  />

  <p>
    {#if isInitialVisit}
      Your first time!
    {:else}
      Welcome back!
    {/if}
  </p>

  <p>
    Welcome to <span class="jb9">The Wiwitool</span>! The Wiwitool is a
    mix-and-match Minecraft resource pack and data pack generator that allows
    you to turn your photos into Minecraft paintings, to <i>block out</i> to
    your favourite tunes and choose from other simple
    <abbr title="Quality of Life">QOL</abbr> features. You can either create a new
    pack or import an existing one.
  </p>

  <div id="center">
    <Button
      large
      intense
      loading={!$wasmReady}
      icon="BadgePlus"
      onclick={() => move("paintings")}
    >
      Create a new pack
    </Button>

    <Button
      large
      loading={!$wasmReady}
      icon="FileUp"
      onclick={() => (showDropZone = !showDropZone)}
    >
      Import an existing pack
    </Button>
  </div>
  <div
    class="dropZoneContainer"
    style:display={showDropZone ? "block" : "none"}
  >
    <DropZone handler={importPacks} let:filePicker>
      <div class="empty-state">
        <ArchiveRestore size="80" strokeWidth="1.5" class="empty-icon" />

        <h2>Drag and drop an existing configuration file</h2>
        <center class="with-title pills-horizontal-container">
          <span class="pill">Wiwitool Project Save file</span>
        </center>

        <div class="empty-actions">
          <Button icon="FolderInput" onclick={filePicker}>
            Select from file picker
          </Button>
        </div>
      </div>
    </DropZone>
  </div>

  <div class="version">
    Wiwitool version {__GIT_TAG__} (commit {__GIT_COMMIT__})
    <Button secondary onclick={() => move("changelog")}>Changelog</Button>
  </div>

  <center>
    <a href="https://github.com/raegnald/wiwitool/issues/" target="_blank">
      <Button transparent icon="Github">Suggest a change or feature</Button>
    </a>
  </center>
</div>

{#snippet feature1()}
  You can load and manipulate images, turning them into
  <b>beautiful Minecraft paintings</b>
  <ImageIcon size="1.2em" />.
{/snippet}

{#snippet feature2()}
  Turning your songs into Minecraft disc items <Disc2Icon size="1.2em" /> has never
  been easier! Just drag and drop your audio files, and they will be turned into
  <b>music discs</b> you can craft with a stonecutter.
{/snippet}

{#snippet feature3()}
  Want to display something on a wall but don't like how the item frame looks?
  We don't like it either! With the <b>Invisible Item Frames pack</b>
  <WandSparklesIcon size="1.2em" /> you can craft, ehem-ehem, invisible item frames!
{/snippet}

{#snippet feature4()}
  <b>Open-source software</b> (<a
    href="https://github.com/raegnald/wiwitool/"
    target="_blank"
  >
    Github <GithubIcon size="1.2em" /> repository
  </a>). Free to use forever.
  <br />
  <b>Runs locally on your browser</b>
  <CpuIcon size="1.2em" /> (all your files are always kept on <i>your</i> machine.)
{/snippet}

<style>
  .dropZoneContainer {
    padding-top: 20px;
  }

  #center {
    display: flex;
    gap: 10px;
    justify-content: center;
  }

  .app-card p:first-child {
    margin-top: 0;
  }

  .version {
    margin: 40px 0 20px 0;
    padding-top: 20px;
    border-top: 1px solid rgba(127, 127, 127, 0.5);
    display: flex;
    justify-content: center;
    align-items: center;
    gap: 10px;
  }
</style>
