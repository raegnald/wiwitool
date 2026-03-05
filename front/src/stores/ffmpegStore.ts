import { writable } from "svelte/store";

import { toast, INFO } from "./toastsStore";

import { FFmpeg } from "@ffmpeg/ffmpeg";
import { toBlobURL } from "@ffmpeg/util";

import ffmpegWorkerUrl from "@ffmpeg/ffmpeg/worker?worker&url";

export let ffmpegReady = writable(false);
let ffmpeg: FFmpeg | null = null;

export async function getFfmpegModule() {
  if (ffmpeg) return ffmpeg;

  const baseURL = "https://cdn.jsdelivr.net/npm/@ffmpeg/core@0.12.10/dist/esm";

  ffmpeg = new FFmpeg();

  ffmpeg?.on("log", ({ message }) => console.log(message));

  await ffmpeg?.load({
    coreURL: await toBlobURL(`${baseURL}/ffmpeg-core.js`, "text/javascript"),
    wasmURL: await toBlobURL(`${baseURL}/ffmpeg-core.wasm`, "application/wasm"),
    classWorkerURL: ffmpegWorkerUrl,
  });

  ffmpegReady.set(true);
  toast(INFO, "FFmpeg loaded");

  return ffmpeg;
}
