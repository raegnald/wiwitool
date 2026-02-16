// Paintings store

import { writable, type Writable } from "svelte/store";

import type { Painting } from "../bindings/wiwitool";

export interface PaintingWrapper {
  id: number;
  originalImageBytes: Uint8Array;
  title: string;
  author: string;
  cppPainting: Painting;
}

export const paintingsStore: Writable<PaintingWrapper[]> = writable([]);
