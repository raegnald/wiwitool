<script lang="ts">
  import DropZone from "./DropZone.svelte";

  export let move: (id: string) => void;
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

  <h2>Option 2: Import an exisinting pack config</h2>

  <DropZone handler={(filename) => console.log(filename)}>
    <p>Drag and drop an existing configuration</p>
    <small><i>Dummy drop zone</i></small>
  </DropZone>
</div>

<style>
</style>
