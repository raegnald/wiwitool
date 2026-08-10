import { writable } from "svelte/store";

export const nativeReady = writable(false);

declare global {
  interface Window {
    saucer?: {
      call: (name: string, params?: any[]) => Promise<any>;
      exposed?: Record<string, Function>;
    };
  }
}

export async function callNative<T = any>(
  name: string,
  params: any[] = [],
): Promise<T> {
  if (window.saucer && typeof window.saucer.call === "function") {
    return await window.saucer.call(name, params);
  } else if (
    window.saucer &&
    window.saucer.exposed &&
    typeof window.saucer.exposed[name] === "function"
  ) {
    return await window.saucer.exposed[name](...params);
  }

  console.warn(`[Saucer] window.saucer not found for call '${name}'.`);

  throw new Error(
    `Saucer IPC not available in standard browser environment for '${name}'`,
  );
}

export async function callNativeJson<T = any>(
  name: string,
  params: any[] = [],
): Promise<T> {
  return JSON.parse(await callNative(name, params));
}

// Initialise and check connection
if (typeof window !== "undefined") {
  const checkInterval = setInterval(() => {
    if (window.saucer) {
      nativeReady.set(true);
      clearInterval(checkInterval);
    }
  }, 50);

  // Stop checking after 2 seconds if not in saucer
  setTimeout(() => clearInterval(checkInterval), 2000);
}
