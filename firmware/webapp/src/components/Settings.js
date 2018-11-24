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
          <label for="wifi_ssid">WiFi SSID</label>
          <input id="wifi_ssid"
                 type="text"
                 placeholder="WiFi SSID"
                 value={state.settings.wifi_ssid}
                 oninput={e => actions.settings.update({ wifi_ssid: e.target.value }) }
          />
          </div>

          <div class="pure-control-group">
            <label for="wifi_pass">WiFi password</label>
            <input id="wifi_pass"
                   type="text"
                   placeholder="WiFi password"
                   value={state.settings.wifi_pass}
                   oninput={e => actions.settings.update({ wifi_pass: e.target.value }) }
            />
          </div>

          <div class="pure-controls">
            <a class="pure-button pure-button-primary" onclick={() => actions.settings.save()}>Save</a>
          </div>
        </fieldset>
      </form>

    </div>
  </div>
