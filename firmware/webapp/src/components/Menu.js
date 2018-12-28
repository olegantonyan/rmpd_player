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
              <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" class="centering"><path d="M0 0h24v24H0z" fill="none"/><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm-2 14.5v-9l6 4.5-6 4.5z"/></svg>
              <span class="centering"> Playpack</span>
            </a>
          </li>

          <li class={ state.nav_current_route === SETTINGS ? "pure-menu-item pure-menu-selected" : "pure-menu-item"}>
            <a class="pure-menu-link" onclick={() => actions.nav_route_to(SETTINGS)}>
              <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" class="centering"><path d="M0 0h24v24H0z" fill="none"/><path d="M12 10c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2zm7-7H5c-1.11 0-2 .9-2 2v14c0 1.1.89 2 2 2h14c1.11 0 2-.9 2-2V5c0-1.1-.89-2-2-2zm-1.75 9c0 .23-.02.46-.05.68l1.48 1.16c.13.11.17.3.08.45l-1.4 2.42c-.09.15-.27.21-.43.15l-1.74-.7c-.36.28-.76.51-1.18.69l-.26 1.85c-.03.17-.18.3-.35.3h-2.8c-.17 0-.32-.13-.35-.29l-.26-1.85c-.43-.18-.82-.41-1.18-.69l-1.74.7c-.16.06-.34 0-.43-.15l-1.4-2.42c-.09-.15-.05-.34.08-.45l1.48-1.16c-.03-.23-.05-.46-.05-.69 0-.23.02-.46.05-.68l-1.48-1.16c-.13-.11-.17-.3-.08-.45l1.4-2.42c.09-.15.27-.21.43-.15l1.74.7c.36-.28.76-.51 1.18-.69l.26-1.85c.03-.17.18-.3.35-.3h2.8c.17 0 .32.13.35.29l.26 1.85c.43.18.82.41 1.18.69l1.74-.7c.16-.06.34 0 .43.15l1.4 2.42c.09.15.05.34-.08.45l-1.48 1.16c.03.23.05.46.05.69z"/></svg>
              <span class="centering"> Settings</span>
            </a>
          </li>

          <li class={ state.nav_current_route === SYSTEM ? "pure-menu-item pure-menu-selected" : "pure-menu-item"}>
            <a class="pure-menu-link" onclick={() => actions.nav_route_to(SYSTEM)}>
              <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" class="centering"><path d="M0 0h24v24H0z" fill="none"/><path d="M15 9H9v6h6V9zm-2 4h-2v-2h2v2zm8-2V9h-2V7c0-1.1-.9-2-2-2h-2V3h-2v2h-2V3H9v2H7c-1.1 0-2 .9-2 2v2H3v2h2v2H3v2h2v2c0 1.1.9 2 2 2h2v2h2v-2h2v2h2v-2h2c1.1 0 2-.9 2-2v-2h2v-2h-2v-2h2zm-4 6H7V7h10v10z"/></svg>
              <span class="centering"> System</span>
            </a>
          </li>
        </ul>
      </div>

      <div id="volume-control-block">
        <label for="volume-control" class="centering">Volume</label>
        <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" class="centering"><path d="M3 9v6h4l5 5V4L7 9H3zm13.5 3c0-1.77-1.02-3.29-2.5-4.03v8.05c1.48-.73 2.5-2.25 2.5-4.02zM14 3.23v2.06c2.89.86 5 3.54 5 6.71s-2.11 5.85-5 6.71v2.06c4.01-.91 7-4.49 7-8.77s-2.99-7.86-7-8.77z"/><path d="M0 0h24v24H0z" fill="none"/></svg>
        <input type="range" class="vertical" orient="vertical" min="0" max="100" step="5" value={state.status.volume} class="slider" id="volume-control" class="centering" oninput={e => actions.status.update_volume(e.target.value)}/>
        <span>{state.status.volume}%</span>
      </div>
    </div>
  </div>
