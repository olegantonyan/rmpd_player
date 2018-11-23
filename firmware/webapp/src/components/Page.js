import { h } from "hyperapp"

import Container from "./Container"

export default (state, actions) =>
  <div id="layout" class={ state.nav_menu_open ? "active" : "" }>
    <a href="javascript:void(0);" id="menu_link" class="menu-link" onclick={() => actions.nav_menu_toggle(!state.nav_menu_open)}><span></span></a>

    <div id="menu">
      <div class="pure-menu">
        <ul class="pure-menu-list">
          <li class={ state.nav_current_route === "Home" ? "pure-menu-item pure-menu-selected" : "pure-menu-item"}>
            <a href="javascript:void(0);" class="pure-menu-link" onclick={() => actions.nav_route_to("Home")}>Home</a>
          </li>

          <li class={ state.nav_current_route === "Settings" ? "pure-menu-item pure-menu-selected" : "pure-menu-item"}>
            <a href="javascript:void(0);" class="pure-menu-link" onclick={() => actions.nav_route_to("Settings")}>Settings</a>
          </li>

          <li class={ state.nav_current_route === "System" ? "pure-menu-item pure-menu-selected" : "pure-menu-item"}>
            <a href="javascript:void(0);" class="pure-menu-link" onclick={() => actions.nav_route_to("System")}>System</a>
          </li>
        </ul>
      </div>
    </div>

    <div class="header">
      <h1>{state.nav_current_route}</h1>
    </div>

    <div id="main">
      <div class="content">
        <Container state={state} actions={actions} />
      </div>
    </div>
  </div>
