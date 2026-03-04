import { writable } from "svelte/store";
import type { Wiwiworkspace } from "../bindings/wiwitool";

export const workspace = writable<Wiwiworkspace | null>(null);
