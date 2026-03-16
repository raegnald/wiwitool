import { defineConfig } from "vite";
import { svelte } from "@sveltejs/vite-plugin-svelte";
import { execSync } from "child_process";

const commitHash = execSync("git rev-parse --short HEAD").toString().trim();

let gitTag = "";
try {
  gitTag = execSync("git describe --tags").toString().trim();
} catch (error) {
  gitTag = "v0.0.0";
}

export default defineConfig({
  plugins: [svelte()],
  optimizeDeps: {
    exclude: [],
  },
  server: {
    host: "0.0.0.0", // all addresses
    port: 8080,
    allowedHosts: ["xulo.local", "suspi.gligan.net"],
    headers: {
      "Cross-Origin-Opener-Policy": "same-origin",
      "Cross-Origin-Embedder-Policy": "require-corp",
    },
  },
  define: {
    __GIT_COMMIT__: JSON.stringify(commitHash),
    __GIT_TAG__: JSON.stringify(gitTag),
  },
});
