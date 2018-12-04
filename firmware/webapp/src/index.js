import { app } from 'hyperapp'

import actions from './actions'
import state from './state'
import view from './components/Layout'

const appArgs = [
  state,
  actions,
  view,
  document.getElementById('app'),
]

function onMount(main) {
  main.status.fetch()
  setInterval(() => { main.status.fetch() }, 1500)
  //setTimeout(sub, 2000);
}

let main = app(...appArgs)
onMount(main)
