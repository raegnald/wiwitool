// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
declare namespace RuntimeExports {
    namespace FS {
        export let root: null;
        export let mounts: never[];
        export let devices: {};
        export let streams: never[];
        export let nextInode: number;
        export let nameTable: null;
        export let currentPath: string;
        export let initialized: boolean;
        export let ignorePermissions: boolean;
        export let filesystems: null;
        export let syncFSRequests: number;
        export { ErrnoError };
        export { FSStream };
        export { FSNode };
        export function lookupPath(path: any, opts?: {}): {
            path: string;
            node?: undefined;
        } | {
            path: string;
            node: any;
        };
        export function getPath(node: any): any;
        export function hashName(parentid: any, name: any): number;
        export function hashAddNode(node: any): void;
        export function hashRemoveNode(node: any): void;
        export function lookupNode(parent: any, name: any): any;
        export function createNode(parent: any, name: any, mode: any, rdev: any): any;
        export function destroyNode(node: any): void;
        export function isRoot(node: any): boolean;
        export function isMountpoint(node: any): boolean;
        export function isFile(mode: any): boolean;
        export function isDir(mode: any): boolean;
        export function isLink(mode: any): boolean;
        export function isChrdev(mode: any): boolean;
        export function isBlkdev(mode: any): boolean;
        export function isFIFO(mode: any): boolean;
        export function isSocket(mode: any): boolean;
        export function flagsToPermissionString(flag: any): string;
        export function nodePermissions(node: any, perms: any): 0 | 2;
        export function mayLookup(dir: any): any;
        export function mayCreate(dir: any, name: any): any;
        export function mayDelete(dir: any, name: any, isdir: any): any;
        export function mayOpen(node: any, flags: any): any;
        export function checkOpExists(op: any, err: any): any;
        export let MAX_OPEN_FDS: number;
        export function nextfd(): number;
        export function getStreamChecked(fd: any): any;
        export function getStream(fd: any): any;
        export function createStream(stream: any, fd?: number): any;
        export function closeStream(fd: any): void;
        export function dupStream(origStream: any, fd?: number): any;
        export function doSetAttr(stream: any, node: any, attr: any): void;
        export namespace chrdev_stream_ops {
            function open(stream: any): void;
            function llseek(): never;
        }
        export function major(dev: any): number;
        export function minor(dev: any): number;
        export function makedev(ma: any, mi: any): number;
        export function registerDevice(dev: any, ops: any): void;
        export function getDevice(dev: any): any;
        export function getMounts(mount: any): any[];
        export function syncfs(populate: any, callback: any): void;
        export function mount(type: any, opts: any, mountpoint: any): any;
        export function unmount(mountpoint: any): void;
        export function lookup(parent: any, name: any): any;
        export function mknod(path: any, mode: any, dev: any): any;
        export function statfs(path: any): any;
        export function statfsStream(stream: any): any;
        export function statfsNode(node: any): {
            bsize: number;
            frsize: number;
            blocks: number;
            bfree: number;
            bavail: number;
            files: any;
            ffree: number;
            fsid: number;
            flags: number;
            namelen: number;
        };
        export function create(path: any, mode?: number): any;
        export function mkdir(path: any, mode?: number): any;
        export function mkdirTree(path: any, mode: any): void;
        export function mkdev(path: any, mode: any, dev: any): any;
        export function symlink(oldpath: any, newpath: any): any;
        export function link(oldpath: any, newpath: any, flags: any): any;
        export function rename(old_path: any, new_path: any): void;
        export function rmdir(path: any): void;
        export function readdir(path: any): any;
        export function unlink(path: any): void;
        export function readlink(path: any): any;
        export function stat(path: any, dontFollow: any): any;
        export function fstat(fd: any): any;
        export function lstat(path: any): any;
        export function doChmod(stream: any, node: any, mode: any, dontFollow: any): void;
        export function chmod(path: any, mode: any, dontFollow: any): void;
        export function lchmod(path: any, mode: any): void;
        export function fchmod(fd: any, mode: any): void;
        export function doChown(stream: any, node: any, dontFollow: any): void;
        export function chown(path: any, uid: any, gid: any, dontFollow: any): void;
        export function lchown(path: any, uid: any, gid: any): void;
        export function fchown(fd: any, uid: any, gid: any): void;
        export function doTruncate(stream: any, node: any, len: any): void;
        export function truncate(path: any, len: any): void;
        export function ftruncate(fd: any, len: any): void;
        export function utime(path: any, atime: any, mtime: any, dontFollow: any): void;
        export function open(path: any, flags: any, mode?: number): any;
        export function close(stream: any): void;
        export function isClosed(stream: any): boolean;
        export function llseek(stream: any, offset: any, whence: any): any;
        export function read(stream: any, buffer: any, offset: any, length: any, position: any): any;
        export function write(stream: any, buffer: any, offset: any, length: any, position: any, canOwn: any): any;
        export function mmap(stream: any, length: any, position: any, prot: any, flags: any): any;
        export function msync(stream: any, buffer: any, offset: any, length: any, mmapFlags: any): any;
        export function ioctl(stream: any, cmd: any, arg: any): any;
        export function readFile(path: any, opts?: {}): Uint8Array<any>;
        export function writeFile(path: any, data: any, opts?: {}): void;
        export function cwd(): any;
        export function chdir(path: any): void;
        export function createDefaultDirectories(): void;
        export function createDefaultDevices(): void;
        export function createSpecialDirectories(): void;
        export function createStandardStreams(input: any, output: any, error: any): void;
        export function staticInit(): void;
        export function init(input: any, output: any, error: any): void;
        export function quit(): void;
        export function findObject(path: any, dontResolveLastLink: any): any;
        export function analyzePath(path: any, dontResolveLastLink: any): {
            isRoot: boolean;
            exists: boolean;
            error: number;
            name: null;
            path: null;
            object: null;
            parentExists: boolean;
            parentPath: null;
            parentObject: null;
        };
        export function createPath(parent: any, path: any, canRead: any, canWrite: any): any;
        export function createFile(parent: any, name: any, properties: any, canRead: any, canWrite: any): any;
        export function createDataFile(parent: any, name: any, data: any, canRead: any, canWrite: any, canOwn: any): void;
        export function createDevice(parent: any, name: any, input: any, output: any): any;
        export function forceLoadFile(obj: any): true | undefined;
        export function createLazyFile(parent: any, name: any, url: any, canRead: any, canWrite: any): any;
    }
    function getExceptionMessage(exn: any): any;
    function FS_createPath(...args: any[]): any;
    function FS_createDataFile(...args: any[]): any;
    function FS_preloadFile(parent: any, name: any, url: any, canRead: any, canWrite: any, dontCreateFile: any, canOwn: any, preFinish: any): Promise<void>;
    function FS_unlink(...args: any[]): any;
    function FS_createLazyFile(...args: any[]): any;
    function FS_createDevice(...args: any[]): any;
    function addRunDependency(id: any): void;
    function removeRunDependency(id: any): void;
}
declare class ErrnoError {
    constructor(errno: any);
    name: string;
    errno: any;
}
declare class FSStream {
    shared: {};
    set object(val: any);
    get object(): any;
    node: any;
    get isRead(): boolean;
    get isWrite(): boolean;
    get isAppend(): number;
    set flags(val: any);
    get flags(): any;
    set position(val: any);
    get position(): any;
}
declare class FSNode {
    constructor(parent: any, name: any, mode: any, rdev: any);
    node_ops: {};
    stream_ops: {};
    readMode: number;
    writeMode: number;
    mounted: null;
    parent: any;
    mount: any;
    id: number;
    name: any;
    mode: any;
    rdev: any;
    atime: number;
    mtime: number;
    ctime: number;
    set read(val: boolean);
    get read(): boolean;
    set write(val: boolean);
    get write(): boolean;
    get isFolder(): any;
    get isDevice(): any;
    addListener(cb: any, exclusive?: boolean): {
        listeners: any;
        entry: {
            cb: any;
            exclusive: boolean;
        };
    };
    notifyListeners(flags: any): void;
    exclTurn: any;
}
interface WasmModule {
}

