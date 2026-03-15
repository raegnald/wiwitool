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

  let showingInfo = false;

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
    <Button onclick={() => (showingInfo = !showingInfo)} icon="Info">
      Information
    </Button>
  </div>

  <div
    class="app-card help"
    style={`display: ${showingInfo ? "block" : "none"}`}
  >
    <p>
      This tool allows you to create <b
        >custom playable music discs in Minecraft out of your own audio files</b
      >. Simply drag and drop the songs you want to add, and this tool will
      bundle them into your
      <code>datapack.zip</code> and <code>respack.zip</code> files.
    </p>

    <table>
      <tbody>
        <tr>
          <td>You have audio files you want to play in a Minecraft jukebox.</td>
          <td></td>
          <td>You customise your tracks and covers with the Wiwitool.</td>
          <td></td>
          <td>The Wiwitool gives you two zip files to import into Minecraft.</td
          >
          <td></td>
          <td>Play your tunes in-game!</td>
        </tr>
        <tr>
          <td>
            <div class="icon-placeholder">🎵</div>
          </td>
          <td>You load <img src="/rightarrow.svg" alt="Arrow" /></td>
          <td
            ><img
              class="fill-space"
              src="/faviconcropped.png"
              alt="App icon"
            /></td
          >
          <td>Wiwitool generates <img src="/rightarrow.svg" alt="Arrow" /></td>
          <td><div class="icon-placeholder">📦</div></td>
          <td
            >Import into Minecraft <img src="/rightarrow.svg" alt="Arrow" /></td
          >
          <td>
            <div class="icon-placeholder">📻</div>
          </td>
        </tr>
        <tr>
          <td><code>song.mp3</code>, <code>track.wav</code>...</td>
          <td></td>
          <td><span class="jb9 wiwitool-text">The Wiwitool</span></td>
          <td></td>
          <td><code>datapack.zip</code> & <code>respack.zip</code></td>
          <td></td>
          <td>Enjoy your music!</td>
        </tr>
      </tbody>
    </table>

    <p>
      Once loaded into this tool, you can <b>trim the audio</b>, edit the
      <b>title and artist</b>, and even
      <b>drag and drop a custom cover image</b> that will be blended onto the record
      item in-game.
    </p>

    <p>
      In Minecraft, your custom music discs can be crafted from a stonecutter.
      Simply place any music disc into the stonecutter to obtain your custom
      tracks.
    </p>

    <center>
      <img
        src="/stonecutter-discs.png"
        alt="Transforming any vanilla disc into a custom music disc"
      />
    </center>
  </div>

  <DropZone handler={handleDrop}>Drag and drop audio files here</DropZone>

  <div>
    {#if $musicDiscsStore.length > 0}
      <div class="title-component" style="margin-top: 2em">
        <h2>Customise your music discs</h2>

        <!-- <Button
        onclick={toggleSelectAll}
        icon={allSelected ? "CircleMinus" : "CircleCheck"}
      >
        {allSelected ? "Deselect" : "Select"} all
      </Button> -->
      </div>
    {/if}

    {#each [...$musicDiscsStore].reverse() as wrapper (wrapper.cppDisc.stringId())}
      <MusicDiscCard {module} {wrapper} />
    {/each}
  </div>

  <div class="pagination">
    <Button
      class="secondary"
      onclick={() => move("paintings")}
      icon="ArrowLeft"
    >
      Previous
    </Button>

    <Button onclick={() => move("misc")} icon="ArrowRight">
      {$musicDiscsStore.length > 0 ? "Next" : "Skip"}
    </Button>
  </div>
</main>

<style>
  .help {
    margin-bottom: 20px;
  }

  .fill-space {
    width: 100%;
    height: 100%;
    object-fit: contain;
    max-height: 64px;
  }
  .icon-placeholder {
    font-size: 48px;
    line-height: 1;
  }
  .wiwitool-text {
    font-size: 150%;
  }

  p:first-child {
    margin-top: 0;
  }

  table {
    margin: 20px 0;
    width: 100%;
  }

  td {
    text-align: center;
    vertical-align: middle;
    padding: 5px;
  }
</style>
