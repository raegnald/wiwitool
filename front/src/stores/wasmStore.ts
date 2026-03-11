import { writable } from "svelte/store";

import type { MainModule } from "../bindings/wiwitool";
import type MainModuleFactory from "../bindings/wiwitool";

import { toast, ERROR } from "./toastsStore";

export const wasmReady = writable(false);
let wasmModule: MainModule | null = null;

declare global {
  interface Window {
    createWiwitoolModule: typeof MainModuleFactory;
  }
}

export async function getWasmModule(): Promise<MainModule> {
  if (wasmModule) return wasmModule;

  // First time loading the webassembly module script
  if (!window.createWiwitoolModule) {
    await new Promise((resolve, reject) => {
      const script = document.createElement("script");
      script.src = "/wasm/wiwitool.js";
      script.onload = resolve;
      script.onerror = reject;
      document.body.appendChild(script);
    });
  }

  // Initialise the module with configuration
  wasmModule = await window.createWiwitoolModule({
    locateFile: (path: string) => {
      if (path.endsWith(".wasm") || path.endsWith(".data")) {
        return "/wasm/" + path;
      }
      console.log("Locate file ", path);
      return path;
    },
    print: (text: string) => {
      console.log("[WASM]", text);
    },
    printErr: (text: string) => {
      toast(ERROR, text);
      console.error("[WASM Error]", text);
    },
  });

  wasmReady.set(true);

  return wasmModule;
}
