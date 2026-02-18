// Paintings store

import { writable, type Writable } from "svelte/store";

import type { Painting } from "../bindings/wiwitool";

export type Crop = {
  x0: number;
  y0: number;
  x1: number;
  y1: number;
};

export interface PaintingWrapper {
  id: number;
  originalImageBytes: Uint8Array;
  title: string;
  author: string;
  cppPainting: Painting;
  selected: boolean;
  crop: Crop | null;
}

export const paintingsStore: Writable<PaintingWrapper[]> = writable([]);
