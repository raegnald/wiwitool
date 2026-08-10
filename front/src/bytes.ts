export async function toBytes(
  fileOrData: Uint8Array | File | string,
): Promise<Uint8Array> {
  if (fileOrData instanceof Uint8Array) {
    return fileOrData;
  }

  if (fileOrData instanceof File) {
    return new Uint8Array(await fileOrData.arrayBuffer());
  }

  return new TextEncoder().encode(fileOrData);
}
