// Toasts store

import { writable } from "svelte/store";

export const INFO = "info";
export const WARNING = "warning";
export const ERROR = "error";

export type ToastType = typeof INFO | typeof WARNING | typeof ERROR;

interface ToastMessage {
  id: number;
  type: ToastType;
  message: string;
}

export const toasts = writable<ToastMessage[]>([]);

export function toast(
  type: ToastType,
  message: string,
  duration: number = 3000,
) {
  const id = Date.now() + Math.random(); // "unique" id

  switch (type) {
    case INFO:
      console.log(message);
      break;
    case WARNING:
      console.warn(message);
      break;
    case ERROR:
      console.error(message);
      break;
  }

  toasts.update((all) => [...all, { id, type, message }]);

  setTimeout(() => {
    toasts.update((all) => all.filter((t) => t.id !== id));
  }, duration);
}
