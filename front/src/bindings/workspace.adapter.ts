import type {
  IWorkspace,
  IPainting,
  IMusicDisc,
  INamespaceOverride,
} from "../bindings/interfaces";
import type { Wiwiworkspace } from "../bindings/wasm/wiwitool";

import { getWasmModule } from "../stores/wasmStore";

import { toBytes } from "../bytes";
import { callNative, callNativeJson } from "../stores/saucerStore";

import { NativePainting, WasmPainting } from "./painting.adapter";
import { NativeMusicDisc, WasmMusicDisc } from "./musicDisc.adapter";
import {
  NativeNamespaceOverride,
  WasmNamespaceOverride,
} from "./namespaceOverride.adapter";

export class WasmWorkspace implements IWorkspace {
  private cppWorkspace: Wiwiworkspace;
  private paintingsMap: Map<string, WasmPainting> = new Map();
  private musicDiscsMap: Map<string, WasmMusicDisc> = new Map();

  constructor(cppObj: Wiwiworkspace) {
    this.cppWorkspace = cppObj;
  }

  get workspaceName(): string {
    return this.cppWorkspace.workspaceName;
  }
  get invisibleItemFrames(): boolean {
    return this.cppWorkspace.invisibleItemFrames;
  }

  async setWorkspaceName(name: string): Promise<void> {
    this.cppWorkspace.workspaceName = name;
  }
  async setInvisibleItemFrames(enabled: boolean): Promise<void> {
    this.cppWorkspace.invisibleItemFrames = enabled;
  }

  getInvisibleItemFrames(): boolean {
    return this.cppWorkspace.invisibleItemFrames;
  }

  async getWorkspaceImageUrl(): Promise<string> {
    const iconData = this.cppWorkspace.workspaceImage;

    if (!iconData || iconData.empty() || iconData.width === 0) {
      iconData.delete();
      return "";
    }

    const canvas = document.createElement("canvas");
    canvas.width = iconData.width;
    canvas.height = iconData.height;

    const ctx = canvas.getContext("2d");
    if (ctx) {
      const imgData = ctx.createImageData(iconData.width, iconData.height);
      const pixels = iconData.getPixels();
      imgData.data.set(pixels);
      ctx.putImageData(imgData, 0, 0);
    }

    iconData.delete();
    return canvas.toDataURL();
  }

  async setWorkspaceImage(
    fileOrData: File | Uint8Array | string,
  ): Promise<void> {
    let bytes: Uint8Array = await toBytes(fileOrData);
    this.cppWorkspace.setWorkspaceIconFromBytes(bytes);
  }

  async resetWorkspaceImage(): Promise<void> {
    this.cppWorkspace.resetWorkspaceImage();
  }

  async getPaintings(): Promise<IPainting[]> {
    return Array.from(this.paintingsMap.values());
  }

  async addPainting(
    fileOrData: File | Uint8Array | string,
  ): Promise<IPainting> {
    const wasmModule = await getWasmModule();
    let bytes: Uint8Array = await toBytes(fileOrData);

    const cppPainting = new wasmModule.Painting(bytes);
    this.cppWorkspace.addPainting(cppPainting);

    const wasmPainting = new WasmPainting(cppPainting);

    if (fileOrData instanceof File)
      await wasmPainting.setTitle(fileOrData.name.split(".")[0]);

    this.paintingsMap.set(wasmPainting.id, wasmPainting);

    return wasmPainting;
  }

  async removePainting(id: string): Promise<void> {
    this.cppWorkspace.removePainting(id);
    const existing = this.paintingsMap.get(id);
    if (!existing) return;

    await existing.dispose();
    this.paintingsMap.delete(id);
  }

  async getMusicDiscs(): Promise<IMusicDisc[]> {
    return Array.from(this.musicDiscsMap.values());
  }

  async addMusicDisc(
    fileOrData: File | Uint8Array | string,
  ): Promise<IMusicDisc> {
    const wasmModule = await getWasmModule();
    const cppDisc = new wasmModule.MusicDisc();

    let bytes: Uint8Array | null = null;
    let fileName = "";

    if (fileOrData instanceof File) {
      fileName = fileOrData.name.split(".")[0];
      bytes = new Uint8Array(await fileOrData.arrayBuffer());
    } else if (fileOrData instanceof Uint8Array) {
      bytes = fileOrData;
    } else if (typeof fileOrData === "string") {
      fileName = fileOrData;
    }

    if (fileName) {
      cppDisc.title = fileName;
    }

    if (bytes && bytes.length > 0) {
      try {
        const audioContext =
          window.AudioContext || (window as any).webkitAudioContext;
        const audioCtx = new audioContext();

        const decoded = await audioCtx.decodeAudioData(
          bytes.buffer.slice(
            bytes.byteOffset,
            bytes.byteOffset + bytes.byteLength,
          ) as ArrayBuffer,
        );

        const leftChannel = decoded.getChannelData(0);
        const rightChannel =
          decoded.numberOfChannels > 1
            ? decoded.getChannelData(1)
            : new Float32Array(0);

        const leftString = new Uint8Array(
          leftChannel.buffer,
          leftChannel.byteOffset,
          leftChannel.byteLength,
        );
        const rightString = new Uint8Array(
          rightChannel.buffer,
          rightChannel.byteOffset,
          rightChannel.byteLength,
        );

        if (typeof cppDisc.setPcmAudio === "function")
          cppDisc.setPcmAudio(leftString, rightString, decoded.sampleRate);
      } catch (err) {
        console.warn("Could not decode audio data in WASM mode:", err);
      }
    }

    this.cppWorkspace.addMusicDisc(cppDisc);
    const wasmDisc = new WasmMusicDisc(cppDisc);
    this.musicDiscsMap.set(wasmDisc.id, wasmDisc);
    return wasmDisc;
  }

