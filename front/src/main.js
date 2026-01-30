import { mount } from 'svelte'
import './app.css'
import AppInit from './AppInit.svelte'

const app = mount(AppInit, {
  target: document.getElementById('app'),
})

export default app
