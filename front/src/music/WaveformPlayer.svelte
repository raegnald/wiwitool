<script lang="ts">
  import { onMount, onDestroy } from "svelte";
  import { activeDiscId } from "../stores/musicDiscsStore";
  import Button from "../components/Button.svelte";
  import { formatSeconds } from "./seconds";

  // --- Props ---
  export let audioSrc: string;
  export let waveformData: Float32Array;
  export let duration: number;
  export let discId: string; // Used to identify this player in the global store

  export let trimStart: number;
  export let trimEnd: number;

  // Callback when the user finishes dragging a trim handle
  export let onTrimChange: (start: number, end: number) => void;

  // --- State ---
  let waveCanvas: HTMLCanvasElement;
  let audioElement: HTMLAudioElement;
  let isPlaying = false;
  let currentTime = 0;
  let unsubscribeActiveDisc: () => void;

  const canvasPadding = 10;
  let draggingMode: "start" | "end" | "scrub" | null = null;

  onMount(() => {
    unsubscribeActiveDisc = activeDiscId.subscribe((currentActiveId) => {
      if (currentActiveId !== discId && audioElement && !audioElement.paused) {
        audioElement.pause();
      }
    });

    requestAnimationFrame(drawWaveform);
  });

  onDestroy(() => {
    if (unsubscribeActiveDisc) unsubscribeActiveDisc();
  });

  // --- AUDIO PLAYBACK LOGIC ---

  function togglePlayback() {
    if (!audioElement) return;
    if (audioElement.paused) {
      if (
        audioElement.currentTime < trimStart ||
        audioElement.currentTime >= trimEnd
      ) {
        audioElement.currentTime = trimStart;
      }
      audioElement.play();
    } else {
      audioElement.pause();
    }
  }

  function resetPlayback() {
    if (!audioElement) return;
    audioElement.currentTime = trimStart;
    audioElement.pause();
  }

  function handleTimeUpdate() {
    if (!audioElement) return;
    if (audioElement.currentTime >= trimEnd && !audioElement.paused) {
      audioElement.pause();
      audioElement.currentTime = trimStart;
    }
    isPlaying = !audioElement.paused;
    drawWaveform();
  }

  // --- CANVAS INTERACTION LOGIC ---

  function getMouseTime(e: MouseEvent) {
    const rect = waveCanvas.getBoundingClientRect();
    const innerWidth = rect.width - canvasPadding * 2;
    let x = e.clientX - rect.left - canvasPadding;
    x = Math.max(0, Math.min(x, innerWidth));
    return (x / innerWidth) * duration;
  }

  function handleMouseDown(e: MouseEvent) {
    const time = getMouseTime(e);
    const grabTolerance = duration * 0.02;

    if (Math.abs(time - trimStart) < grabTolerance) {
      draggingMode = "start";
    } else if (Math.abs(time - trimEnd) < grabTolerance) {
      draggingMode = "end";
    } else {
      draggingMode = "scrub";
      if (audioElement) audioElement.currentTime = time;
      drawWaveform();
    }
  }

  function handleMouseMove(e: MouseEvent) {
    if (!draggingMode) return;

    let time = getMouseTime(e);

    if (draggingMode === "start") {
      trimStart = Math.min(time, trimEnd - 0.5);
    } else if (draggingMode === "end") {
      trimEnd = Math.max(time, trimStart + 0.5);
    } else if (draggingMode === "scrub" && audioElement) {
      audioElement.currentTime = time;
    }
    drawWaveform();
  }

  function handleMouseUp() {
    if (draggingMode === "start" || draggingMode === "end") {
      // Only sync back to the parent component when the user releases the mouse
      onTrimChange(trimStart, trimEnd);
    }
    draggingMode = null;
  }

  function resetTrim() {
    trimStart = 0;
    trimEnd = duration;
    onTrimChange(trimStart, trimEnd);
    drawWaveform();
  }

  function drawWaveform() {
    if (!waveCanvas || !waveformData) return;
    const ctx = waveCanvas.getContext("2d");
    if (!ctx) return;

    const width = waveCanvas.width;
    const height = waveCanvas.height;
    const innerWidth = width - canvasPadding * 2;

    ctx.clearRect(0, 0, width, height);

    ctx.fillStyle = "#646cff";
    const step = waveformData.length / innerWidth;

    for (let x = 0; x < innerWidth; x++) {
      let maxAmp = 0;
      const startIdx = Math.floor(x * step);
      const endIdx = Math.floor((x + 1) * step);

      for (let i = startIdx; i < endIdx && i < waveformData.length; i++) {
        if (waveformData[i] > maxAmp) maxAmp = waveformData[i];
      }

      const barHeight = Math.max(1, maxAmp * height);
      const y = (height - barHeight) / 2;

      ctx.fillRect(canvasPadding + x, y, 1, barHeight);
    }

    const startX = canvasPadding + (trimStart / duration) * innerWidth;
    const endX = canvasPadding + (trimEnd / duration) * innerWidth;

    ctx.fillStyle = "rgba(0, 0, 0, 0.4)";
    ctx.fillRect(0, 0, startX, height);
    ctx.fillRect(endX, 0, width - endX, height);

    ctx.fillStyle = "#ff4444";
    ctx.fillRect(startX - 1, 0, 2, height);
    ctx.fillRect(endX - 1, 0, 2, height);

    if (audioElement) {
      const playheadX =
        canvasPadding + (audioElement.currentTime / duration) * innerWidth;
      ctx.fillStyle = "#ffffff";
      ctx.fillRect(playheadX - 1, 0, 2, height);

      ctx.fillStyle = "#000000";
      ctx.fillRect(playheadX - 2, 0, 1, height);
      ctx.fillRect(playheadX + 1, 0, 1, height);
    }
  }
