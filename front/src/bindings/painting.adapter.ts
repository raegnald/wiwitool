import type { IPainting } from "./interfaces";
import type { Painting } from "./wasm/wiwitool";
import { callNative, callNativeJson } from "../stores/saucerStore";

export class WasmPainting implements IPainting {
  private cppObj: Painting;
  public id: string;

  constructor(cppObj: Painting) {
    this.cppObj = cppObj;
    this.id = cppObj.stringId();
  }

  get title(): string {
    return this.cppObj.title;
  }
  set title(val: string) {
    this.cppObj.title = val;
  }

  get author(): string {
    return this.cppObj.author;
  }
  set author(val: string) {
    this.cppObj.author = val;
  }

  get cropX(): number {
    return this.cppObj.cropX;
  }
  get cropY(): number {
    return this.cppObj.cropY;
  }
  get cropW(): number {
    return this.cppObj.cropW;
  }
  get cropH(): number {
    return this.cppObj.cropH;
  }

  get contrast(): number {
    return this.cppObj.contrast;
  }
  set contrast(val: number) {
    this.cppObj.contrast = val;
  }

  get brightness(): number {
    return this.cppObj.brightness;
  }
  set brightness(val: number) {
    this.cppObj.brightness = val;
  }

  get placeable(): boolean {
    return this.cppObj.placeable;
  }
  set placeable(val: boolean) {
    this.cppObj.placeable = val;
  }

  get hasStonecutterRecipe(): boolean {
    return this.cppObj.hasStonecutterRecipe;
  }
  set hasStonecutterRecipe(val: boolean) {
    this.cppObj.hasStonecutterRecipe = val;
  }

  get pixelPerBlock(): number {
    return this.cppObj.pixelPerBlock;
  }
  set pixelPerBlock(ppb: number) {
    this.cppObj.pixelPerBlock = ppb;
  }

  stringId(): string {
    return this.id;
  }

  async setTitle(title: string): Promise<void> {
    this.cppObj.title = title;
  }
  async setAuthor(author: string): Promise<void> {
    this.cppObj.author = author;
  }
  async setCrop(x: number, y: number, w: number, h: number): Promise<void> {
    this.cppObj.setCrop(x, y, w, h);
  }
  async setContrast(contrast: number): Promise<void> {
    this.cppObj.contrast = contrast;
  }
  async setBrightness(brightness: number): Promise<void> {
    this.cppObj.brightness = brightness;
  }
  async setPlaceable(placeable: boolean): Promise<void> {
    this.cppObj.placeable = placeable;
  }
  async setHasStonecutterRecipe(hasRecipe: boolean): Promise<void> {
    this.cppObj.hasStonecutterRecipe = hasRecipe;
  }
  async setPixelPerBlock(ppb: number): Promise<void> {
    this.cppObj.pixelPerBlock = ppb;
  }

  async useNoFrame(): Promise<void> {
    if (typeof this.cppObj.useNoFrame === "function") this.cppObj.useNoFrame();
  }

  async useProceduralFrame(): Promise<void> {
    if (typeof this.cppObj.useProceduralFrame === "function")
      this.cppObj.useProceduralFrame();
  }

  getProceduralSettings(): {
    seed: bigint | number | string;
    tintHex: string;
  } | null {
    return typeof this.cppObj.getProceduralSettings === "function"
      ? this.cppObj.getProceduralSettings()
      : null;
  }

  async setProceduralSettings(
    seed: bigint | number | string,
    tintHex: string,
  ): Promise<void> {
    const settings = this.getProceduralSettings();
    if (!settings) return;

    settings.seed = BigInt(seed);
    settings.tintHex = tintHex;
  }

  async getRatios(): Promise<string[]> {
    return this.cppObj.getRatios() || [];
  }
  async addRatio(ratio: string): Promise<void> {
    this.cppObj.addRatio(ratio);
  }
  async removeRatio(ratio: string): Promise<boolean> {
    return this.cppObj.removeRatio(ratio);
  }

