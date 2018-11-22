import { h, app } from "hyperapp"

const state = {
  count: 0,

  nav_menu_open: false,
  nav_active_status: true,
  nav_active_settings: false,
  nav_active_system: false
}

const actions = {
  down: value => state => ({ count: state.count - value }),
  up: value => state => ({ count: state.count + value }),

  nav_menu_toggle: value => state => ({ nav_menu_open: value })
}

const view = (state, actions) => (
  <div id="layout" class={ state.nav_menu_open ? "active" : "" }>
    <a href="javascript:void(0);" id="menu_link" class="menu-link" onclick={() => actions.nav_menu_toggle(!state.nav_menu_open)}><span></span></a>

    <div id="menu">
      <div class="pure-menu">
        <ul class="pure-menu-list">
          <li class={ state.nav_active_status ? 'pure-menu-item pure-menu-selected' : 'pure-menu-item'}><a href="javascript:void(0);" class="pure-menu-link">Status</a></li>
          <li class={ state.nav_active_settings ? 'pure-menu-item pure-menu-selected' : 'pure-menu-item'}><a href="javascript:void(0);" class="pure-menu-link">Settings</a></li>
          <li class={ state.nav_active_system ? 'pure-menu-item pure-menu-selected' : 'pure-menu-item'}><a href="javascript:void(0);" class="pure-menu-link">System</a></li>
        </ul>
      </div>
    </div>

    <div class="header">
      <h1>Page Title</h1>
      <h2>A subtitle for your page goes here</h2>
    </div>

    <div id="main">
      <div class="content">
        <h1>{state.count}</h1>
        <button class="pure-button pure-button-primary" onclick={() => actions.down(1)}>-</button>
        <button class="pure-button pure-button-primary" onclick={() => actions.up(1)}>+</button>
      </div>
    </div>
  </div>
)

window.addEventListener("load", function() {
  app(state, actions, view, document.body)
});
