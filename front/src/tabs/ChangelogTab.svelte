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
