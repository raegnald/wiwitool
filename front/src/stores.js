import { writable } from 'svelte/store';

// This will hold objects looking like:
// { id: number, cppPainting: CppObject, title: string, author: string, ... }
export const paintingsStore = writable([]);
