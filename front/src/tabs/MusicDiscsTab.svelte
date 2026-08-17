<script lang="ts">
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

  import { fileService } from "../services/backendProvider";
  import Wiwinput from "../components/Wiwinput.svelte";

  export let move: (id: string) => void;

  let musicDiscsNamespaceOverride: string = "";
  let namespaceOverrideDialog: HTMLDialogElement;

  let showingInfo = false;

  $: allSelected = $musicDiscsStore.every((musicDisc) => musicDisc.selected);
  $: selectedCount = $musicDiscsStore.filter((disc) => disc.selected).length;

  onMount(async () => {
    if ($workspace) {
      const ns = await $workspace.getMusicDiscsNamespaceOverride();
      if (ns) musicDiscsNamespaceOverride = await ns.get();
    }
  });

  async function processAudioTags(fileOrBlob: Blob, disc: any) {
    try {
      const metadata = await parseBlob(fileOrBlob);
      if (metadata?.common) {
        if (metadata.common.title) {
          await disc.setTitle(metadata.common.title);
        }
        if (metadata.common.artist) {
          await disc.setArtist(metadata.common.artist);
        }
        if (metadata.common.picture && metadata.common.picture.length > 0) {
          const pic = metadata.common.picture[0];
          const picBytes = new Uint8Array(pic.data);
          if (typeof disc.setOriginalCoverFromBytes === "function") {
            await disc.setOriginalCoverFromBytes(picBytes);
          } else if (typeof disc.setCoverFromBytes === "function") {
            await disc.setCoverFromBytes(picBytes);
          }
        }
      }
    } catch (err) {
      console.warn("Could not parse embedded audio tags/artwork:", err);
    }
  }

  async function openPicker() {
    if (!$workspace) return;
    try {
      const picked = await $fileService.openAudioFile();
      if (!picked) return;
      const disc = await $workspace.addMusicDisc(picked.path || picked.data);
      if (picked.name) await disc.setTitle(picked.name.split(".")[0]);

      if (picked.data && picked.data.length > 0) {
        const blob = new Blob([picked.data as BlobPart]);
        await processAudioTags(blob, disc);
      } else if (picked.path) {
        try {
          const res = await fetch(picked.path);
          const blob = await res.blob();
          await processAudioTags(blob, disc);
        } catch (e) {}
      }

      musicDiscsStore.update((current) => [
        ...current,
        { disc, selected: false },
      ]);
    } catch (e) {
      console.error(e);
    }
  }

  async function handleDrop(file: File) {
    if (!$workspace) return;
    try {
      const input = (file as any).path || file;
      const disc = await $workspace.addMusicDisc(input);
      if (file.name) await disc.setTitle(file.name.split(".")[0]);

      await processAudioTags(file, disc);

      musicDiscsStore.update((current) => [
        ...current,
        { disc, selected: false },
      ]);
    } catch (e) {
      console.error(e);
    }
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
      <Wiwinput
        type="text"
        placeholder="Music discs namespace override"
        bind:value={musicDiscsNamespaceOverride}
        oninput={() => {
          musicDiscsNamespaceOverride = formatWorkspaceName(
            musicDiscsNamespaceOverride,
          );
        }}
        onchange={() =>
          $workspace
            ?.getMusicDiscsNamespaceOverride()
            .then((ns) => ns?.set(musicDiscsNamespaceOverride))}
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
            ?.getMusicDiscsNamespaceOverride()
            .then((ns) => ns?.set(musicDiscsNamespaceOverride));
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
          <Button icon="FolderInput" onclick={openPicker}>
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

    {#each [...$musicDiscsStore].reverse() as wrapper ((wrapper.disc || wrapper.disc).stringId())}
      <MusicDiscCard {wrapper} />
    {/each}
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
