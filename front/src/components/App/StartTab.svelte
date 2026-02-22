<script lang="ts">
  import type { SerialisationModel } from "../../serialisation/models";
  import DropZone from "./DropZone.svelte";
  import { deserialise } from "../../serialisation/deserialise";
  import type { MainModule } from "../../bindings/wiwitool";
  import { INFO, toast, toasts } from "../../stores/toastsStore";

  export let module: MainModule;
  export let move: (id: string) => void;

  $: showDropZone = false;

  async function importPacks(file: File) {
    toast(INFO, "Importing configuration...");

    const buffer = await file.arrayBuffer();
    const decoder = new TextDecoder();
    const model: SerialisationModel = JSON.parse(decoder.decode(buffer));

    deserialise(model, module);

    toasts.set([]);
    toast(INFO, "Configuration was imported");
    move("paintings");
  }
</script>

<div class="app-card">
  <p>
    Welcome to <span class="jb9">The Wiwitool</span>! The wiwitool is a
    mix-and-match minecraft resource pack and data pack generator that allows
    you to turn your photos into mineraft painting, to <i>block out</i> to your favourite
    tunes (Coming Soon&#8482;) and choose from other simple QOL features. You can
    either create a new pack or import an existing one.
  </p>

  <div id="center">
    <button on:click={() => move("paintings")}>Create a new pack</button>

    <button on:click={() => (showDropZone = true)}
      >Import an existing pack</button
    >
  </div>
  <div class:showDropZone class="dropZoneContainer">
    <DropZone handler={importPacks}>
      <span>Drag and drop an existing configuration file</span><br />
      <small>(its default name is <code>import.json</code>)</small>
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
</style>
