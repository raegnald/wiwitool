import type { SerialisationModel, PaintingModel } from "./models";
import type { PaintingWrapper } from "../stores/paintingsStore";
import type { MainModule } from "../bindings/wiwitool";

function bytesToBase64(bytes: Uint8Array): string {
  let binary = "";
  const len = bytes.byteLength;

  for (let i = 0; i < len; i++) binary += String.fromCharCode(bytes[i]);

  return btoa(binary);
}

function base64ToBytes(base64: string): Uint8Array {
  const binaryStr = atob(base64);
  const len = binaryStr.length;
  const bytes = new Uint8Array(len);

  for (let i = 0; i < len; i++) bytes[i] = binaryStr.charCodeAt(i);

  return bytes;
}

export function serialisePainting(
  model: SerialisationModel,
  painting: PaintingWrapper,
) {
  const originalImageData = painting.cppPainting.originalData();
  const pngBytes = originalImageData.encodePng();
  originalImageData.delete();

  model.paintings.push({
    id: painting.id,
    sourceImage: bytesToBase64(pngBytes),
    title: painting.title,
    author: painting.author,
    ratio: painting.cppPainting.ratio,
  });
}

export function deserialisePainting(
  module: MainModule,
  paintingModel: PaintingModel,
): PaintingWrapper {
  const vec = new module.PaintingBufferVector();
  const imageBytes = base64ToBytes(paintingModel.sourceImage);

  for (let i = 0; i < imageBytes.length; i++) vec.push_back(imageBytes[i]);

  const cppPainting = new module.Painting(vec);
  vec.delete();

  cppPainting.title = paintingModel.title;
  cppPainting.author = paintingModel.author;
  cppPainting.ratio = paintingModel.ratio;

  const painting: PaintingWrapper = {
    id: paintingModel.id,
    title: paintingModel.title,
    author: paintingModel.author,
    originalImageBytes: imageBytes,
    cppPainting: cppPainting,
    selected: false,
    crop: null,
  };

  console.log(`Deserialised painting: ${painting.title}`);

  return painting;
}
