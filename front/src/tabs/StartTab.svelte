<script lang="ts">
  import DropZone from "../components/DropZone.svelte";
  import { ERROR, INFO, toast, toasts } from "../stores/toastsStore";
  import { workspace } from "../stores/workspaceStore";
  import {
    paintingsStore,
    type PaintingWrapper,
  } from "../stores/paintingsStore";
  import Button from "../components/Button.svelte";
  import { backendReady, fileService } from "../services/backendProvider";
  import {
    musicDiscsStore,
    type MusicDiscWrapper,
  } from "../stores/musicDiscsStore";
  import Carousel from "../components/Carousel.svelte";
  import {
    ArchiveRestore,
    CpuIcon,
    Disc2Icon,
    GithubIcon,
    ImageIcon,
    WandSparklesIcon,
  } from "@lucide/svelte";
  import { slide } from "svelte/transition";

  export let move: (id: string) => void;

  $: showDropZone = false;

  async function selectFileFromPicker() {
    try {
      let picked: { name: string; data: Uint8Array; path?: string } | null =
        null;
      if ($fileService && typeof $fileService.openProjectFile === "function") {
        picked = await $fileService.openProjectFile();
      }
      if (!picked) return;
      await processImportBytes(picked.data);
    } catch (e: any) {
      console.error("Import failed:", e);
      toast(ERROR, e.message || "Error during import", 8000);
    }
  }

  async function processImportBytes(uint8View: Uint8Array) {
    if (!$workspace) return;
    toast(INFO, "Importing configuration...");

    if ($workspace?.deserialise) {
      await $workspace.deserialise(uint8View);
    }

    await retrievePaintings();
    await retrieveMusicDiscs();

    toasts.set([]);
    toast(INFO, `${$workspace.workspaceName ?? "Workspace"} was imported`);
    move("paintings");
  }

  async function importPacks(file: File) {
    try {
      const buffer = await file.arrayBuffer();
      await processImportBytes(new Uint8Array(buffer));
    } catch (e: any) {
      console.error("Deserialisation failed:", e);
      toast(ERROR, e.message || "Error during import", 8000);
    }
  }

  async function retrievePaintings() {
    if (!$workspace) return;
    const paintingsList = await $workspace.getPaintings();
    const newWrappers: PaintingWrapper[] = paintingsList.map((p) => ({
      painting: p,
      selected: false,
    }));

    paintingsStore.set(newWrappers);
  }

  async function retrieveMusicDiscs() {
    if (!$workspace) return;
    const discsList = await $workspace.getMusicDiscs();
    const newWrappers: MusicDiscWrapper[] = discsList.map((d) => ({
      disc: d,
      selected: false,
    }));

    musicDiscsStore.set(newWrappers);
  }
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
      loading={!$backendReady}
      icon="BadgePlus"
      onclick={() => move("paintings")}
    >
      Create a new pack
    </Button>

    <Button
      large
      loading={!$backendReady}
      icon="FileUp"
      onclick={() => (showDropZone = !showDropZone)}
    >
      Import an existing pack
    </Button>
  </div>

  {#if showDropZone}
    <div class="dropZoneContainer" transition:slide={{ duration: 100 }}>
      <DropZone handler={importPacks} let:filePicker>
        <div class="empty-state">
          <ArchiveRestore size="80" strokeWidth="1.5" class="empty-icon" />

          <h2>Drag and drop an existing configuration file</h2>
          <center class="with-title pills-horizontal-container">
            <span class="pill">Wiwitool Project Save file</span>
          </center>

          <div class="empty-actions">
            <Button icon="FolderInput" onclick={selectFileFromPicker}>
              Select from file picker
            </Button>
          </div>
        </div>
      </DropZone>
    </div>
  {/if}

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