type EmbindString = ArrayBuffer|Uint8Array|Uint8ClampedArray|Int8Array|string;
export interface ClassHandle {
  isAliasOf(other: ClassHandle): boolean;
  delete(): void;
  deleteLater(): this;
  isDeleted(): boolean;
  // @ts-ignore - If targeting lower than ESNext, this symbol might not exist.
  [Symbol.dispose](): void;
  clone(): this;
}
export interface MusicDisc extends ClassHandle {
  hasStonecutterRecipe: boolean;
  comparatorOutput: number;
  silenceGap: number;
  get title(): string;
  set title(value: EmbindString);
  get artist(): string;
  set artist(value: EmbindString);
  restoreOriginalCover(): void;
  removeCover(): void;
  isCoverModified(): boolean;
  hasOriginalCover(): boolean;
  setTrim(_0: number, _1: number): void;
  getTrimStart(): number;
  getTrimEnd(): number;
  getDurationSeconds(): number;
  getTrimmedDuration(): number;
  stringId(): string;
  setCoverFromBytes(_0: EmbindString): void;
  setOriginalCoverFromBytes(_0: EmbindString): void;
  setPcmAudio(_0: EmbindString, _1: EmbindString, _2: number): void;
  getWaveform(): any;
  getOggAudioData(): any;
  encodeOgg(): any;
  setCover(_0: ImageData): void;
  getDiscItemImage(): ImageData;
}

