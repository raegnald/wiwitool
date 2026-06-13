import { writable } from "svelte/store";
import type { Wiwiworkspace } from "../bindings/wiwitool";

export const workspace = writable<Wiwiworkspace | null>(null);

export const MAX_WORKSPACE_NAME_LENGTH = 32;

export function formatWorkspaceName(name: string) {
  return name
    .replace(/[^(\w|\s)]/g, "")
    .replace(/[\s]/g, "_")
    .slice(0, MAX_WORKSPACE_NAME_LENGTH);
}
