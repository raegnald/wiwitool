<script>
  import { Link } from 'svelte-routing'

  import logo from '/logo.svg'
  import WiwiButton from '../lib/WiwiButton.svelte'
</script>

<main class="centered">
  <div>
    <img src={logo} class="logo" alt="The Wiwitool logo" />
  </div>

  <div>
    <Link to='/paintings'><WiwiButton text="Create new pack" /></Link>
    <WiwiButton text="Work with existing pack" disabled />
  </div>
</main>

<style>
  .logo {
    height: 6em;
    padding: 1.5em;
    will-change: filter;
    transition: filter 300ms;
  }
  .logo:hover {
    filter: drop-shadow(0 0 5em #978BE5);
  }
  .centered {
    display: inline-block;
    text-align: center;
    position: absolute;
    top: 50vh;
    left: 50vw;
    transform: translate(-50%, -50%);
  }
</style>