  async rotateClockwise(): Promise<void> {
    this.cppObj.rotateClockwise();
  }
  async rotateAnticlockwise(): Promise<void> {
    this.cppObj.rotateAnticlockwise();
  }

  async getIconUrl(): Promise<string> {
    const iconData = this.cppObj.iconData();

    const canvas = document.createElement("canvas");
    canvas.width = iconData.width;
    canvas.height = iconData.height;

    const ctx = canvas.getContext("2d");
    if (ctx) {
      const imgData = ctx.createImageData(iconData.width, iconData.height);
      const pixels = iconData.getPixels();
      imgData.data.set(pixels);
      ctx.putImageData(imgData, 0, 0);
    }

    iconData.delete();
    return canvas.toDataURL();
  }

  async getPaintingUrl(ratio?: string): Promise<string> {
    let imgData = ratio
      ? this.cppObj.paintingData(ratio)
      : this.cppObj.croppedOriginalData();

    const canvas = document.createElement("canvas");
    canvas.width = imgData.width;
    canvas.height = imgData.height;

    const ctx = canvas.getContext("2d");

    if (ctx) {
      const pixelsData = ctx.createImageData(imgData.width, imgData.height);
      pixelsData.data.set(imgData.getPixels());
      ctx.putImageData(pixelsData, 0, 0);
    }

    imgData.delete();
    return canvas.toDataURL();
  }

  async dispose(): Promise<void> {
    if (this.cppObj && !this.cppObj.isDeleted()) {
      this.cppObj.delete();
    }
  }
}

export class NativePainting implements IPainting {
  public id: string;
  private _title: string = "";
  private _author: string = "";
  public cropX: number = 0;
  public cropY: number = 0;
  public cropW: number = 1;
  public cropH: number = 1;
  private _contrast: number = 1;
  private _brightness: number = 0;
  private _placeable: boolean = true;
  private _hasStonecutterRecipe: boolean = false;
  private _pixelPerBlock: number = 16;
  private _frameSeed: string = "0";
  private _frameTint: string = "#ffffff";
  private _isProceduralFrame: boolean = true;

  constructor(id: string, metadata?: any) {
    this.id = id;
    if (metadata) {
      if (metadata.title !== undefined) this._title = metadata.title;
      if (metadata.author !== undefined) this._author = metadata.author;
      if (metadata.cropX !== undefined) this.cropX = metadata.cropX;
      if (metadata.cropY !== undefined) this.cropY = metadata.cropY;
      if (metadata.cropW !== undefined) this.cropW = metadata.cropW;
      if (metadata.cropH !== undefined) this.cropH = metadata.cropH;
      if (metadata.contrast !== undefined) this._contrast = metadata.contrast;
      if (metadata.brightness !== undefined)
        this._brightness = metadata.brightness;
      if (metadata.placeable !== undefined)
        this._placeable = metadata.placeable;
      if (metadata.hasStonecutterRecipe !== undefined)
        this._hasStonecutterRecipe = metadata.hasStonecutterRecipe;
      if (metadata.pixelPerBlock !== undefined)
        this._pixelPerBlock = metadata.pixelPerBlock;
      if (metadata.isProceduralFrame !== undefined)
        this._isProceduralFrame = Boolean(metadata.isProceduralFrame);
      if (metadata.frameSeed !== undefined)
        this._frameSeed = String(metadata.frameSeed);
      if (metadata.tintHex !== undefined)
        this._frameTint = metadata.tintHex;
    }
  }

  stringId(): string {
    return this.id;
  }

  get title(): string {
    return this._title;
  }
  set title(val: string) {
    this._title = val;
    callNative("painting_set_title", [this.id, val]);
  }

  get author(): string {
    return this._author;
  }
  set author(val: string) {
    this._author = val;
    callNative("painting_set_author", [this.id, val]);
  }

  get contrast(): number {
    return this._contrast;
  }
  set contrast(val: number) {
    this._contrast = val;
    callNative("painting_set_contrast", [this.id, val]);
  }

  get brightness(): number {
    return this._brightness;
  }
  set brightness(val: number) {
    this._brightness = val;
    callNative("painting_set_brightness", [this.id, val]);
  }

