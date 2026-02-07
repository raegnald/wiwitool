import { writable } from 'svelte/store';

export const wasmReady = writable(false);

let wasmModule = null;

export async function getWasmModule() {
  if (wasmModule) return wasmModule;

  // We fetch the glue script dynamically
  // Note: Emscripten defaults to a global function usually named createModule or Module
  // Check your compiler flags. If you use -s MODULARIZE=1, this changes.
  // Assuming standard output:
  if (!window.createWiwitoolModule) {
    // Load the script tag manually if not present
    await new Promise((resolve) => {
      const script = document.createElement('script');
      script.src = '/wasm/wiwitool.js';
      script.onload = resolve;
      document.body.appendChild(script);
    });
  }

  // 2. Initialise the module with configuration
  // 'createWiwitoolModule' is the name we set in the Makefile
  wasmModule = await window.createWiwitoolModule({
    locateFile: (path) => {
      console.log("locateFile: ", path);
      // Handle both the binary AND the asset bundle
      if (path.endsWith('.wasm') || path.endsWith('.data')) {
        return '/wasm/' + path;
      }
      return path;
    },
    print: (text) => console.log('[WASM]', text),
    printErr: (text) => console.error('[WASM Error]', text)
  });

  wasmReady.set(true);
  return wasmModule;
}
