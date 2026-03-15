<script lang="ts">
  import ComparisonList, {
    No,
    Yes,
    Info,
    Showcase,
    Space,
  } from "../components/comparison-list/ComparisonList.svelte";

  import Button from "../components/Button.svelte";
  import Wiwicheckbox from "../components/Wiwicheckbox.svelte";

  let clickMeButtonDisabled = $state(false);

  let buttonToggle = $state(true);

  let loading = $state(false);

  function toggleSpin() {
    loading = !loading;
  }
</script>

<div id="main">
  <div class="app-card">
    <h2>Button showcase</h2>

    <ComparisonList>
      <Info>
        Buttons can be of <i>one</i> of these types:
        <code>primary</code>,
        <code>secondary</code>,
        <code>transparent</code>, or
        <code>destructive</code>. By default, buttons are <code>primary</code>.
        If a button is assigned multiple types at the same time, it will always
        be resolved using this preference:
        <code>destructive > primary > secondary > transparent</code>.
      </Info>

      <Showcase>
        <Button>Primario</Button>
        <Button secondary>Secundario</Button>
        <Button transparent>Transparente</Button>
        <Button destructive>Destructivo</Button>
      </Showcase>

      <Info>
        Buttons are implicitly <code>primary</code> and enabled by default. To
        disable a button, use the <code>disabled</code> property. Disabled
        buttons do not execute the <code>onclick</code> callback.
      </Info>

      <Showcase>
        <Button>Default button</Button>
        <Button disabled>Disabled</Button>
      </Showcase>

      <Info>Buttons can hold text, an icon, or both at the same time.</Info>

      <Showcase>
        <Button icon="Copy" />
        <Button secondary icon="Crop" />
        <Button transparent icon="BadgeQuestionMark" />
        <Button destructive icon="Trash2" />

        <div style="width: 100%"></div>

        <Button icon="ThumbsUp">Approve</Button>
        <Button secondary icon="Info">Information</Button>
        <Button transparent icon="BadgeQuestionMark">Help</Button>
        <Button destructive icon="BadgeX">Reset</Button>
      </Showcase>

      <Info>
        Button size can be changed with the attributes <code>large</code> or
        <code>small</code>.
      </Info>

      <Showcase>
        <Button large>Large button</Button>
        <Button>Default button</Button>
        <Button small>Small button</Button>

        <div style="width: 100%"></div>

        <Button large icon="Candy" />
        <Button icon="CandyCane" />
        <Button small icon="CandyOff" />

        <Button secondary large icon="Candy" />
        <Button secondary icon="CandyCane" />
        <Button secondary small icon="CandyOff" />

        <Button transparent large icon="Candy" />
        <Button transparent icon="CandyCane" />
        <Button transparent small icon="CandyOff" />

        <div style="width: 100%"></div>

        <Button large icon="Undo2">Undo</Button>
        <Button secondary icon="Redo2">Redo</Button>
        <Button small destructive icon="BrushCleaning">Clear</Button>
      </Showcase>

      <Info>
        Buttons can have the <code>grow</code> property to make them occupy 100% of
        their parent's width.
      </Info>

      <Showcase>
        <Button grow icon="MoveHorizontal">I am a growing button</Button>
      </Showcase>

      <Space />

      <Yes>Use buttons when an action must be performed.</Yes>

      <Showcase>
        <span>This button performs an action when clicked:</span>
        <Button
          large
          icon="MousePointer"
          disabled={clickMeButtonDisabled}
          onclick={() => alert("You touch my talala!")}
        >
          Click me
        </Button>

        <div style="width: 100%"></div>

        <Wiwicheckbox bind:checked={clickMeButtonDisabled}>
          Disable button above to prevent clicking
        </Wiwicheckbox>
      </Showcase>

      <No>
        Buttons do <i>not</i> hold state! Do not try to use them as a toggle.
      </No>

      <No>Buttons must not change themselves on click.</No>

      <Info>
        If, perhaps, you want a button that «changes» «state», you can use a
        reactive variable and the <code>hugeBorder</code> property. We use a
        reactive variable defined <i>outside</i> the button component, and
        assign the <code>hugeBorder</code> property and the <code>onclick</code> callback
        to that variable and a toggle function for that variable.
      </Info>

      <Showcase>
        <Button
          hugeBorder={buttonToggle}
          icon={buttonToggle ? "Check" : "X"}
          onclick={() => (buttonToggle = !buttonToggle)}
        >
          {buttonToggle ? "Activado" : "Desactivado"}
        </Button>

        <Button
          transparent
          hugeBorder={buttonToggle}
          icon={buttonToggle ? "Check" : "X"}
          onclick={() => (buttonToggle = !buttonToggle)}
        >
          {buttonToggle ? "Activado" : "Desactivado"}
        </Button>
      </Showcase>

      <Info>
        Buttons have the <code>loading</code> property, which disables the button
        and shows a spinner.
      </Info>

      <Showcase>
        <Button onclick={toggleSpin} {loading} icon="Send">Submit</Button>

        <Button onclick={toggleSpin} secondary {loading} icon="CloudSync">
          Save changes
        </Button>

        <Button onclick={toggleSpin} transparent {loading} icon="Activity">
          Check activity
        </Button>

        <Button onclick={toggleSpin} destructive {loading} icon="X" />

        <div style="width: 100%"></div>

        <Button onclick={toggleSpin}>Toggle loading state always</Button>
      </Showcase>
    </ComparisonList>
  </div>
  <!-- end buttons showcase -->

  <!-- <div class="app-card">other component showcase</div> -->
</div>

<style>
  #main {
    display: flex;
    flex-direction: column;
    gap: 20px;
  }

  h2 {
    margin: 0;
  }
</style>
