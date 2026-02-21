import { paintingsStore, type PaintingWrapper } from "../stores/paintingsStore";
import { invisibleItemFramesPack } from "../stores/invisibleItemFrameStore";
import type { SerialisationModel } from "./models";
import { deserialisePainting } from "./painting";
import { type MainModule } from "../bindings/wiwitool";

export function deserialise(model: SerialisationModel, module: MainModule) {
  paintingsStore.set([]);

  // Deserialise invisible item frames toggle
  invisibleItemFramesPack.set(model.invisibleItemFrames);

  // Deserialise paintings
  const wrappers: PaintingWrapper[] = model.paintings.map((paintingModel) =>
    deserialisePainting(module, paintingModel),
  );

  paintingsStore.set(wrappers);
}