  async removeMusicDisc(id: string): Promise<void> {
    this.cppWorkspace.removeMusicDisc(id);
    const existing = this.musicDiscsMap.get(id);
    if (existing) {
      await existing.dispose();
      this.musicDiscsMap.delete(id);
    }
  }

  async getPaintingsNamespaceOverride(): Promise<INamespaceOverride | null> {
    return new WasmNamespaceOverride(
      this.cppWorkspace.getPaintingsNamespaceOverride(),
    );
  }

  async getMusicDiscsNamespaceOverride(): Promise<INamespaceOverride | null> {
    return new WasmNamespaceOverride(
      this.cppWorkspace.getMusicDiscsNamespaceOverride(),
    );
  }

  async getIIFNamespaceOverride(): Promise<INamespaceOverride | null> {
    return new WasmNamespaceOverride(
      this.cppWorkspace.getIIFNamespaceOverride(),
    );
  }

  isWorkspaceNew(): boolean {
    if (typeof this.cppWorkspace.isWorkspaceNew === "function") {
      return this.cppWorkspace.isWorkspaceNew();
    }
    return true;
  }

  async getReasonableRatios(): Promise<string[]> {
    const wasmModule = await getWasmModule();
    return wasmModule.PaintingRatio.getReasonable();
  }

  async generateZip(): Promise<Uint8Array | string> {
    const zipPath = this.cppWorkspace.generateZip();
    const wasmModule = await getWasmModule();
    if (wasmModule.FS && zipPath) {
      try {
        const fileBytes = wasmModule.FS.readFile(zipPath);
        return new Uint8Array(fileBytes);
      } catch (e) {
        console.error("Failed to read generated zip from WASM FS:", e);
      }
    }
    return zipPath;
  }

  deserialise(data: Uint8Array): void {
    if (typeof this.cppWorkspace.deserialise === "function") {
      this.cppWorkspace.deserialise(data);

      this.paintingsMap.clear();
      const cppPaintings = this.cppWorkspace.getPaintings();
      if (cppPaintings && typeof cppPaintings.size === "function") {
        for (let i = 0; i < cppPaintings.size(); i++) {
          const cppP = cppPaintings.get(i);
          if (cppP) {
            const wasmP = new WasmPainting(cppP);
            this.paintingsMap.set(wasmP.id, wasmP);
          }
        }
        cppPaintings.delete();
      }

      this.musicDiscsMap.clear();
      const cppDiscs = this.cppWorkspace.getMusicDiscs();
      if (cppDiscs && typeof cppDiscs.size === "function") {
        for (let i = 0; i < cppDiscs.size(); i++) {
          const cppD = cppDiscs.get(i);
          if (cppD) {
            const wasmD = new WasmMusicDisc(cppD);
            this.musicDiscsMap.set(wasmD.id, wasmD);
          }
        }
        cppDiscs.delete();
      }
    }
  }

  async dispose(): Promise<void> {
    for (const p of this.paintingsMap.values()) await p.dispose();
    for (const d of this.musicDiscsMap.values()) await d.dispose();

    this.paintingsMap.clear();
    this.musicDiscsMap.clear();

    if (this.cppWorkspace && !this.cppWorkspace.isDeleted()) {
      this.cppWorkspace.delete();
    }
  }
}

export class NativeWorkspace implements IWorkspace {
  public workspaceName: string = "New Workspace";
  public invisibleItemFrames: boolean = false;

  async getWorkspaceName(): Promise<string> {
    const name = await callNative("workspace_get_name");
    if (name) this.workspaceName = name;
    return this.workspaceName;
  }

  async isWorkspaceNew(): Promise<boolean> {
    const res = await callNative("workspace_is_new");
    return typeof res === "boolean" ? res : true;
  }

  async setWorkspaceName(name: string): Promise<void> {
    this.workspaceName = name;
    await callNative("workspace_set_name", [name]);
  }

