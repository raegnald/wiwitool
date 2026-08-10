import { writable } from "svelte/store";
import type { IWorkspace, IFileService } from "../bindings/interfaces";
import {
  NativeWorkspace,
} from "../bindings/workspace.adapter";
import {
  NativeFileService,
  WasmFileService,
} from "../bindings/fileService.adapter";
import { WasmWorkspace } from "../bindings/workspace.adapter";
import { getWasmModule } from "../stores/wasmStore";

export type BackendType = "native" | "wasm";

export const backendType = writable<BackendType>("wasm");
export const backendReady = writable<boolean>(false);
export const activeWorkspace = writable<IWorkspace | null>(null);
export const fileService = writable<IFileService>(new WasmFileService());

let currentWorkspace: IWorkspace | null = null;
let currentFileService: IFileService = new WasmFileService();

export async function initialiseBackend(): Promise<IWorkspace> {
  const isSaucer = typeof window !== "undefined" && Boolean(window.saucer);

  if (isSaucer) {
    console.log(
      "[BackendProvider] Detecting Saucer Desktop environment -> Using Native Adapters.",
    );
    backendType.set("native");
    currentWorkspace = new NativeWorkspace();
    currentFileService = new NativeFileService();
  } else {
    console.log(
      "[BackendProvider] Detecting Web Browser environment -> Using WASM Adapters.",
    );
    backendType.set("wasm");
    const wasmModule = await getWasmModule();
    currentWorkspace = new WasmWorkspace(new wasmModule.Wiwiworkspace());
    currentFileService = new WasmFileService();
  }

  activeWorkspace.set(currentWorkspace);
  fileService.set(currentFileService);
  backendReady.set(true);

  if (!currentWorkspace) {
    throw new Error("[BackendProvider] Failed to initialize workspace backend.");
  }

  return currentWorkspace;
}

export function getWorkspace(): IWorkspace {
  if (!currentWorkspace) {
    throw new Error(
      "[BackendProvider] Backend accessed before initialisation. Call initialiseBackend() first.",
    );
  }
  return currentWorkspace;
}

export function getFileService(): IFileService {
  return currentFileService;
}
