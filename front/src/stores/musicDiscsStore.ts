import { writable, type Writable } from "svelte/store";
import type { MusicDisc } from "../bindings/wiwitool";

export interface MusicDiscWrapper {
  cppDisc: MusicDisc;
  selected: boolean;
}

export const musicDiscsStore: Writable<MusicDiscWrapper[]> = writable([]);

export const activeDiscId = writable<string | null>(null);
