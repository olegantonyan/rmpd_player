import { h } from "hyperapp"

import Container from "./Container"
import Footer from "./Footer"
import Menu from "./Menu"
import { HOME, SYSTEM, SETTINGS } from "../routes"

export default (state, actions) =>
  <div id="layout" class={ state.nav_menu_open ? "active" : "" }>
    <Menu state={state} actions={actions} />

    <Container state={state} actions={actions} />

    <Footer />
  </div>
