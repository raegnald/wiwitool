import { paintingsStore, PaintingWrapper } from "../stores/paintingsStore";
import { invisibleItemFramesPack } from "../stores/invisibleItemFrameStore";
import { SerialisationModel } from "./models";
import { serialisePainting } from "./painting";
import { get } from "svelte/store";

export function serialise(): SerialisationModel {
  let model: SerialisationModel = {
    paintings: [],
    invisibleItemFrames: get(invisibleItemFramesPack),
  };

  get(paintingsStore).forEach((painting) => {
    serialisePainting(model as SerialisationModel, painting);
  });

  return model;
}
