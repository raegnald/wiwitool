<script>
  import {
    CopyIcon,
    CropIcon,
    FolderInputIcon,
    ImageUpIcon,
    RotateCcwIcon,
    Trash2Icon,
  } from "@lucide/svelte";
</script>

<div>
  <h3>How to generate your paintings</h3>
  <ol>
    <li>
      <b>Import your files.</b>
      The first step is selecting which pictures you want to turn into paintings and
      importing the to the Wiwitool. For that, either drag and drop them into the
      images import zone (<ImageUpIcon size="1.1em" />) or select them from your
      computer using the file picker button (<FolderInputIcon size="1.1em" />).
    </li>
    <li>
      <b>Edit your souce images.</b>
      Now is when the fun begins! Once your images have loaded into the Wiwitool you
      can edit them to make the look even better in-game. On the left hand side of
      the painting card you have a few tools at your disposal: you can rotate the
      image (<RotateCcwIcon size="1.1em" />) or crop it (<CropIcon
        size="1.1em"
      />). If you want multiple crops of the same image you can clone it (<CopyIcon
        size="1.1em"
      />), and if you change your mind you can delete it (<Trash2Icon
        size="1.1em"
      />).
    </li>

    <li>
      <b>Edit your paintings.</b>
      Now its time to get into the minecrafty stuff, on the right of the painting
      card you have options to change the title (...) and author (...) of the painting
      in-game.
    </li>
  </ol>
  <p>
    This is a web tool for creating <b
      >custom painting items in Minecraft out of your own images</b
    >. You drag and drop all the images you want to have as Minecraft painting
    items, and this tool bundles them into two files:
    <code>datapack.zip</code>
    and <code>respack.zip</code>.
  </p>
  <table>
    <tbody>
      <tr>
        <td
          >You have a set of images that you want to turn into Minecraft
          paintings.</td
        >
        <td></td>
        <td>You customise your paintings with the Wiwitool.</td>
        <td></td>
        <td
          >The Wiwitool gives you two zip files that you can import into
          Minecraft.</td
        >
        <td></td>
        <td>You can use your own paintings in Minecraft.</td>
      </tr>
      <tr>
        <td><img src="/before.jpg" alt="Source picture" /></td>
        <td>You load <img src="/rightarrow.svg" alt="Arrow" /></td>
        <td
          ><img
            class="fill-space"
            src="/faviconcropped.png"
            alt="App icon"
          /></td
        >
        <td>Wiwitool generates <img src="/rightarrow.svg" alt="Arrow" /></td>
        <td
          ><img
            class="fill-space"
            src="/after.png"
            alt="Transformed image"
          /></td
        >
        <td>Import into Minecraft <img src="/rightarrow.svg" alt="Arrow" /></td>
        <td
          ><img
            src="/painting-in-minecraft.png"
            alt="Viewing the resulting painting in Minecraft"
          /></td
        >
      </tr>
      <tr>
        <td
          ><code>sunset.jpg</code>,
          <code>landscape.png</code>...</td
        >
        <td></td>
        <td><span class="jb9 wiwitool-text">The Wiwitool</span></td>
        <td></td>
        <td
          ><code>datapack.zip</code> &
          <code>respack.zip</code></td
        >
        <td></td>
        <td>Enjoy your paintings!</td>
      </tr>
    </tbody>
  </table>

  <p>
    The generated paintings can be crafted from a stonecutter. Simply add any
    painting to obtain any of your custom paintings.
  </p>

  <center>
    <img
      src="/stonecutter.png"
      alt="Transforming any painting into a custom painting"
    />
  </center>
</div>

<style>
  .fill-space {
    width: 100%;
    height: 100%;
    object-fit: cover;
  }
  .wiwitool-text {
    font-size: 150%;
  }

  p:first-child {
    margin-top: 0;
  }
</style>
