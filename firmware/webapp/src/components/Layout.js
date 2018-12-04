import { h } from "hyperapp"

import Container from "./Container"
import Footer from "./Footer"
import Menu from "./Menu"

export default (state, actions) =>
  <div id="layout" class={ state.nav_menu_open ? "active" : "" }>
    <Menu state={state} actions={actions} />

    <Container state={state} actions={actions} />

    <Footer state={state} actions={actions} />
  </div>
