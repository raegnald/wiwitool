<script lang="ts">
  import { onMount } from "svelte";

  import type { FFmpeg } from "@ffmpeg/ffmpeg";
  import { fetchFile, toBlobURL } from "@ffmpeg/util";
  import { ffmpegReady, getFfmpegModule } from "../stores/ffmpegStore";

  import Button from "../components/Button.svelte";

  let ffmpeg: FFmpeg;
  let video: HTMLVideoElement;

  onMount(async () => {
    ffmpeg = await getFfmpegModule();
  });

  async function transcode() {
    if (!ffmpegReady) return;

    await ffmpeg.writeFile(
      "input.webm",
      await fetchFile(
        "https://raw.githubusercontent.com/ffmpegwasm/testdata/master/Big_Buck_Bunny_180_10s.webm",
      ),
    );

    await ffmpeg.exec(["-i", "input.webm", "output.mp4"]);

    const data = await ffmpeg.readFile("output.mp4");

    video.src = URL.createObjectURL(
      new Blob([data as any], { type: "video/mp4" }),
    );

    await ffmpeg.deleteFile("input.webm");
    await ffmpeg.deleteFile("output.mp4");
  }
</script>

<div>
  <p>hola</p>

  <video bind:this={video} controls></video>
  <br />

  <Button onclick={transcode} disabled={!ffmpegReady}>
    {ffmpegReady ? "Transcode webm to mp4" : "Loading FFmpeg..."}
  </Button>
</div>
