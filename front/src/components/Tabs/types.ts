import type { Writable } from "svelte/store";

export interface TabInfo {
  id: string;
  title: string;
}

export interface TabsContext {
  registerTab: (tab: TabInfo) => void;
  unregisterTab: (tab: TabInfo) => void;
  selectedTab: Writable<string | null>;
  selectTab: (id: string) => void;
}
