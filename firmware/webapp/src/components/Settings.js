import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="settings">
    <br />

    <div class="content">

      <h3>Audio</h3>
      <div class="audio-control">
        <form class="pure-form pure-form-aligned">
          <fieldset>
            <div class="pure-control-group">
              <label for="bass-freqlimit-control" class="centering">Bass frequency limit </label>
              <input type="range" min="2" max="15" step="1" value={state.audio.bass_frequency_limit} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ bass_frequency_limit: Number(e.target.value) })}/>
              <span>{state.audio.bass_frequency_limit * 10} Hz</span>
            </div>

            <div class="pure-control-group">
              <label for="bass-amplitude-control" class="centering">Bass amplitude </label>
              <input type="range" min="0" max="15" step="1" value={state.audio.bass_amplitude} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ bass_amplitude: Number(e.target.value) })}/>
              <span>{state.audio.bass_amplitude} dB</span>
            </div>

            <div class="pure-control-group">
              <label for="treble-freqlimit-control" class="centering">Treble frequency limit </label>
              <input type="range" min="0" max="15" step="1" value={state.audio.treble_frequency_limit} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ treble_frequency_limit: Number(e.target.value) })}/>
              <span>{state.audio.treble_frequency_limit} KHz</span>
            </div>

            <div class="pure-control-group">
              <label for="treble-amplitude-control" class="centering">Treble amplitude </label>
              <input type="range" min="-8" max="7" step="1" value={state.audio.treble_amplitude} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ treble_amplitude: Number(e.target.value) })}/>
              <span>{state.audio.treble_amplitude * 1.5} dB</span>
            </div>
          </fieldset>
        </form>
      </div>

      <h3>Wi-Fi</h3>
      <form class="pure-form pure-form-aligned">
        <fieldset>
          <div class="pure-control-group">
          <label for="wifi_ssid">SSID</label>
          <input id="wifi_ssid"
                 type="text"
                 placeholder=""
                 value={state.settings.wifi_ssid}
                 oninput={e => actions.settings.update({ wifi_ssid: e.target.value }) }
          />
          </div>

          <div class="pure-control-group">
            <label for="wifi_pass">Password</label>
            <input id="wifi_pass"
                   type="password"
                   placeholder=""
                   value={state.settings.wifi_pass}
                   oninput={e => actions.settings.update({ wifi_pass: e.target.value }) }
            />
          </div>

          <div class="pure-controls">
            <a class="pure-button pure-button-primary" onclick={() => actions.settings.save()} disabled={state.settings.saving}>
              {state.settings.saving ? 'Saving...' : 'Save'}
            </a>
            <i> DHCP will be used to obtain IP address/gateway</i>
          </div>
        </fieldset>
      </form>

      <h3>Time & Date</h3>
      <form class="pure-form pure-form-aligned">
        <fieldset>
          <div class="pure-control-group">
            <label for="timezone-select">Timezone</label>
            <select id="timezone-select" onchange={e => actions.settings.update({ timezone: e.target.value }) && actions.settings.save()}>
              {
                Object.keys(state.settings.all_timezones).map(key => (<option value={key} selected={key == state.settings.timezone}>{key}</option>))
              }
            </select>
          </div>
        </fieldset>
      </form>

    </div>
  </div>
