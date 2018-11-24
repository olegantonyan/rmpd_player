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
  //const { add, sub, } = main;

  /**
   * Hyperapp wires your actions so the view is re-rendered every time the state
   * changes as a result of calling any action. This object is useful because it
   * allows you to talk to your app from another app, framework, vanilla JS, etc.
   *
   * Here is an example on CodePen: https://codepen.io/selfup/pen/jLMRjO
   */

   //main.fetch_settings()

  //setInterval(() => { main.fetch_settings() }, 1000)
  //setTimeout(sub, 2000);

}

let main = app(...appArgs)
onMount(main)
