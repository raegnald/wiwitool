<script lang="ts">
  import type { SerialisationModel } from "../../serialisation/models";
  import DropZone from "./DropZone.svelte";
  import { deserialise } from "../../serialisation/deserialise";
  import type { MainModule } from "../../bindings/wiwitool";
  import { INFO, toast, toasts } from "../../stores/toastsStore";

  export let module: MainModule;
  export let move: (id: string) => void;

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

<div>
  <details class="app-card help-card">
    <summary><span class="jb9">Wiwitool</span> features</summary>

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
  </details>

  <p>
    Welcome to the Wiwitool. You can either click on other tabs to generate a
    fresh custom pack (option 1) or import an existing pack you have made in the
    past below (option 2).
  </p>

  <h2>Option 1: Start a fresh pack</h2>

  <center>
    <button on:click={() => move("paintings")}>Create a fresh pack</button>
  </center>

  <h2>Option 2: Import an existing pack config</h2>

  <DropZone handler={importPacks}>
    <p>Drag and drop an existing configuration file</p>
    <small>(its default name is <code>import.json</code>)</small>
  </DropZone>
</div>

<style>
</style>
