<script lang="ts">
  import type { MainModule } from "../bindings/wiwitool";
  import { workspace } from "../stores/workspaceStore";

  import { musicDiscsStore } from "../stores/musicDiscsStore";

  import { parseBlob } from "music-metadata";

  import Button from "../components/Button.svelte";
  import DropZone from "../components/DropZone.svelte";
  import MusicDiscCard from "../music/MusicDiscCard.svelte";

  export let module: MainModule;
  export let move: (id: string) => void;

  async function handleDrop(file: File) {
    if (!module || !$workspace) return;

    const cppDisc = new module.MusicDisc();

    try {
      const metadata = await parseBlob(file);

      cppDisc.title = metadata.common.title || file.name.split(".")[0];
      cppDisc.artist = metadata.common.artist || "";

      const picture = metadata.common.picture?.[0];
      if (picture && picture.data) {
        const cleanBytes = new Uint8Array(picture.data);
        cppDisc.setCoverFromBytes(cleanBytes);
      }
    } catch (e) {
      console.warn("Could not parse audio metadata:", e);
      cppDisc.title = file.name.split(".")[0];
      cppDisc.artist = "Unknown";
    }

    // Decoding the heavy PCM audio
    const buffer = await file.arrayBuffer();
    let audioContext = new AudioContext();
    let audioBuffer = await audioContext.decodeAudioData(buffer);

    let leftChannel = audioBuffer.getChannelData(0);
    let rightChannel =
      audioBuffer.numberOfChannels > 1 ? audioBuffer.getChannelData(1) : null;

    let leftBytes = new Uint8Array(
      leftChannel.buffer,
      leftChannel.byteOffset,
      leftChannel.byteLength,
    );

    let rightBytes = rightChannel
      ? new Uint8Array(
          rightChannel.buffer,
          rightChannel.byteOffset,
          rightChannel.byteLength,
        )
      : new Uint8Array();

    console.log("Encoding audio to OGG...");
    cppDisc.setPcmAudio(leftBytes, rightBytes, audioBuffer.sampleRate);
    console.log("Encoding complete!");

    $workspace.addMusicDisc(cppDisc);

    musicDiscsStore.update((current) => [
      ...current,
      { cppDisc, selected: false },
    ]);
  }
</script>

<main>
  <div class="title-component">
    <h2>Load your songs</h2>
    <Button icon="Info">Information</Button>
  </div>

  <DropZone handler={handleDrop}>Drag and drop audio files here</DropZone>

  <div>
    {#each [...$musicDiscsStore].reverse() as wrapper (wrapper.cppDisc.stringId())}
      <MusicDiscCard {module} {wrapper} />
    {/each}
  </div>
</main>
