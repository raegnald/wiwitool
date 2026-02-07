
export function downloadFromFS(
  path,
  filename = null,
  mime = "application/octet-stream"
) {
  const data = FS.readFile(path); // Uint8Array

  const blob = new Blob([data], { type: mime });
  const url = URL.createObjectURL(blob);

  const a = document.createElement("a");
  a.href = url;
  a.download = filename ?? path.split("/").pop();
  document.body.appendChild(a);
  a.click();

  document.body.removeChild(a);
  URL.revokeObjectURL(url);
}

// downloadFromFS('/datapack.zip');
