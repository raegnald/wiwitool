import { writable, type Writable } from "svelte/store";
import type { IMusicDisc } from "../bindings/interfaces";

export interface MusicDiscWrapper {
  disc: IMusicDisc;
  selected: boolean;
  coverUrl?: string;
}

export const musicDiscsStore: Writable<MusicDiscWrapper[]> = writable([]);

export const activeDiscId = writable<string | null>(null);
