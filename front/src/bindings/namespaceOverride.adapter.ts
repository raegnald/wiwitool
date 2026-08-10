import type { INamespaceOverride } from "../bindings/interfaces";
import type { NamespaceOverride } from "../bindings/wasm/wiwitool";
import { callNative } from "../stores/saucerStore";

export class WasmNamespaceOverride implements INamespaceOverride {
  private cppObj: NamespaceOverride | null;

  constructor(cppObj: NamespaceOverride | null) {
    this.cppObj = cppObj;
  }

  async get(): Promise<string> {
    return this.cppObj?.get() || "";
  }

  async set(namespace: string): Promise<void> {
    this.cppObj?.set(namespace);
  }

  async reset(): Promise<void> {
    this.cppObj?.reset();
  }
}

export class NativeNamespaceOverride implements INamespaceOverride {
  private target: string;

  constructor(target: string) {
    this.target = target;
  }

  async get(): Promise<string> {
    return await callNative("namespace_override_get", [this.target]);
  }

  async set(namespace: string): Promise<void> {
    await callNative("namespace_override_set", [this.target, namespace]);
  }

  async reset(): Promise<void> {
    await callNative("namespace_override_reset", [this.target]);
  }
}
