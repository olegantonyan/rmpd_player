import { h } from "hyperapp"

import Home from "./Home"
import System from "./System"
import Settings from "./Settings"

export default ({ state, actions }) =>
  state.nav_current_route === "Home" ? (
    <Home state={state} actions={actions} />
  ) : state.nav_current_route === "System" ? (
    <System state={state} actions={actions} />
  ) : state.nav_current_route === "Settings" ? (
    <Settings state={state} actions={actions} />
  ) : (
    <Home state={state} actions={actions} />
  )
