<script lang="ts">
  import { TriangleAlert } from "@lucide/svelte";
  import Button from "../components/Button.svelte";

  export let move: (id: string) => void;
</script>

<div class="app-card">
  <Button onclick={() => move("start")} icon="Home">Go back</Button>

  <div class="changelog">
    <p class="your-version">
      So glad to know you are using this tool. <br />
      Wiwitool <b>version {__GIT_TAG__}</b>, built from commit {__GIT_COMMIT__}.
    </p>

    <div class="version-header">
      <h2>v0.3.0</h2>
      <time datetime="2026-03-29">March 28, 2026</time>
    </div>

    <p>
      This update focuses heavily on Quality-of-Life improvements, a refreshed
      user interface, and under-the-hood optimisations. Music discs now support
      bulk management and comparator output customisation, while paintings allow
      you to toggle the frame on and off.
    </p>

    <dl>
      <dt>Update: Music discs</dt>
      <dd>
        <ul>
          <li>
            <b>Bulk management</b>: Implemented bulk actions allowing you to
            easily select, deselect, delete, and change the artist, comparator
            output and silence gap of multiple music discs at once.
          </li>
          <li>
            <b>Comparator outputs</b>: You can now manually set or randomise the
            redstone power emitted by a comparator when a music disc is playing
            in a jukebox.
          </li>
          <li>
            <b>Card interactions</b>: Added the ability to delete individual
            music disc cards.
          </li>
          <li>
            <b>Visuals</b>: Applied stylistic improvements to the music disc
            interface
          </li>
        </ul>
      </dd>

      <dt>Update: Paintings</dt>
      <dd>
        <ul>
          <li>
            <b>Frame toggling</b>: The procedurally generated painting frames
            can now be disabled entirely if you prefer borderless artwork.
          </li>
          <li>
            <b>Quality bump</b>: Reduced the downscaling amount applied to
            uploaded images to preserve better visual quality. This is done
            because in the future cropping will be available.
          </li>
          <li>
            <b>Safety checks</b>: Added a confirmation prompt when deleting a
            painting to prevent accidental data loss.
          </li>
          <li>
            <b>Guidance</b>: Added descriptive text to help guide new users
            through the paintings tool.
          </li>
        </ul>
      </dd>

      <dt>Interface & UX improvements</dt>
      <dd>
        <ul>
          <li>
            <b>Refreshed navigation</b>: Switched to a new tab bar style with
            rounded corners and a subtle blur gradient.
          </li>
          <li>
            <b>Selection counters</b>: Added counters to indicate how many
            paintings and discs are selected.
          </li>
          <li>
            <b>Button styling</b>: Important buttons now feature an animated
            moving border to highlight their presence.
          </li>
          <li>
            <b>Drag & drop:</b> Now they look better, having a consistent and pretty
            UI.
          </li>
        </ul>
      </dd>

      <dt>Performance & bug fixes</dt>
      <dd>
        <ul>
          <li>
            <b>Memory optimisations</b>: The tool now avoids unnecessary memory
            copying for paintings, music discs and all images, and aggressively
            frees up browser resources once a pack is generated.
          </li>
        </ul>
      </dd>

      <dt>
        Incompatibilities
        <TriangleAlert />
      </dt>
      <dd>
        Importing a pack from any older version will <b>not</b> work. The Wiwitool
        still needs to mature until it is in a state of file format stability. For
        now we are focused on bringing new features, maintaining version-to-version
        file compatibility will be a priority at a later date.
      </dd>
    </dl>

    <div class="version-header">
      <h2>v0.2</h2>
      <time datetime="2026-03-15">March 15, 2026</time>
    </div>

    <p>
      Custom music discs are now available. Pack generation and import is now
      much faster. Now, paintings use procedurally generated frames.
    </p>

    <dl>
      <dt>New feature: Music discs</dt>
      <dd>
        You can load your own songs and customise them:
        <ul>
          <li>
            When you drop an audio file, it appears as a card where you can
            listen to it.
          </li>
          <li>Title and author / description can be changed</li>
          <li>The audio can optionally be trimmed</li>
          <li>
            You can drag and drop an image to be used as a custom cover for the
            disc.
          </li>
        </ul>
      </dd>

      <dt>Update: Paintings</dt>
      <dd>
        <ul>
          <li>
            Paintings now have a custom procedurally generated frame using
            Perlin noise.
          </li>
          <li>The colour of the frame is customisable.</li>
          <li>
            The frame texture (the seed) can be changed to make the frame, and
            overall painting, look slightly different.
          </li>
        </ul>
      </dd>

      <dt>Other improvements</dt>
      <dd>
        <ul>
          <li>Serialisation and import is now fully done in C++.</li>
          <li>The interface has seen improvements, mostly internally.</li>
        </ul>
      </dd>

      <dt>
        Incompatibilities
        <TriangleAlert />
      </dt>
      <dd>
        Importing a pack created using v0.1 will not work due to the change in
        file format.
      </dd>
    </dl>

    <div class="version-header">
      <h2>v0.1</h2>
      <time datetime="2026-02-22">February 22, 2026</time>
    </div>

    <p>
      Initial public release. This version of the Wiwitool includes generation
      for custom custom painting frames and invisible item frames. All of this
      locally in your browser.
    </p>

    <dl>
      <dt>Painting generation</dt>
      <dd>
        <p>
          You can generate custom paintings that can be imported into Minecraft
          and back into this tool. All these features regarding paintings can be
          configured:
        </p>

        <ul>
          <li>Title and author</li>
          <li>Aspect ratio</li>
          <li>Image orientation</li>
        </ul>

        <p>
          Furthermore, the pack that you generate can be imported back into this
          tool to modify and expand it.
        </p>

        <p>
          With the bulk tools you can perform the same action to multiple
          paintings.
        </p>
      </dd>

      <dt>Invisible item frames</dt>
      <dd>
        Items that this pack can add:
        <ul>
          <li>Invisible item frames</li>
          <li>Glowing invisible item frames</li>
        </ul>
      </dd>
    </dl>
  </div>
</div>

<style>
  .changelog {
    width: 100%;
    max-width: 680px;
    margin: 50px auto;
  }

  .your-version {
    font-size: 110%;
  }

  .version-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
  }

  .version-header time {
    color: #646cff;
  }

  dl {
    padding-left: 1em;
  }

  dt {
    font-weight: 500;
    display: flex;
    align-items: center;
    gap: 10px;
  }

  dd {
    margin-bottom: 10px;
  }

  dd > *:first-child {
    margin-top: 0;
  }

  dd > *:last-child {
    margin-bottom: 0;
  }
</style>
