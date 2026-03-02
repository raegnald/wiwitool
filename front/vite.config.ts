import { defineConfig } from "vite";
import { svelte } from "@sveltejs/vite-plugin-svelte";
import { execSync } from "child_process";

const commitHash = execSync("git rev-parse --short HEAD").toString().trim();

let gitTag = "";
try {
  gitTag = execSync("git describe --tags --abbrev=0").toString().trim();
} catch (error) {
  gitTag = "v0.0.0";
}

export default defineConfig({
  plugins: [svelte()],
  server: {
    host: "0.0.0.0", // all addresses
    port: 8080,
    allowedHosts: ["xulo.local", "suspi.gligan.net"],
  },
  define: {
    __GIT_COMMIT__: JSON.stringify(commitHash),
    __GIT_TAG__: JSON.stringify(gitTag),
  },
});
