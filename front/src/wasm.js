import { writable } from "svelte/store";

export const wasmReady = writable(false);
let wasmModule = null;

export async function getWasmModule() {
  if (wasmModule) return wasmModule;

  // Load the script if it's not already there
  if (!window.createWiwitoolModule) {
    await new Promise((resolve, reject) => {
      const script = document.createElement("script");
      script.src = "/wasm/wiwitool.js"; // Ensure this matches your public folder structure
      script.onload = resolve;
      script.onerror = reject;
      document.body.appendChild(script);
    });
  }

  // Initialise the module with configuration
  wasmModule = await window.createWiwitoolModule({
    locateFile: (path) => {
      if (path.endsWith(".wasm") || path.endsWith(".data")) {
        return "/wasm/" + path;
      }
      console.log("Locate file ", path);
      return path;
    },
    print: (text) => console.log("[WASM]", text),
    printErr: (text) => console.error("[WASM Error]", text),
  });

  wasmReady.set(true);
  return wasmModule;
}