export interface Serialiser extends ClassHandle {
}

export interface MinecraftPack extends ClassHandle {
  generate(_0: boolean): void;
  compress(): void;
}

export interface PaintingsVector extends ClassHandle, Iterable<Painting> {
  size(): number;
  get(_0: number): Painting | undefined;
  push_back(_0: Painting): void;
  resize(_0: number, _1: Painting): void;
  set(_0: number, _1: Painting): boolean;
}

export interface PaintingsPack extends MinecraftPack {
  setPaintings(_0: WorkspacePaintingsVector): void;
}

export interface PaintingRatio extends ClassHandle {
}

export interface ProceduralFrameGenerator extends ClassHandle {
  seed: bigint;
  get tintHex(): string;
  set tintHex(value: EmbindString);
  tint: Pixel;
}

export interface PaintingBufferVector extends ClassHandle, Iterable<number> {
  push_back(_0: number): void;
  resize(_0: number, _1: number): void;
  size(): number;
  get(_0: number): number | undefined;
  set(_0: number, _1: number): boolean;
}

export interface Painting extends ClassHandle {
  get title(): string;
  set title(value: EmbindString);
  get author(): string;
  set author(value: EmbindString);
  readonly cropX: number;
  readonly cropY: number;
  readonly cropW: number;
  readonly cropH: number;
  contrast: number;
  brightness: number;
  placeable: boolean;
  hasStonecutterRecipe: boolean;
  pixelPerBlock: number;
  clone(): Painting | null;
  stringId(): string;
  setCrop(_0: number, _1: number, _2: number, _3: number): void;
  getRatios(): any;
  addRatio(_0: EmbindString): void;
  removeRatio(_0: EmbindString): boolean;
  hasRatio(_0: EmbindString): boolean;
  rotateClockwise(): void;
  rotateAnticlockwise(): void;
  useProceduralFrame(): void;
  useNoFrame(): void;
  getProceduralSettings(): ProceduralFrameGenerator | null;
  isFrameProcedural(): boolean;
  isFrameNonexistent(): boolean;
  refresh(): void;
  originalData(): ImageData;
  croppedOriginalData(): ImageData;
  iconData(): ImageData;
  paintingData(_0: EmbindString): ImageData;
}

export interface WorkspacePaintingsVector extends ClassHandle, Iterable<Painting | null> {
  push_back(_0: Painting | null): void;
  resize(_0: number, _1: Painting | null): void;
  size(): number;
  get(_0: number): Painting | null | undefined;
  set(_0: number, _1: Painting | null): boolean;
}

