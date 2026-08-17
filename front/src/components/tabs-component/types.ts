import type { Component } from "svelte";
import type { Writable } from "svelte/store";

export interface TabInfo {
  id: string;
  title: string;
  icon?: Component;
  hidden: boolean;
  activeBackground?: string;
  activeBackgroundDark?: string;
}

export interface TabsContext {
  registerTab: (tab: TabInfo) => void;
  unregisterTab: (tab: TabInfo) => void;
  selectedTab: Writable<string | null>;
  selectTab: (id: string) => void;
}
