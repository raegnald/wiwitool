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
  <p>
    Welcome to <span class="jb9">The Wiwitool</span>! The Wiwitool is a
    mix-and-match Minecraft resource pack and data pack generator that allows
    you to turn your photos into Minecraft paintings, to <i>block out</i> to your
    favourite tunes (Coming Soon&#8482;) and choose from other simple QOL features.
    You can either create a new pack or import an existing one.
  </p>

  <div id="center">
    <Button disabled={!$wasmReady} onclick={() => move("paintings")}>
      Create a new pack
    </Button>

    <Button disabled={!$wasmReady} onclick={() => (showDropZone = true)}>
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

  <h2><span class="jb9">Wiwitool</span> features</h2>
  <ul>
    <li>
      Open-source software (<a
        href="https://github.com/raegnald/wiwitool/"
        target="_blank">Github repository</a
      >). Free to use forever.
    </li>
    <li>
      Runs locally on your browser (all your images are kept on <i>your</i> machine.)
    </li>
    <li>
      You can manipulate and preview how your image will be seen in Minecraft.
    </li>
  </ul>

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
    margin: 20px 0;
    display: flex;
    justify-content: center;
    align-items: center;
    gap: 10px;
  }
</style>
