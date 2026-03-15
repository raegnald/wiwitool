<script lang="ts">
  import DropZone from "../components/DropZone.svelte";
  import type { MainModule } from "../bindings/wiwitool";
  import { INFO, toast, toasts } from "../stores/toastsStore";
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
  import Carousel from "../components/Carousel.svelte";

  export let move: (id: string) => void;
  export let module: MainModule;

  $: showDropZone = false;

  async function importPacks(file: File) {
    if (!module || !$workspace) return;

    toast(INFO, "Importing configuration...");

    const buffer = await file.arrayBuffer();
    const uint8View = new Uint8Array(buffer);

    const vec = new module.PaintingBufferVector();
    for (let i = 0; i < uint8View.length; i++) {
      vec.push_back(uint8View[i]);
    }

    $workspace.deserialise(vec);
    vec.delete();

    retrievePaintings();
    retrieveMusicDiscs();

    toasts.set([]);
    toast(INFO, "Configuration was imported");
    move("paintings");
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
      loading={!$wasmReady}
      icon="BadgePlus"
      onclick={() => move("paintings")}
    >
      Create a new pack
    </Button>

    <Button
      loading={!$wasmReady}
      icon="FileUp"
      onclick={() => (showDropZone = true)}
    >
      Import an existing pack
    </Button>
  </div>
  <div class:showDropZone class="dropZoneContainer">
    <DropZone handler={importPacks}>
      <span>Drag and drop an existing configuration file</span><br />
      <small>
        (its default name is <code>Wiwitool Project Save file</code>)
      </small>
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
  You can load and manipulate images, turning them into beautiful Minecraft
  paintings.
{/snippet}

{#snippet feature2()}
  Turning your songs into Minecraft disc items has been easier! Just drag and
  drop your audio files, and they will be turned into music discs you can craft
  with a stonecutter.
{/snippet}

{#snippet feature3()}
  Want to display something on a wall but don't like how the item frame looks?
  We don't like it either! With the Invisible Item Frames pack you can craft,
  ehem-ehem, invisible item frames!
{/snippet}

{#snippet feature4()}
  Open-source software (<a
    href="https://github.com/raegnald/wiwitool/"
    target="_blank">Github repository</a
  >). Free to use forever.
  <br /><br />
  Runs locally on your browser (all your images are kept on <i>your</i> machine.)
{/snippet}

<style>
  .dropZoneContainer {
    display: none;
  }

  .dropZoneContainer.showDropZone {
    display: block;
    padding-top: 20px;
  }

  #center {
    display: flex;
    gap: 10px;
    justify-content: center;
  }

  h2 {
    margin-bottom: 0;
    border-bottom: 1px solid #ccc;
    font-weight: normal;
  }

  @media (prefers-color-scheme: dark) {
    h2 {
      border-color: #666;
    }
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

  .features .imgs {
    display: flex;
    gap: 10px;
    justify-content: space-around;
    align-items: center;
    margin: 20px 0;
  }

  .features .imgs img {
    width: 100%;
  }
</style>
