<script>
  import { invisibleItemFramesPack } from "../../stores/invisibleItemFrameStore";
  import { paintingsStore } from "../../stores/paintingsStore";

  import { INFO, toast } from "../../stores/toastsStore";
</script>

<div>
  <p>Extra configs and download packs</p>

  <p>Your pack will contain:</p>
  <ul>
    <li>
      {$paintingsStore.length > 0 ? $paintingsStore.length : "No"}
      painting{$paintingsStore.length != 1 ? "s" : ""}
    </li>

    {#if $invisibleItemFramesPack}
      <li>Invisible item frames (normal and glow variants)</li>
    {/if}
  </ul>

  <button onclick={() => toast(INFO, "TODO: Minecraft_packer")}
    >Generate pack</button
  >
</div>