export interface WorkspaceMusicDiscsVector extends ClassHandle, Iterable<MusicDisc | null> {
  push_back(_0: MusicDisc | null): void;
  resize(_0: number, _1: MusicDisc | null): void;
  size(): number;
  get(_0: number): MusicDisc | null | undefined;
  set(_0: number, _1: MusicDisc | null): boolean;
}

export interface NamespaceOverride extends ClassHandle {
  get(): any;
  set(_0: EmbindString): void;
  reset(): void;
}

export interface Wiwiworkspace extends ClassHandle {
  get workspaceName(): string;
  set workspaceName(value: EmbindString);
  invisibleItemFrames: boolean;
  workspaceImage: ImageData;
  resetWorkspaceImage(): void;
  setWorkspaceIconFromBytes(_0: EmbindString): void;
  addPainting(_0: Painting | null): void;
  removePainting(_0: EmbindString): void;
  getPaintings(): WorkspacePaintingsVector;
  addMusicDisc(_0: MusicDisc | null): void;
  removeMusicDisc(_0: EmbindString): void;
  getMusicDiscs(): WorkspaceMusicDiscsVector;
  getPaintingsNamespaceOverride(): NamespaceOverride | null;
  getMusicDiscsNamespaceOverride(): NamespaceOverride | null;
  getIIFNamespaceOverride(): NamespaceOverride | null;
  isWorkspaceNew(): boolean;
  serialise(): PaintingBufferVector;
  deserialise(_0: any): void;
  generateZip(): string;
}

export interface MinecraftPacker extends ClassHandle {
  add(_0: MinecraftPack): void;
  getZip(): string;
}

export interface InvisibleItemFramePack extends MinecraftPack {
}

export type Pixel = {
  r: number,
  g: number,
  b: number,
  a: number
};

export interface ImageData extends ClassHandle {
  readonly width: number;
  readonly height: number;
  readonly channels: number;
  empty(): boolean;
  path(): string;
  getPixels(): any;
  at(_0: number, _1: number): Pixel;
  scaleFactor(_0: number): ImageData;
  scaleDims(_0: number, _1: number): ImageData;
  crop(_0: number, _1: number, _2: number, _3: number): ImageData;
  cropRatio(_0: number, _1: number): ImageData;
  rotateClockwise(): ImageData;
  rotateAnticlockwise(): ImageData;
  flipVertically(): ImageData;
  flipHorizontally(): ImageData;
  adjustContrastBrightness(_0: number, _1: number): ImageData;
  adjustContrast(_0: number): ImageData;
  adjustBrightness(_0: number): ImageData;
  encodePng(): any;
  saveAs(_0: EmbindString): void;
  savePng(_0: EmbindString): void;
  saveJpg(_0: EmbindString, _1: number): void;
}

interface EmbindModule {
  MusicDisc: {
    new(): MusicDisc;
  };
  Serialiser: {
    serialise(_0: PaintingBufferVector): void;
    deserialise(): PaintingBufferVector;
  };
  MinecraftPack: {};
  PaintingsVector: {
    new(): PaintingsVector;
  };
  PaintingsPack: {
    new(): PaintingsPack;
  };
  PaintingRatio: {
    getPreferred(): any;
    getReasonable(): any;
  };
  ProceduralFrameGenerator: {};
  PaintingBufferVector: {
    new(): PaintingBufferVector;
  };
  Painting: {
    new(_0: any): Painting;
  };
  WorkspacePaintingsVector: {
    new(): WorkspacePaintingsVector;
  };
  WorkspaceMusicDiscsVector: {
    new(): WorkspaceMusicDiscsVector;
  };
  NamespaceOverride: {
    new(): NamespaceOverride;
    new(_0: EmbindString): NamespaceOverride;
  };
  Wiwiworkspace: {
    new(): Wiwiworkspace;
  };
  MinecraftPacker: {
    new(): MinecraftPacker;
  };
  InvisibleItemFramePack: {
    new(): InvisibleItemFramePack;
  };
  ImageData: {
    new(): ImageData;
    new(_0: number, _1: number): ImageData;
    new(_0: EmbindString): ImageData;
  };
}

export type MainModule = WasmModule & typeof RuntimeExports & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