  async getInvisibleItemFrames(): Promise<boolean> {
    const val = await callNative("workspace_get_invisible_item_frames");
    if (typeof val === "boolean") this.invisibleItemFrames = val;
    return this.invisibleItemFrames;
  }

  async setInvisibleItemFrames(enabled: boolean): Promise<void> {
    this.invisibleItemFrames = enabled;
    await callNative("workspace_set_invisible_item_frames", [enabled]);
  }

  async getWorkspaceImageUrl(): Promise<string> {
    return await callNative("workspace_get_image_url");
  }

  async setWorkspaceImage(
    fileOrData: File | Uint8Array | string,
  ): Promise<void> {
    if (typeof fileOrData === "string") {
      await callNative("workspace_set_image_from_path", [fileOrData]);
    } else {
      let bytes = await toBytes(fileOrData);
      await callNative("workspace_set_image_from_bytes", [Array.from(bytes)]);
    }
  }

  async resetWorkspaceImage(): Promise<void> {
    await callNative("workspace_reset_image");
  }

  async getPaintings(): Promise<IPainting[]> {
    const items = await callNativeJson("workspace_get_paintings");
    return items.map((item: any) => new NativePainting(item.id, item));
  }

  async addPainting(
    fileOrData: File | Uint8Array | string,
  ): Promise<IPainting> {
    let res =
      typeof fileOrData === "string"
        ? await callNativeJson("workspace_add_painting_from_path", [fileOrData])
        : await callNativeJson("workspace_add_painting_from_bytes", [
            Array.from(await toBytes(fileOrData)),
          ]);

    return new NativePainting(res.id, res);
  }

  async removePainting(id: string): Promise<void> {
    await callNative("workspace_remove_painting", [id]);
  }

  async getMusicDiscs(): Promise<IMusicDisc[]> {
    const items = await callNativeJson("workspace_get_music_discs");
    return items.map((item: any) => new NativeMusicDisc(item.id, item));
  }

  async addMusicDisc(
    fileOrData: File | Uint8Array | string,
  ): Promise<IMusicDisc> {
    let res: any;
    let bytes: Uint8Array | null = null;

    if (typeof fileOrData === "string") {
      res = await callNativeJson("workspace_add_music_disc_from_path", [
        fileOrData,
      ]);
    } else {
      bytes = await toBytes(fileOrData);
      res = await callNativeJson("workspace_add_music_disc_from_bytes", [
        Array.from(bytes || []),
      ]);
    }

    const disc = new NativeMusicDisc(res.id, res);

    if (bytes && bytes.length > 0) {
      try {
        const audioCtx = new (
          window.AudioContext || (window as any).webkitAudioContext
        )();
        const decoded = await audioCtx.decodeAudioData(
          bytes.buffer.slice(
            bytes.byteOffset,
            bytes.byteOffset + bytes.byteLength,
          ) as ArrayBuffer,
        );
        const leftChannel = decoded.getChannelData(0);
        const rightChannel =
          decoded.numberOfChannels > 1
            ? decoded.getChannelData(1)
            : new Float32Array(0);

        const leftBytes = Array.from(
          new Uint8Array(
            leftChannel.buffer,
            leftChannel.byteOffset,
            leftChannel.byteLength,
          ),
        );
        const rightBytes = Array.from(
          new Uint8Array(
            rightChannel.buffer,
            rightChannel.byteOffset,
            rightChannel.byteLength,
          ),
        );

        await callNative("disc_set_pcm_audio", [
          disc.id,
          leftBytes,
          rightBytes,
          decoded.sampleRate,
        ]);
      } catch (err) {
        console.warn("Could not decode audio data for native disc:", err);
      }
    }

    return disc;
  }

  async removeMusicDisc(id: string): Promise<void> {
    await callNative("workspace_remove_music_disc", [id]);
  }

  async getPaintingsNamespaceOverride(): Promise<INamespaceOverride | null> {
    return new NativeNamespaceOverride("paintings");
  }

  async getMusicDiscsNamespaceOverride(): Promise<INamespaceOverride | null> {
    return new NativeNamespaceOverride("music_discs");
  }

  async getIIFNamespaceOverride(): Promise<INamespaceOverride | null> {
    return new NativeNamespaceOverride("invisible_item_frames");
  }

  async getReasonableRatios(): Promise<string[]> {
    return await callNativeJson("painting_get_reasonable_ratios");
  }

  async generateZip(): Promise<Uint8Array | string> {
    return await callNative("workspace_generate_zip");
  }

  async deserialise(data: Uint8Array): Promise<void> {
    await callNative("workspace_deserialise", [Array.from(data)]);
    await this.getWorkspaceName();
    await this.getInvisibleItemFrames();
  }

  async dispose(): Promise<void> {
    await callNative("workspace_dispose");
  }
}
