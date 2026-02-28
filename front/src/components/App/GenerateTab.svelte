<script lang="ts">
  import { paintingsStore } from "../../stores/paintingsStore";
  import {
    type MainModule,
    type MinecraftPacker,
  } from "../../bindings/wiwitool";
  import { workspace } from "../../stores/workspaceStore";
  import Button from "../Button.svelte";

  export let module: MainModule;

  function generatePacks() {
    if (!$workspace) return;

    const zipPath = $workspace.generateZip();
    download(zipPath);
  }

  function addPaintingsPack(packer: MinecraftPacker) {
    const paintingsPack = new module.PaintingsPack();
    const paintingVector = new module.PaintingsVector();

    $paintingsStore.forEach((wrapper) => {
      paintingVector.push_back(wrapper.cppPainting);
    });

    paintingsPack.setPaintings(paintingVector);

    packer.add(paintingsPack);

    // paintingsPack.delete();
    // paintingVector.delete();
  }

  function addInvisibleItemFramesPack(packer: MinecraftPacker) {
    let invisibleItemFramesPack = new module.InvisibleItemFramePack();
    packer.add(invisibleItemFramesPack);
    // invisibleItemFramesPack.delete();
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
  <p>Your pack will contain:</p>
  <ul>
    <li>
      {$paintingsStore.length > 0 ? $paintingsStore.length : "No"}
      painting{$paintingsStore.length != 1 ? "s" : ""}
    </li>

    {#if $workspace && $workspace.invisibleItemFrames}
      <li>Invisible item frames (normal and glow variants)</li>
    {/if}
  </ul>

  <Button large onclick={generatePacks} icon="Download">Download pack</Button>
</div>
