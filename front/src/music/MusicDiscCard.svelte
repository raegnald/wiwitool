<script lang="ts">
  import { onMount, onDestroy } from "svelte";
  import type { MainModule } from "../bindings/wiwitool";
  import {
    musicDiscsStore,
    type MusicDiscWrapper,
  } from "../stores/musicDiscsStore";

  import SelectableCard from "../components/SelectableCard.svelte";
  import Button from "../components/Button.svelte";
  import DropZone from "../components/DropZone.svelte";
  import WaveformPlayer from "./WaveformPlayer.svelte";
  import { MicVocal, AudioLines, NotepadText } from "@lucide/svelte";
  import { workspace } from "../stores/workspaceStore";

  export let module: MainModule;
  export let wrapper: MusicDiscWrapper;

  let title = "";
  let artist = "";

  let descriptionMode: boolean = false; // false: title/author; true: description

  // Data for the player
  let duration = 0;
  let trimStart = 0;
  let trimEnd = 0;
  let audioSrc = "";
  let waveformData: Float32Array;

  let coverImgSrc = "";

  let deleteDiscDialog: HTMLDialogElement;

  function syncMetadataToCpp() {
    wrapper.cppDisc.title = title;
    wrapper.cppDisc.artist = artist;
  }

  function handleTrimChange(start: number, end: number) {
    trimStart = start;
    trimEnd = end;
    wrapper.cppDisc.setTrim(trimStart, trimEnd);
  }

  onMount(() => {
    title = wrapper.cppDisc.title;
    artist = wrapper.cppDisc.artist;
    duration = wrapper.cppDisc.getDurationSeconds();

    trimStart = wrapper.cppDisc.getTrimStart();
    if (trimStart < 0) trimStart = 0;

    trimEnd = wrapper.cppDisc.getTrimEnd();
    if (trimEnd < 0) trimEnd = duration;

    // Setup Audio Player
    const oggBytes = wrapper.cppDisc.getOggAudioData();
    if (oggBytes.length > 0) {
      const blob = new Blob([oggBytes], { type: "audio/ogg" });
      audioSrc = URL.createObjectURL(blob);
    }

    // Extract Waveform
    const waveView = wrapper.cppDisc.getWaveform();
    waveformData = new Float32Array(waveView);

    // Extract Cover Art
    const coverImage = wrapper.cppDisc.getDiscItemImage();
    if (coverImage && !coverImage.empty()) {
      const pngBytes = coverImage.encodePng();
      const blob = new Blob([pngBytes], { type: "image/png" });
      coverImgSrc = URL.createObjectURL(blob);
      coverImage.delete();
    }
  });

  onDestroy(() => {
    if (audioSrc) URL.revokeObjectURL(audioSrc);
    if (coverImgSrc) URL.revokeObjectURL(coverImgSrc);
  });

  function removeCoverImage() {
    wrapper.cppDisc.removeCover();
    coverImgSrc = "";
    wrapper = wrapper;
  }

  async function loadCover(file: File) {
    const buffer = await file.arrayBuffer();
    const uint8View = new Uint8Array(buffer);

    wrapper.cppDisc.setCoverFromBytes(uint8View);

    const coverImage = wrapper.cppDisc.getDiscItemImage();
    if (coverImage && !coverImage.empty()) {
      const pngBytes = coverImage.encodePng();
      const blob = new Blob([pngBytes], { type: "image/png" });

      if (coverImgSrc) URL.revokeObjectURL(coverImgSrc);
      coverImgSrc = URL.createObjectURL(blob);
      coverImage.delete();
    }

    wrapper = wrapper;
  }

  function remove() {
    const idToRemove = wrapper.cppDisc.stringId();

    $workspace.removeMusicDisc(idToRemove);

    musicDiscsStore.update((list) => list.filter((p) => p !== wrapper));

    wrapper.cppDisc.delete();
  }
</script>

