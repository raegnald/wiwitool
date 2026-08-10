import type { IFileService } from "../bindings/interfaces";
import { getWasmModule } from "../stores/wasmStore";
import { callNative, callNativeJson } from "../stores/saucerStore";

export class WasmFileService implements IFileService {
  async openImageFile(): Promise<{ name: string; data: Uint8Array } | null> {
    return new Promise((resolve) => {
      const input = document.createElement("input");

      input.type = "file";
      input.accept = "image/*";

      input.onchange = async () => {
        if (input.files && input.files[0]) {
          const file = input.files[0];
          const buffer = await file.arrayBuffer();
          resolve({ name: file.name, data: new Uint8Array(buffer) });
        } else {
          resolve(null);
        }
      };

      input.click();
    });
  }

  async openAudioFile(): Promise<{ name: string; data: Uint8Array } | null> {
    return new Promise((resolve) => {
      const input = document.createElement("input");

      input.type = "file";
      input.accept = "audio/*";

      input.onchange = async () => {
        if (input.files && input.files[0]) {
          const file = input.files[0];
          const buffer = await file.arrayBuffer();
          resolve({ name: file.name, data: new Uint8Array(buffer) });
        } else {
          resolve(null);
        }
      };

      input.click();
    });
  }

  async openProjectFile(): Promise<{ name: string; data: Uint8Array } | null> {
    return new Promise((resolve) => {
      const input = document.createElement("input");

      input.type = "file";
      input.accept = ".zip,application/zip";

      input.onchange = async () => {
        if (input.files && input.files[0]) {
          const file = input.files[0];
          const buffer = await file.arrayBuffer();
          resolve({ name: file.name, data: new Uint8Array(buffer) });
        } else {
          resolve(null);
        }
      };

      input.click();
    });
  }

  async savePackFile(
    defaultName: string,
    data: Uint8Array | string,
  ): Promise<boolean> {
    let bytes: Uint8Array;

    if (typeof data === "string") {
      const wasmModule = await getWasmModule();

      if (wasmModule.FS && data) {
        try {
          bytes = new Uint8Array(wasmModule.FS.readFile(data));
        } catch (e) {
          bytes = new TextEncoder().encode(data);
        }
      } else {
        bytes = new TextEncoder().encode(data);
      }
    } else {
      bytes = data;
    }

    const blob = new Blob([bytes as BlobPart], { type: "application/zip" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");

    a.href = url;
    a.download = defaultName;
    a.click();

    URL.revokeObjectURL(url);
    return true;
  }
}

export class NativeFileService implements IFileService {
  async openImageFile(): Promise<{
    name: string;
    data: Uint8Array;
    path?: string;
  } | null> {
    const res = await callNativeJson("native_open_file_dialog", ["image"]);
    if (!res || !res.path) return null;

    return {
      name: res.name,
      data: new Uint8Array(res.data || []),
      path: res.path,
    };
  }

  async openAudioFile(): Promise<{
    name: string;
    data: Uint8Array;
    path?: string;
  } | null> {
    const res = await callNativeJson("native_open_file_dialog", ["audio"]);
    if (!res || !res.path) return null;

    return {
      name: res.name,
      data: new Uint8Array(res.data || []),
      path: res.path,
    };
  }

  async openProjectFile(): Promise<{
    name: string;
    data: Uint8Array;
    path?: string;
  } | null> {
    const res = await callNativeJson("native_open_file_dialog", ["all"]);
    if (!res || (!res.path && !res.data)) return null;

    return {
      name: res.name,
      data: new Uint8Array(res.data || []),
      path: res.path,
    };
  }

  async savePackFile(
    defaultName: string,
    data: Uint8Array | string,
  ): Promise<boolean> {
    return await callNative("native_save_file_dialog", [defaultName, data]);
  }
}
