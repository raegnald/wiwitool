<script>
  import { paintingsStore } from "../../stores/paintingsStore";
  import { INFO, toast } from "../../stores/toastsStore";
</script>

<div>
  <p>Extra configs and download packs</p>

  <p>Your pack will contain:</p>
  <ul>
    {#if $paintingsStore.length > 0}
      <li>{$paintingsStore.length} paintings</li>
    {/if}

    {#if true}
      <li>Invisible item frames (normal and glow variants)</li>
    {/if}
  </ul>

  <button onclick={() => toast(INFO, "TODO: Minecraft_packer")}
    >Generate pack</button
  >
</div>
