import { writable } from 'svelte/store';

// This is the store that components will subscribe to
export const wasmModule = writable(null);
export const packInstance = writable(null);

let isLoading = false;

export async function initWasm() {
  if (isLoading) return; // Prevent double loading
  isLoading = true;

  // Dynamically load the Emscripten generated JS file
  const script = document.createElement('script');
  script.src = '/executable.js'; // Assumes this is in your public folder

  script.onload = async () => {
    // The script defines a global function (usually named Module or createModule)
    // Check your CMake/Makefile for the EXPORT_NAME. Default is `Module`.
    // If you compiled with -s MODULARIZE=1, you get a factory function.

    if (typeof window.Module === 'function') {
      const moduleInstance = await window.Module({
        // Hook print to console so we see C++ std::println
        print: (text) => console.log('[WASM]:', text),
        printErr: (text) => console.error('[WASM Error]:', text),
      });

      wasmModule.set(moduleInstance);

      // Instantiate the C++ Class immediately and store it
      const PackClass = moduleInstance.Paintings_pack;
      const instance = new PackClass();
      packInstance.set(instance);

      console.log("Wiwitool WASM Initialised");
    }
  };

  document.body.appendChild(script);
}
