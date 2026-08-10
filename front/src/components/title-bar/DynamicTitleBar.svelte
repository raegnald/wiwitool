<script lang="ts">
  import type { TitleBarProps } from "./titleBarProps";

  import MacTitleBar from "./MacTitleBar.svelte";
  import WindowsTitleBar from "./WindowsTitleBar.svelte";

  let props: TitleBarProps = $props();

  let isMac = $state(false);

  if (typeof window !== "undefined") {
    const platform = (navigator.platform || "").toUpperCase();
    const userAgent = navigator.userAgent || "";
    isMac = platform.includes("MAC") || /Macintosh|Mac OS X/i.test(userAgent);
  }
</script>

{#if isMac}
  <MacTitleBar {...props} />
{:else}
  <!-- Default -->
  <WindowsTitleBar {...props} />
{/if}
