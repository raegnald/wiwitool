import { writable, type Writable } from "svelte/store";
import type { IPainting } from "../bindings/interfaces";

export interface PaintingWrapper {
  painting: IPainting;
  selected: boolean;
  iconUrl?: string;
}

export const paintingsStore: Writable<PaintingWrapper[]> = writable([]);
