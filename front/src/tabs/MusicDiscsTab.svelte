<script lang="ts">
  import type { MainModule } from "../bindings/wiwitool";
  import {
    formatWorkspaceName,
    MAX_WORKSPACE_NAME_LENGTH,
    workspace,
  } from "../stores/workspaceStore";

  import { musicDiscsStore } from "../stores/musicDiscsStore";

  import { parseBlob } from "music-metadata";

  import Button from "../components/Button.svelte";
  import DropZone from "../components/DropZone.svelte";
  import MusicDiscCard from "../music/MusicDiscCard.svelte";
  import { FileMusic } from "@lucide/svelte";
  import HelpUsingMusicDiscsTool from "../music/HelpUsingMusicDiscsTool.svelte";
  import BulkMusicDiscsActions from "../music/BulkMusicDiscsActions.svelte";
  import { onMount } from "svelte";
  import ButtonListOptions from "../components/button-list/ButtonListOptions.svelte";
  import ListOption from "../components/button-list/ListOption.svelte";
  import ListSectionTitle from "../components/button-list/ListSectionTitle.svelte";

  export let module: MainModule;
  export let move: (id: string) => void;

  let musicDiscsNamespaceOverride: string = "";
  let namespaceOverrideDialog: HTMLDialogElement;

  let showingInfo = false;

  $: allSelected = $musicDiscsStore.every((musicDisc) => musicDisc.selected);
  $: selectedCount = $musicDiscsStore.filter((disc) => disc.selected).length;

  onMount(() => {
    musicDiscsNamespaceOverride = $workspace
      .getMusicDiscsNamespaceOverride()
      .get();
  });

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
        cppDisc.setOriginalCoverFromBytes(cleanBytes);
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

  function getSupportedAudioFormats() {
    const audio = document.createElement("audio");
    return {
      mp3: audio.canPlayType("audio/mpeg"),
      wav: audio.canPlayType("audio/wav"),
      ogg: audio.canPlayType("audio/ogg; codecs=vorbis"),
      opus: audio.canPlayType("audio/ogg; codecs=opus"),
      aac: audio.canPlayType("audio/aac"),
      m4a: audio.canPlayType("audio/mp4; codecs=mp4a.40.2"),
      webm: audio.canPlayType("audio/webm; codecs=vorbis"),
      flac: audio.canPlayType("audio/flac"),
    };
  }

  function toggleSelectAll() {
    $musicDiscsStore.forEach((disc) => {
      disc.selected = !allSelected;
    });
    musicDiscsStore.update((it) => it);
  }
</script>

<dialog bind:this={namespaceOverrideDialog}>
  <div class="modal-content">
    {#if musicDiscsNamespaceOverride.length > 0}
      <span>Custom namespace will be used</span>
    {:else}
      <span>Using default namespace</span>
    {/if}

    <div>
      <input
        type="text"
        placeholder="Music discs namespace override"
        bind:value={musicDiscsNamespaceOverride}
        on:input={() => {
          musicDiscsNamespaceOverride = formatWorkspaceName(
            musicDiscsNamespaceOverride,
          );
        }}
        on:change={() =>
          $workspace
            .getMusicDiscsNamespaceOverride()
            .set(musicDiscsNamespaceOverride)}
      />

      <span
        style={"display: inline-block; width: 60px;" +
          (musicDiscsNamespaceOverride.length >= MAX_WORKSPACE_NAME_LENGTH
            ? "color: orange"
            : "")}
      >
        {#if musicDiscsNamespaceOverride.length > 0}
          <span style="display: inline-block; width: 25px; text-align: right;">
            {musicDiscsNamespaceOverride.length}
          </span>
          / {MAX_WORKSPACE_NAME_LENGTH}
        {/if}
      </span>
    </div>

    <div class="modal-actions">
      <Button
        secondary
        onclick={() => {
          musicDiscsNamespaceOverride = "";
          $workspace
            .getMusicDiscsNamespaceOverride()
            .set(musicDiscsNamespaceOverride);
        }}
        disabled={musicDiscsNamespaceOverride == ""}
      >
        Clear
      </Button>

      <Button
        primary
        onclick={() => {
          namespaceOverrideDialog.close();
        }}
      >
        Close
      </Button>
    </div>
  </div>
</dialog>

<main>
  <div class="app-card drop-zone-container">
    <DropZone handler={handleDrop} let:filePicker>
      <div class="empty-state">
        <FileMusic size="80" strokeWidth="1.5" class="empty-icon" />

        <h2>Load your songs</h2>
        <center class="with-title pills-horizontal-container">
          {#each Object.entries(getSupportedAudioFormats()) as format}
            {#if format[1] != ""}
              <span class="pill">{format[0]}</span>
            {/if}
          {/each}
        </center>

        <div class="empty-actions">
          <Button icon="FolderInput" onclick={filePicker}>
            Select from file picker
          </Button>
        </div>
      </div>
    </DropZone>

    <center>
      <Button
        secondary
        hugeBorder={showingInfo}
        onclick={() => (showingInfo = !showingInfo)}
        icon="Info"
      >
        Information
      </Button>
    </center>

    <div
      class="instructions-box"
      style={`display: ${showingInfo ? "block" : "none"}`}
    >
      <HelpUsingMusicDiscsTool />
    </div>
  </div>

  <div>
    {#if $musicDiscsStore.length > 0}
      <div class="title-component" style="margin-top: 2em">
        <h2>Customise your music discs</h2>

        <div style="display: flex; align-items: center; gap: 20px">
          {#if selectedCount > 0}
            <span style="font-size: 110%">{selectedCount} selected</span>
          {/if}

          <ButtonListOptions rightAligned icon="Ellipsis">
            <ListOption
              icon={allSelected ? "CircleMinus" : "CircleCheck"}
              onclick={toggleSelectAll}
            >
              {allSelected ? "Deselect" : "Select"} all
            </ListOption>

            <ListSectionTitle>Advanced options</ListSectionTitle>
            <ListOption onclick={() => namespaceOverrideDialog.showModal()}>
              Set custom namespace
            </ListOption>
          </ButtonListOptions>
        </div>
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

  <BulkMusicDiscsActions />
</main>

<style>
  .drop-zone-container {
    display: flex;
    flex-direction: column;
    gap: 20px;
  }
</style>
