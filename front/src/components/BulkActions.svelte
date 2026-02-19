<script>
  import {
    SquareIcon,
    RotateCcwIcon,
    RotateCwIcon,
    UserIcon,
    TypeIcon,
    Trash2Icon,
  } from "svelte-feather-icons";
  import { paintingsStore } from "../stores/paintingsStore";

  $: hasSelected = $paintingsStore.some((painting) => painting.selected);

  function rotateSelectedCCW() {
    $paintingsStore.forEach((painting) => {
      if (painting.selected) {
        painting.cppPainting.rotateAnticlockwise();
      }
    });
    paintingsStore.update((paintings) => paintings);
  }

  function rotateSelectedCW() {
    $paintingsStore.forEach((painting) => {
      if (painting.selected) {
        painting.cppPainting.rotateClockwise();
      }
    });
    paintingsStore.update((paintings) => paintings);
  }

  function deleteSelected() {
    paintingsStore.update((paintings) =>
      paintings.filter((painting) => {
        let shouldDelete = painting.selected;
        if (shouldDelete) painting.cppPainting.delete();
        return !shouldDelete;
      }),
    );
  }
</script>

{#if hasSelected}
  <div id="bulk-tools">
    <button> <SquareIcon /> </button>
    <button onclick={rotateSelectedCCW}> <RotateCcwIcon /> </button>
    <button onclick={rotateSelectedCW}> <RotateCwIcon /> </button>
    <button> <UserIcon /> </button>
    <button> <TypeIcon /> </button>
    <button class="delete" onclick={deleteSelected}> <Trash2Icon /> </button>
  </div>
{/if}

<style>
  #bulk-tools {
    position: sticky;
    bottom: 5px;
    gap: 5px;
    display: flex;
    width: 46px;
    flex-direction: column;
    padding: 5px;
    justify-content: center;
    align-items: center;
    box-shadow: 0px 4px 10px rgba(0, 0, 0, 0.5);
  }

  @media only screen and (min-width: 1200px) {
    #bulk-tools {
      transform: translate(-73px, -10px);
    }
  }
  #bulk-tools > button {
    height: 46px;
    width: 46px;
    padding: 0.5em;
    line-height: 0.9;
    align-items: center;
    vertical-align: center;
  }

  .delete {
    background-color: #ffcccc;
    border: 2px solid #ffaaaa;
  }
  .delete:hover {
    border-color: #ff5555;
  }

  @media (prefers-color-scheme: dark) {
    .delete {
      background-color: #e21f1f;
      color: #f4d9d9;
      border: 2px solid #f4d9d9;
    }
  }
</style>
