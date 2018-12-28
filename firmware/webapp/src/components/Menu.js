import { h } from "hyperapp"

import Container from "./Container"
import { HOME, SYSTEM, SETTINGS, AUDIO } from "../routes"

export default ({ state, actions }) =>
  <div>
    <a id="menu_link" class="menu-link" onclick={() => actions.nav_menu_toggle(!state.nav_menu_open)}><span></span></a>

    <div id="menu">
      <div class="pure-menu">
        <ul class="pure-menu-list">
          <li class={ state.nav_current_route === HOME ? "pure-menu-item pure-menu-selected" : "pure-menu-item"}>
            <a class="pure-menu-link" onclick={() => actions.nav_route_to(HOME)}>
              <img src="play_circle_filled-24px.svg" class="centering" />
              <span class="centering"> Playpack</span>
            </a>
          </li>

          <li class={ state.nav_current_route === SETTINGS ? "pure-menu-item pure-menu-selected" : "pure-menu-item"}>
            <a class="pure-menu-link" onclick={() => actions.nav_route_to(SETTINGS)}>
              <img src="settings_applications-24px.svg" class="centering" />
              <span class="centering"> Settings</span>
            </a>
          </li>

          <li class={ state.nav_current_route === SYSTEM ? "pure-menu-item pure-menu-selected" : "pure-menu-item"}>
            <a class="pure-menu-link" onclick={() => actions.nav_route_to(SYSTEM)}>
              <img src="memory-24px.svg" class="centering" />
              <span class="centering"> System</span>
            </a>
          </li>
        </ul>
      </div>

      <div id="volume-control-block">
        <label for="volume-control" class="centering">Volume</label>
        <img src="volume_up-24px.svg" class="centering" />
        <input type="range" class="vertical" orient="vertical" min="0" max="100" step="5" value={state.status.volume} class="slider" id="volume-control" class="centering" oninput={e => actions.status.update_volume(e.target.value)}/>
        <span>{state.status.volume}%</span>
      </div>
    </div>
  </div>