  get placeable(): boolean {
    return this._placeable;
  }
  set placeable(val: boolean) {
    this._placeable = val;
    callNative("painting_set_placeable", [this.id, val]);
  }

  get hasStonecutterRecipe(): boolean {
    return this._hasStonecutterRecipe;
  }
  set hasStonecutterRecipe(val: boolean) {
    this._hasStonecutterRecipe = val;
    callNative("painting_set_has_stonecutter_recipe", [this.id, val]);
  }

  get pixelPerBlock(): number {
    return this._pixelPerBlock;
  }
  set pixelPerBlock(val: number) {
    this._pixelPerBlock = val;
    callNative("painting_set_pixels_per_block", [this.id, val]);
  }

  async setTitle(title: string): Promise<void> {
    this.title = title;
  }

  async setAuthor(author: string): Promise<void> {
    this.author = author;
  }

  async setCrop(x: number, y: number, w: number, h: number): Promise<void> {
    this.cropX = x;
    this.cropY = y;
    this.cropW = w;
    this.cropH = h;
    await callNative("painting_set_crop", [this.id, x, y, w, h]);
  }

  async setContrast(contrast: number): Promise<void> {
    this.contrast = contrast;
  }

  async setBrightness(brightness: number): Promise<void> {
    this.brightness = brightness;
  }

  async setPlaceable(placeable: boolean): Promise<void> {
    this.placeable = placeable;
  }

  async setHasStonecutterRecipe(hasRecipe: boolean): Promise<void> {
    this.hasStonecutterRecipe = hasRecipe;
  }

  async setPixelPerBlock(ppb: number): Promise<void> {
    this.pixelPerBlock = ppb;
  }

  async useNoFrame(): Promise<void> {
    this._isProceduralFrame = false;
    await callNative("painting_use_no_frame", [this.id]);
  }

  async useProceduralFrame(): Promise<void> {
    this._isProceduralFrame = true;
    await callNative("painting_use_procedural_frame", [this.id]);
  }

  getProceduralSettings(): {
    seed: bigint | number | string;
    tintHex: string;
  } | null {
    if (!this._isProceduralFrame) return null;

    const self = this;
    return {
      get seed() {
        return self._frameSeed;
      },
      set seed(val: bigint | number | string) {
        self._frameSeed = String(val);
        callNative("painting_set_procedural_settings", [
          self.id,
          self._frameSeed,
          self._frameTint,
        ]);
      },

      get tintHex() {
        return self._frameTint;
      },
      set tintHex(val: string) {
        self._frameTint = val;
        callNative("painting_set_procedural_settings", [
          self.id,
          self._frameSeed,
          self._frameTint,
        ]);
      },
    };
  }

  async setProceduralSettings(
    seed: bigint | number | string,
    tintHex: string,
  ): Promise<void> {
    this._frameSeed = String(seed);
    this._frameTint = tintHex;

    await callNative("painting_set_procedural_settings", [
      this.id,
      this._frameSeed,
      this._frameTint,
    ]);
  }

  async getRatios(): Promise<string[]> {
    return await callNativeJson("painting_get_ratios", [this.id]);
  }

  async addRatio(ratio: string): Promise<void> {
    await callNative("painting_add_ratio", [this.id, ratio]);
  }

  async removeRatio(ratio: string): Promise<boolean> {
    return await callNative("painting_remove_ratio", [this.id, ratio]);
  }

  async rotateClockwise(): Promise<void> {
    await callNative("painting_rotate_clockwise", [this.id]);
  }

  async rotateAnticlockwise(): Promise<void> {
    await callNative("painting_rotate_anticlockwise", [this.id]);
  }

  async getIconUrl(): Promise<string> {
    return await callNative("painting_get_icon_url", [this.id]);
  }

  async getPaintingUrl(ratioStr?: string): Promise<string> {
    return await callNative("painting_get_data_url", [this.id, ratioStr || ""]);
  }

  async dispose(): Promise<void> {
    await callNative("painting_dispose", [this.id]);
  }
}
