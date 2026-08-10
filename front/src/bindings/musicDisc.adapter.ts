import type { IMusicDisc } from "./interfaces";
import type { MusicDisc } from "../bindings/wasm/wiwitool";
import { callNative, callNativeJson } from "../stores/saucerStore";

export class WasmMusicDisc implements IMusicDisc {
  private cppObj: MusicDisc;
  public id: string;

  constructor(cppObj: MusicDisc) {
    this.cppObj = cppObj;
    this.id = cppObj.stringId();
  }

  get title(): string {
    return this.cppObj.title;
  }
  set title(val: string) {
    this.cppObj.title = val;
  }

  get artist(): string {
    return this.cppObj.artist;
  }
  set artist(val: string) {
    this.cppObj.artist = val;
  }

  get hasStonecutterRecipe(): boolean {
    return this.cppObj.hasStonecutterRecipe;
  }
  set hasStonecutterRecipe(val: boolean) {
    this.cppObj.hasStonecutterRecipe = val;
  }

  get comparatorOutput(): number {
    return this.cppObj.comparatorOutput;
  }
  set comparatorOutput(val: number) {
    this.cppObj.comparatorOutput = val;
  }

  get silenceGap(): number {
    return this.cppObj.silenceGap;
  }
  set silenceGap(val: number) {
    this.cppObj.silenceGap = val;
  }

  stringId(): string {
    return this.id;
  }

  async setTitle(title: string): Promise<void> {
    this.cppObj.title = title;
  }
  async setArtist(artist: string): Promise<void> {
    this.cppObj.artist = artist;
  }
  async setHasStonecutterRecipe(hasRecipe: boolean): Promise<void> {
    this.cppObj.hasStonecutterRecipe = hasRecipe;
  }
  async setComparatorOutput(val: number): Promise<void> {
    this.cppObj.comparatorOutput = val;
  }
  async setSilenceGap(gap: number): Promise<void> {
    this.cppObj.silenceGap = gap;
  }

  async setTrim(start: number, end: number): Promise<void> {
    this.cppObj.setTrim(start, end);
  }
  getTrimStart(): number {
    return this.cppObj.getTrimStart();
  }
  getTrimEnd(): number {
    return this.cppObj.getTrimEnd();
  }
  getDurationSeconds(): number {
    return this.cppObj.getDurationSeconds();
  }
  getTrimmedDuration(): number {
    return this.cppObj.getTrimmedDuration();
  }

  isCoverModified(): boolean {
    return typeof this.cppObj.isCoverModified === "function"
      ? this.cppObj.isCoverModified()
      : false;
  }

  hasOriginalCover(): boolean {
    return typeof this.cppObj.hasOriginalCover === "function"
      ? this.cppObj.hasOriginalCover()
      : false;
  }

  async removeCover(): Promise<void> {
    if (typeof this.cppObj.removeCover === "function")
      this.cppObj.removeCover();
  }

  async restoreOriginalCover(): Promise<void> {
    if (typeof this.cppObj.restoreOriginalCover === "function")
      this.cppObj.restoreOriginalCover();
  }

  getOggAudioData(): Uint8Array {
    return typeof this.cppObj.getOggAudioData === "function"
      ? this.cppObj.getOggAudioData()
      : new Uint8Array(0);
  }

  getWaveform(): Float32Array {
    if (typeof this.cppObj.getWaveform !== "function")
      return new Float32Array(0);

    const view = this.cppObj.getWaveform();
    return new Float32Array(view);
  }

  async getCoverUrl(): Promise<string> {
    const imgData = this.cppObj.getDiscItemImage();
    if (!imgData || imgData.empty?.()) return "";

    const canvas = document.createElement("canvas");
    canvas.width = imgData.width;
    canvas.height = imgData.height;

    const ctx = canvas.getContext("2d");
    if (ctx) {
      const pixels = imgData.getPixels();
      const imageDataObj = ctx.createImageData(imgData.width, imgData.height);
      imageDataObj.data.set(pixels);
      ctx.putImageData(imageDataObj, 0, 0);
    }

    imgData.delete();
    return canvas.toDataURL();
  }

  async setCoverFromBytes(data: Uint8Array): Promise<void> {
    this.cppObj.setCoverFromBytes(data);
  }

  async setOriginalCoverFromBytes(data: Uint8Array): Promise<void> {
    return typeof this.cppObj.setOriginalCoverFromBytes === "function"
      ? this.cppObj.setOriginalCoverFromBytes(data)
      : this.cppObj.setCoverFromBytes(data);
  }