<dialog bind:this={deleteDiscDialog}>
  <div class="modal-content">
    <span>
      Are you sure you want to remove this music disc? <br />
      All the customizations you've made to it will be lost.
    </span>

    <div class="modal-actions">
      <Button transparent onclick={() => deleteDiscDialog.close()}>
        Cancel
      </Button>

      <Button
        destructive
        onclick={() => {
          deleteDiscDialog.close();
          remove();
        }}
      >
        Remove painting
      </Button>
    </div>
  </div>
</dialog>

<SelectableCard bind:selected={wrapper.selected}>
  <div class="disc-card">
    <div class="controls-container">
      {#if audioSrc && waveformData}
        <WaveformPlayer
          {audioSrc}
          {waveformData}
          {duration}
          discId={wrapper.cppDisc.stringId()}
          {trimStart}
          {trimEnd}
          onTrimChange={handleTrimChange}
        />
      {/if}

      <div class="metadata-and-delete">
        <Button
          destructive
          icon="Trash2"
          onclick={() => deleteDiscDialog.showModal()}
        />

        <div class="metadata">
          {#if descriptionMode}
            <label style:flex={1}>
              <NotepadText />
              <span>Description </span>
              <input
                type="text"
                bind:value={title}
                style="width: 100%; margin-left: 20px"
                oninput={() => {
                  artist = "";
                  syncMetadataToCpp();
                }}
              />
            </label>
          {:else}
            <label>
              <AudioLines />
              <span>Title </span>
              <input
                type="text"
                bind:value={title}
                oninput={syncMetadataToCpp}
              />
            </label>

            <label>
              <MicVocal />
              <span>Artist </span>
              <input
                type="text"
                bind:value={artist}
                oninput={syncMetadataToCpp}
                placeholder="(optional)"
              />
            </label>
          {/if}

          <Button
            onclick={() => {
              descriptionMode = !descriptionMode;

              if (descriptionMode) {
                title =
                  artist && title ? artist + " - " + title : title ? title : "";
                artist = "";
                syncMetadataToCpp();
              }
            }}
            icon={descriptionMode ? "MicVocal" : "NotepadText"}
            title="Toggle description type"
          />
        </div>
      </div>
    </div>

    <div class="cover-container">
      <DropZone
        style={`
          padding: 0;
          height: 100%;
          margin: 0;
          font-size: 80%;
          ${coverImgSrc ? "background-color: transparent;" : ""}
          padding-left: ${coverImgSrc ? 8 : 0}px;
        `}
        handler={loadCover}
      >
        {#if coverImgSrc}
          <img src={coverImgSrc} class="cover" alt="Disc Cover" />
        {:else}
          Drag and drop a cover image
        {/if}
      </DropZone>

      <div class="cover-actions">
        <Button
          small
          destructive
          disabled={!coverImgSrc}
          onclick={removeCoverImage}
          icon="X"
          title="Remove cover"
        />
      </div>
    </div>
  </div>
</SelectableCard>

<style>
  .disc-card {
    display: flex;
    gap: 15px;
    width: 100%;
    align-items: top;
  }

  .cover-container {
    width: 160px;
    height: 160px;
    flex-shrink: 0;
    display: flex;
    align-items: center;
    justify-content: center;
    position: relative;
    margin-left: 20px;
    user-select: none;
  }

  .cover-container .cover-actions {
    position: absolute;
    top: -12px;
    left: -12px;
  }

  img.cover {
    width: 100%;
    height: 100%;
    object-fit: contain;
    image-rendering: pixelated;
    border-radius: 8px;
  }

  .controls-container {
    display: flex;
    flex-direction: column;
    gap: 10px;
    flex-grow: 1;
    width: 100%;
  }

  .metadata-and-delete {
    display: flex;
    gap: 15px;
  }

  .metadata {
    flex: 1;
    display: flex;
    justify-content: flex-end;
    display: flex;
    gap: 10px;
  }

  .metadata label {
    display: flex;
    align-items: center;
    gap: 10px;
  }

  .metadata label span {
    min-width: 50px;
  }

  .metadata label input {
    width: 200px;
  }
</style>