</script>

<div class="player">
  <div class="waveform-stack">
    <div class="waveform-container">
      <canvas
        bind:this={waveCanvas}
        width="800"
        height="64"
        onmousedown={handleMouseDown}
        onmousemove={handleMouseMove}
        onmouseup={handleMouseUp}
        onmouseleave={handleMouseUp}
      ></canvas>
    </div>

    <div class="player-buttons">
      <span style="width: 150px">
        <span style="display: inline-block; width: 35px">
          {formatSeconds(currentTime - trimStart)}
        </span>
        /
        {formatSeconds(trimEnd - trimStart)}
      </span>

      <span style="flex: 1"></span>

      <Button small secondary icon="SkipBack" onclick={resetPlayback} />
      <Button icon={isPlaying ? "Pause" : "Play"} onclick={togglePlayback} />
      <span style="width: 36px"></span>

      <span style="flex: 1"></span>

      <div style="width: 150px; display: flex; justify-content: flex-end">
        <Button
          small
          destructive={!(trimStart <= 0 && trimEnd >= duration)}
          onclick={resetTrim}
          disabled={trimStart <= 0 && trimEnd >= duration}
          icon="Scissors"
        >
          Reset trim
        </Button>
      </div>
    </div>
  </div>
</div>

<audio
  bind:this={audioElement}
  bind:currentTime
  src={audioSrc}
  ontimeupdate={handleTimeUpdate}
  onended={() => (isPlaying = false)}
  onplay={() => {
    isPlaying = true;
    activeDiscId.set(discId); // claim the active spot
  }}
  onpause={() => (isPlaying = false)}
></audio>

<style>
  .player {
    display: flex;
    align-items: end;
    gap: 10px;
    user-select: none;
  }

  .waveform-stack {
    display: flex;
    flex-direction: column;
    gap: 10px;
    width: 100%;
    padding-left: 64px;
  }

  .waveform-container {
    width: 100%;
    flex-grow: 1;
    height: 60px;
    background: rgba(0, 0, 0, 0.05);
    border-radius: 6px;
    overflow: hidden;
    cursor: col-resize;
  }

  .waveform-container canvas {
    width: 100%;
    height: 100%;
    display: block;
    image-rendering: pixelated;
  }

  .player-buttons {
    display: flex;
    align-items: center;
    gap: 10px;
  }
</style>
