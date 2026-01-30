<script>
  import {Link} from 'svelte-routing'

  import logo from '/logo.svg'
</script>

<nav>
  <Link to='/'><img src={logo} class="logo" alt="The Wiwitool logo" /></Link>

  <div class="tabs">
    <Link to='/paintings'><span class="active-pane">Paintings</span></Link>
    <Link to='/music'><span>Music</span></Link>
    <Link to='/other'><span>Other</span></Link>
  </div>
</nav>

<style>
  nav {
    width: 100%;
    margin-bottom: 2em;
    display: flex;
    align-self: center;
    justify-content: space-between;
  }
  .logo {
    padding-top: 0.25em;
    max-width: 15em;
    transition: filter 300ms;
  }
  .logo:hover {
    filter: drop-shadow(0 0 2em #978BE5);
  }
  .tabs {
    --pill-height: 2.5em;         /* base height */
    height: var(--pill-height);
    background: rgba(0, 0, 0, 0.1);
    align-content: center;
    border-radius: calc(var(--pill-height) / 2);
  }
  .tabs span {
    display: inline-flex;
    align-items: center;
    height: 100%;
    padding-left: var(--pill-height);
    padding-right: var(--pill-height);
    border-radius: calc(var(--pill-height) / 2);
    color: #666;
    font-size: 20px;
  }
  .active-pane {
    background: #FFDD00;
    color: rgba(0, 0, 0, .8);
  }
</style>
