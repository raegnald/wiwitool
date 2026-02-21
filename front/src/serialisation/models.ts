export type PaintingModel = {
  id: number;
  sourceImage: string; // base64-encoded
  title: string;
  author: string;
  ratio: string;
};

export type SerialisationModel = {
  paintings: PaintingModel[];
  invisibleItemFrames: boolean;
};
