import '../styles/app.css'
import '../styles/loader.css'
import '../styles/side-menu.css'
import '../node_modules/purecss/build/pure-min.css'

import { app } from 'hyperapp'

import actions from './actions'
import state from './state'
import view from './components/Layout'

function onMount(main) {
  main.status.fetch()
}

let main = app(state, actions, view, document.getElementById('app'))
onMount(main)
