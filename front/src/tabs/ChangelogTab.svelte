<script lang="ts">
  import Button from "../components/Button.svelte";

  export let move: (id: string) => void;
</script>

<div class="app-card">
  <Button onclick={() => move("start")} icon="Home">Go back</Button>

  <p>
    You are running Wiwitool version {__GIT_TAG__} (commit {__GIT_COMMIT__})
  </p>

  <h2>v0.2</h2>

  <p>
    This version builds up on v0.1 and tries to make it better. The pack
    generation and import is now much faster. Also, there is a new option to use
    procedurally generated frames.
  </p>

  <dl>
    <dt>Updates on painting generation</dt>
    <dd>
      <ul>
        <li>Painting serialisation and import is now fully done in C++.</li>
        <li>
          Paintings can now have a custom procedurally generated frame using
          Perlin noise.
        </li>
      </ul>
    </dd>

    <dt>Other improvements</dt>
    <dd>
      <ul>
        <li>The interface has seen improvements internally.</li>
      </ul>
    </dd>
  </dl>

  <h2>v0.1</h2>

  <p>
    Initial public release. This version of the Wiwitool includes generation for
    custom custom painting frames and invisible item frames. All of this locally
    in your browser.
  </p>

  <dl>
    <dt>Painting generation</dt>
    <dd>
      <p>
        You can generate custom paintings that can be imported into Minecraft
        and back into this tool. All these things regarding paintings can be
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

<style>
  dl {
    padding-left: 1em;
  }

  dt {
    font-weight: 500;
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
