<script lang="ts">
  import type { TitleBarProps } from "./titleBarProps";

  import MacTitleBar from "./MacTitleBar.svelte";
  import WindowsTitleBar from "./WindowsTitleBar.svelte";

  import { pageTitle } from "../../stores/titleStore";
  import { forceWindowsTitlebar, getWindowType } from "./titleBar";

  let props: TitleBarProps = $props();
  let title: string | undefined = $derived(
    "Wiwitool · " + (props.title || $pageTitle),
  );

  let type = getWindowType();
</script>

{#if type === "windows"}
  <WindowsTitleBar {...props} {title} />
{:else if type === "mac"}
  <MacTitleBar {...props} {title} />
{:else if type === "web" || type === "unknown"}
  <div class="safe-space"></div>
{/if}

<style>
  .safe-space {
    height: 10px;
    margin: 0;
    padding: 0;
  }
</style>