  async dispose(): Promise<void> {
    if (this.cppObj && !this.cppObj.isDeleted()) {
      this.cppObj.delete();
    }
  }
}

export class NativeMusicDisc implements IMusicDisc {
  public id: string;
  private _title: string = "";
  private _artist: string = "";
  private _hasStonecutterRecipe: boolean = false;
  private _comparatorOutput: number = 1;
  private _silenceGap: number = 0;

  constructor(id: string, metadata?: any) {
    this.id = id;
    if (metadata) {
      if (metadata.title !== undefined) this._title = metadata.title;
      if (metadata.artist !== undefined) this._artist = metadata.artist;
      if (metadata.hasStonecutterRecipe !== undefined)
        this._hasStonecutterRecipe = metadata.hasStonecutterRecipe;
      if (metadata.comparatorOutput !== undefined)
        this._comparatorOutput = metadata.comparatorOutput;
      if (metadata.silenceGap !== undefined)
        this._silenceGap = metadata.silenceGap;
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
    callNative("disc_set_title", [this.id, val]);
  }

  get artist(): string {
    return this._artist;
  }
  set artist(val: string) {
    this._artist = val;
    callNative("disc_set_artist", [this.id, val]);
  }

  get hasStonecutterRecipe(): boolean {
    return this._hasStonecutterRecipe;
  }
  set hasStonecutterRecipe(val: boolean) {
    this._hasStonecutterRecipe = val;
    callNative("disc_set_has_stonecutter_recipe", [this.id, val]);
  }

  get comparatorOutput(): number {
    return this._comparatorOutput;
  }
  set comparatorOutput(val: number) {
    this._comparatorOutput = val;
    callNative("disc_set_comparator_output", [this.id, val]);
  }

  get silenceGap(): number {
    return this._silenceGap;
  }
  set silenceGap(val: number) {
    this._silenceGap = val;
    callNative("disc_set_silence_gap", [this.id, val]);
  }

  async setTitle(title: string): Promise<void> {
    this.title = title;
  }

  async setArtist(artist: string): Promise<void> {
    this.artist = artist;
  }

  async setHasStonecutterRecipe(hasRecipe: boolean): Promise<void> {
    this.hasStonecutterRecipe = hasRecipe;
  }

  async setComparatorOutput(val: number): Promise<void> {
    this.comparatorOutput = val;
  }

  async setSilenceGap(gap: number): Promise<void> {
    this.silenceGap = gap;
  }

  async setTrim(start: number, end: number): Promise<void> {
    await callNative("disc_set_trim", [this.id, start, end]);
  }

  async getTrimStart(): Promise<number> {
    return await callNative("disc_get_trim_start", [this.id]);
  }

  async getTrimEnd(): Promise<number> {
    return await callNative("disc_get_trim_end", [this.id]);
  }

  async getDurationSeconds(): Promise<number> {
    return await callNative("disc_get_duration_seconds", [this.id]);
  }

  async getTrimmedDuration(): Promise<number> {
    return await callNative("disc_get_trimmed_duration", [this.id]);
  }

  async isCoverModified(): Promise<boolean> {
    return await callNative("disc_is_cover_modified", [this.id]);
  }

  async hasOriginalCover(): Promise<boolean> {
    return await callNative("disc_has_original_cover", [this.id]);
  }

  async removeCover(): Promise<void> {
    await callNative("disc_remove_cover", [this.id]);
  }

  async restoreOriginalCover(): Promise<void> {
    await callNative("disc_restore_original_cover", [this.id]);
  }

  async getCoverUrl(): Promise<string> {
    return await callNative("disc_get_cover_url", [this.id]);
  }

  async setCoverFromBytes(data: Uint8Array): Promise<void> {
    await callNative("disc_set_cover_from_bytes", [this.id, Array.from(data)]);
  }

  async setOriginalCoverFromBytes(data: Uint8Array): Promise<void> {
    await callNative("disc_set_original_cover_from_bytes", [
      this.id,
      Array.from(data),
    ]);
  }

  async getOggAudioData(): Promise<Uint8Array> {
    const res = await callNative("disc_get_ogg_audio_data", [this.id]);
    return res instanceof Uint8Array ? res : new Uint8Array(res || []);
  }

  async getWaveform(): Promise<Float32Array> {
    const waveform = await callNativeJson("disc_get_waveform", [this.id]);
    return new Float32Array(waveform);
  }

  async dispose(): Promise<void> {
    await callNative("disc_dispose", [this.id]);
  }
}
