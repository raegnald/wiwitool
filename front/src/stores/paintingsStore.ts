import { writable, type Writable } from "svelte/store";
import type { Painting } from "../bindings/wiwitool";

export interface PaintingWrapper {
  cppPainting: Painting;
  selected: boolean;
  cachedOriginalJS?: globalThis.ImageData;
  cachedPaintingJS?: globalThis.ImageData;
}

export const paintingsStore: Writable<PaintingWrapper[]> = writable([]);
