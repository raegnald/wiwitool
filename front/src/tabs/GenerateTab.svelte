<script lang="ts">
  import { paintingsStore } from "../stores/paintingsStore";
  import { type MainModule, type MinecraftPacker } from "../bindings/wiwitool";
  import { workspace } from "../stores/workspaceStore";
  import Button from "../components/Button.svelte";

  export let module: MainModule;

  $: validGeneratableConfig =
    $paintingsStore.length > 0 || $workspace.invisibleItemFrames;

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
</script>

<div>
  {#if validGeneratableConfig}
    <h3>Summary of your pack</h3>
    <ul>
      {#if $paintingsStore.length > 0}
        <li>
          {$paintingsStore.length}
          painting{$paintingsStore.length != 1 ? "s" : ""}
        </li>
      {/if}

      {#if $workspace && $workspace.invisibleItemFrames}
        <li>Invisible item frames (normal and glow variants)</li>
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
  ul {
    margin-top: 8px;
  }
</style>
