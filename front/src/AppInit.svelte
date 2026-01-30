<script>
  import { Router, Route, Link } from 'svelte-routing'

  import { onMount } from 'svelte';
  import { initWasm } from './wasmStore.js';

  import Start from './pages/Start.svelte'
  import PaintingsPane from './pages/PaintingsPane.svelte'

  onMount(() => initWasm());
</script>

<Router>
  <Route path="/">
    <Start />
  </Route>

  <Route path="/paintings">
    <PaintingsPane />
  </Route>
</Router>
