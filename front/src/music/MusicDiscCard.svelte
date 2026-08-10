<script lang="ts">
  import { onMount, onDestroy } from "svelte";
  import {
    musicDiscsStore,
    type MusicDiscWrapper,
  } from "../stores/musicDiscsStore";

  import SelectableCard from "../components/SelectableCard.svelte";
  import Button from "../components/Button.svelte";
  import DropZone from "../components/DropZone.svelte";
  import WaveformPlayer from "./WaveformPlayer.svelte";
  import {
    MicVocal,
    AudioLines,
    Zap,
    Timer,
    ImageUpIcon,
  } from "@lucide/svelte";
  import { workspace } from "../stores/workspaceStore";
  import Wiwicheckbox from "../components/Wiwicheckbox.svelte";
  import Wiwinput from "../components/Wiwinput.svelte";

  export let wrapper: MusicDiscWrapper;

  let selected = false;

  let title = "";
  let artist = "";
  let comparatorOutput: number;
  let silenceGap: number;

  let hasStonecutterRecipe: boolean;
  let isCoverModified = false;

  // Data for the player
  let duration = 0;
  let trimStart = 0;
  let trimEnd = 0;
  let audioSrc = "";
  let waveformData: Float32Array;

  let coverImgSrc = "";

  let deleteDiscDialog: HTMLDialogElement;

  let hasOriginalCover = false;

  $: showMoreOptions = false;

  $: if (wrapper && wrapper.disc) {
    selected = wrapper.selected;
    title = wrapper.disc.title;
    artist = wrapper.disc.artist;
    comparatorOutput = wrapper.disc.comparatorOutput;
    silenceGap = wrapper.disc.silenceGap;
    hasStonecutterRecipe = wrapper.disc.hasStonecutterRecipe;
    updateCoverState();
  }

  async function updateCoverState() {
    if (!wrapper?.disc) return;
    isCoverModified = Boolean(await wrapper.disc.isCoverModified());
    hasOriginalCover = Boolean(await wrapper.disc.hasOriginalCover());
  }

  function syncMetadataToCpp() {
    wrapper.disc.title = title;
    wrapper.disc.artist = artist;
    wrapper.disc.comparatorOutput = comparatorOutput;
    wrapper.disc.silenceGap = silenceGap;
    wrapper.disc.hasStonecutterRecipe = hasStonecutterRecipe;
  }

  async function handleTrimChange(start: number, end: number) {
    trimStart = start;
    trimEnd = end;
    await wrapper.disc.setTrim(trimStart, trimEnd);
  }

  onMount(async () => {
    title = wrapper.disc.title;
    artist = wrapper.disc.artist;
    duration = await wrapper.disc.getDurationSeconds();
    comparatorOutput = wrapper.disc.comparatorOutput;

    const ts = await wrapper.disc.getTrimStart();
    trimStart = ts < 0 ? 0 : ts;

    const te = await wrapper.disc.getTrimEnd();
    trimEnd = te < 0 ? duration : te;

    // Setup Audio Player
    const oggBytes = await wrapper.disc.getOggAudioData();
    if (oggBytes && oggBytes.length > 0) {
      const blob = new Blob([oggBytes as BlobPart], { type: "audio/ogg" });
      audioSrc = URL.createObjectURL(blob);
    }

    // Extract Waveform
    const waveView = await wrapper.disc.getWaveform();
    waveformData =
      waveView instanceof Float32Array
        ? waveView
        : new Float32Array(waveView || []);

    // Extract Cover Art
    await updateCoverImgSrc();
  });

  onDestroy(() => {
    if (audioSrc) URL.revokeObjectURL(audioSrc);
  });

  async function updateCoverImgSrc() {
    coverImgSrc = (await wrapper.disc.getCoverUrl()) || "";
    await updateCoverState();
  }

  async function removeCoverImage() {
    await wrapper.disc.removeCover();
    await updateCoverImgSrc();
    wrapper = wrapper;
  }

  async function restoreOriginalCover() {
    await wrapper.disc.restoreOriginalCover();
    await updateCoverImgSrc();
    wrapper = wrapper;
  }

  async function loadCover(file: File) {
    const buffer = await file.arrayBuffer();
    const uint8View = new Uint8Array(buffer);

    await wrapper.disc.setCoverFromBytes(uint8View);
    await updateCoverImgSrc();

    wrapper = wrapper;
  }

  async function remove() {
    const idToRemove = wrapper.disc.stringId();
    if ($workspace) await $workspace.removeMusicDisc(idToRemove);
    musicDiscsStore.update((list) => list.filter((p) => p !== wrapper));
    if (typeof wrapper.disc.dispose === "function")
      await wrapper.disc.dispose();
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
        Remove disc
      </Button>
    </div>
  </div>
</dialog>

<SelectableCard
  bind:selected
  onToggle={() => {
    wrapper.selected = selected;
    $musicDiscsStore = $musicDiscsStore;
  }}
>
  <div class="disc-card">
    <div class="controls-container">
      {#if audioSrc && waveformData}
        <WaveformPlayer
          {audioSrc}
          {waveformData}
          {duration}
          discId={wrapper.disc.stringId()}
          {trimStart}
          {trimEnd}
          onTrimChange={handleTrimChange}
        />
      {/if}

      <div class="metadata-and-delete">
        <div class="metadata centered" style="margin-left: 63px">
          <label>
            <div style="display:flex; align-items:center; gap: 5px;">
              <AudioLines />
              <span>Title </span>
            </div>
            <Wiwinput
              type="text"
              style="width: 100%"
              bind:value={title}
              oninput={syncMetadataToCpp}
            />
          </label>

          <label>
            <div style="display:flex; align-items:center; gap: 5px;">
              <MicVocal />
              <span>Artist </span>
            </div>
            <Wiwinput
              type="text"
              style="width: 100%"
              bind:value={artist}
              oninput={syncMetadataToCpp}
              placeholder="(optional)"
            />
          </label>
        </div>
      </div>

      <div class="metadata-and-delete">
        <Button
          destructive
          icon="Trash2"
          onclick={() => deleteDiscDialog.showModal()}
        />
        <div class="metadata">
          <label>
            <div style="display:flex; align-items:center; gap: 5px;">
              <Zap />
              <span style="display: block ruby">Comparator output </span>
            </div>
            <Wiwinput
              id="compout-input"
              type="number"
              bind:value={comparatorOutput}
              oninput={syncMetadataToCpp}
              min="1"
              max="15"
            />
          </label>

          <label>
            <div style="display:flex; align-items:center; gap: 5px;">
              <Timer />
              <span style="display: block ruby">
                <abbr title="Seconds of silence added to the end of a song">
                  Silence gap
                </abbr>
              </span>
            </div>
            <Wiwinput
              type="number"
              bind:value={silenceGap}
              oninput={syncMetadataToCpp}
              min="0"
            />
          </label>
        </div>
      </div>

      <Wiwicheckbox
        bind:checked={hasStonecutterRecipe}
        onclick={syncMetadataToCpp}
      >
        Music disc has stonecutter recipe
      </Wiwicheckbox>
    </div>

    <div class="cover-container">
      <DropZone
        style={`
        padding: 0;
        height: 100%;
        margin: 0;
        display: flex;
        justify-content: center;
        align-items: center;
        font-size:120%;
        ${coverImgSrc ? "background-color: transparent;" : ""}
        padding-left: ${coverImgSrc ? 13.875 : 0}px;
      `}
        handler={loadCover}
        let:filePicker
      >
        {#if coverImgSrc}
          <img src={coverImgSrc} class="cover" alt="Disc Cover" />
        {:else}
          <div>
            <ImageUpIcon
              size="90"
              strokeWidth="1.5"
              class="empty-icon"
              style="margin-bottom: 10px;"
            />

            <center>
              <Button icon="FolderInput" onclick={filePicker}>
                Select cover
              </Button>
            </center>
          </div>
        {/if}
      </DropZone>

      <div class="cover-actions">
        <Button
          small
          transparent
          disabled={!coverImgSrc}
          onclick={removeCoverImage}
          icon="X"
          title="Remove cover"
        />

        {#if hasOriginalCover && isCoverModified}
          <Button
            small
            transparent
            onclick={restoreOriginalCover}
            icon="RotateCcw"
            title="Restore original cover"
          />
        {/if}
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
    width: 234px;
    height: 234px;
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
    top: 8px;
    left: 9px;
    display: flex;
    flex-direction: column;
    gap: 5px;
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
    justify-content: space-between;
    display: flex;
    gap: 30px;
  }

  .metadata label {
    display: flex;
    align-items: center;
    gap: 10px;
    flex: 1;
  }

  .centered {
    display: flex;
    align-items: center;
  }
</style>
