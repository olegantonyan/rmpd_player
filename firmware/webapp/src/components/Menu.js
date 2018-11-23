import { h } from "hyperapp"

import Container from "./Container"
import { HOME, SYSTEM, SETTINGS } from "../routes"

export default ({ state, actions }) =>
  <div>
    <a id="menu_link" class="menu-link" onclick={() => actions.nav_menu_toggle(!state.nav_menu_open)}><span></span></a>

    <div id="menu">
      <div class="pure-menu">
        <ul class="pure-menu-list">
          <li class={ state.nav_current_route === HOME ? "pure-menu-item pure-menu-selected" : "pure-menu-item"}>
            <a class="pure-menu-link" onclick={() => actions.nav_route_to(HOME)}>Home</a>
          </li>

          <li class={ state.nav_current_route === SETTINGS ? "pure-menu-item pure-menu-selected" : "pure-menu-item"}>
            <a class="pure-menu-link" onclick={() => actions.nav_route_to(SETTINGS)}>Settings</a>
          </li>

          <li class={ state.nav_current_route === SYSTEM ? "pure-menu-item pure-menu-selected" : "pure-menu-item"}>
            <a class="pure-menu-link" onclick={() => actions.nav_route_to(SYSTEM)}>System</a>
          </li>
        </ul>
      </div>
    </div>
  </div>
