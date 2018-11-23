import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="settings">
    <div class="header">
      <h1>Settings</h1>
    </div>

    <div class="content">

      <form class="pure-form pure-form-aligned">
        <fieldset>
          <div class="pure-control-group">
          <label for="name">WiFi network</label>
          <input id="name" type="text" placeholder="Username" />
          </div>

          <div class="pure-control-group">
            <label for="password">WiFi password</label>
            <input id="password" type="password" placeholder="Password" />
          </div>

          <div class="pure-controls">
            <span class="pure-button pure-button-primary" onclick={() => actions.nav_menu_toggle(!state.nav_menu_open)}>Save</span>
          </div>
        </fieldset>
      </form>

    </div>
  </div>
