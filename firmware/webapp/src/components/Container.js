import { h } from "hyperapp"

import { HOME, SYSTEM, SETTINGS } from "../routes"
import Home from "./Home"
import System from "./System"
import Settings from "./Settings"

const LoadComponent = (state, actions) => {
  switch(state.nav_current_route) {
    case HOME:
      return <Home state={state} actions={actions} />
    case SYSTEM:
      return <System state={state} actions={actions} />
    case SETTINGS:
      return <Settings state={state} actions={actions} />
    default:
      return <Home state={state} actions={actions} />
  }
}

export default ({ state, actions }) =>
  <div id="container">
    {LoadComponent(state, actions)}
  </div>
