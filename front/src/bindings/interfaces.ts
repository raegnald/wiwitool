export interface IPainting {
  id: string;
  title: string;
  author: string;
  cropX: number;
  cropY: number;
  cropW: number;
  cropH: number;
  contrast: number;
  brightness: number;
  placeable: boolean;
  hasStonecutterRecipe: boolean;
  pixelPerBlock: number;

  stringId(): string;
  getIconUrl(): Promise<string>;
  getPaintingUrl(ratioStr?: string): Promise<string>;
  setTitle(title: string): Promise<void>;
  setAuthor(author: string): Promise<void>;
  setCrop(x: number, y: number, w: number, h: number): Promise<void>;
  setContrast(contrast: number): Promise<void>;
  setBrightness(brightness: number): Promise<void>;
  setPlaceable(placeable: boolean): Promise<void>;
  setHasStonecutterRecipe(hasRecipe: boolean): Promise<void>;
  setPixelPerBlock(ppb: number): Promise<void>;

  useNoFrame(): Promise<void>;
  useProceduralFrame(): Promise<void>;
  getProceduralSettings(): {
    seed: bigint | number | string;
    tintHex: string;
  } | null;
  setProceduralSettings(
    seed: bigint | number | string,
    tintHex: string,
  ): Promise<void>;

  getRatios(): Promise<string[]>;
  addRatio(ratio: string): Promise<void>;
  removeRatio(ratio: string): Promise<boolean>;

  rotateClockwise(): Promise<void>;
  rotateAnticlockwise(): Promise<void>;

  dispose(): Promise<void>;
}

export interface IMusicDisc {
  id: string;
  title: string;
  artist: string;
  hasStonecutterRecipe: boolean;
  comparatorOutput: number;
  silenceGap: number;

  stringId(): string;
  setTitle(title: string): Promise<void>;
  setArtist(artist: string): Promise<void>;
  setHasStonecutterRecipe(hasRecipe: boolean): Promise<void>;
  setComparatorOutput(val: number): Promise<void>;
  setSilenceGap(gap: number): Promise<void>;

  setTrim(start: number, end: number): Promise<void>;
  getTrimStart(): Promise<number> | number;
  getTrimEnd(): Promise<number> | number;
  getDurationSeconds(): Promise<number> | number;
  getTrimmedDuration(): Promise<number> | number;

  getCoverUrl(): Promise<string>;
  setCoverFromBytes(data: Uint8Array): Promise<void>;
  setOriginalCoverFromBytes(data: Uint8Array): Promise<void>;
  removeCover(): Promise<void>;
  restoreOriginalCover(): Promise<void>;
  isCoverModified(): Promise<boolean> | boolean;
  hasOriginalCover(): Promise<boolean> | boolean;

  getOggAudioData(): Promise<Uint8Array> | Uint8Array;
  getWaveform(): Promise<Float32Array | number[]> | Float32Array | number[];

  dispose(): Promise<void>;
}

export interface INamespaceOverride {
  get(): Promise<string>;
  set(namespace: string): Promise<void>;
  reset(): Promise<void>;
}

export interface IWorkspace {
  workspaceName: string;
  invisibleItemFrames: boolean;

  setWorkspaceName(name: string): Promise<void>;
  setInvisibleItemFrames(enabled: boolean): Promise<void>;
  getInvisibleItemFrames?(): Promise<boolean> | boolean;
  isWorkspaceNew?(): Promise<boolean> | boolean;

  getWorkspaceImageUrl(): Promise<string>;
  setWorkspaceImage(fileOrData: File | Uint8Array | string): Promise<void>;
  resetWorkspaceImage(): Promise<void>;

  getPaintings(): Promise<IPainting[]>;
  addPainting(fileOrData: File | Uint8Array | string): Promise<IPainting>;
  removePainting(id: string): Promise<void>;

  getMusicDiscs(): Promise<IMusicDisc[]>;
  addMusicDisc(fileOrData: File | Uint8Array | string): Promise<IMusicDisc>;
  removeMusicDisc(id: string): Promise<void>;

  getPaintingsNamespaceOverride(): Promise<INamespaceOverride | null>;
  getMusicDiscsNamespaceOverride(): Promise<INamespaceOverride | null>;
  getIIFNamespaceOverride(): Promise<INamespaceOverride | null>;

  getReasonableRatios(): Promise<string[]>;
  generateZip(): Promise<Uint8Array | string>;
  deserialise?(data: Uint8Array): Promise<void> | void;

  dispose(): Promise<void>;
}

export interface IFileService {
  openImageFile(): Promise<{
    name: string;
    data: Uint8Array;
    path?: string;
  } | null>;
  openAudioFile(): Promise<{
    name: string;
    data: Uint8Array;
    path?: string;
  } | null>;
  openProjectFile?(): Promise<{
    name: string;
    data: Uint8Array;
    path?: string;
  } | null>;
  savePackFile(
    defaultName: string,
    data: Uint8Array | string,
  ): Promise<boolean>;
}
