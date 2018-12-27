import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="settings">
    <div class="header">
      <h1>Settings</h1>
    </div>

    <div class="content">

      <div class="audio-control">
        <div class="slidecontainer">
          <label for="volume-control" class="centering">Volume </label>
          <img src="volume_mute-24px.svg" class="centering" />
          <input type="range" min="0" max="100" step="5" value={state.status.volume} class="slider" id="volume-control" class="centering" oninput={e => actions.status.update_volume(e.target.value)}/>
          <img src="volume_up-24px.svg" class="centering" />
          <span>{state.status.volume}%</span>
        </div>

        <div class="slidecontainer">
          <label for="bass-freqlimit-control" class="centering">Bass frequency limit </label>
          <input type="range" min="2" max="15" step="1" value={state.audio.bass_frequency_limit} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ bass_frequency_limit: Number(e.target.value) })}/>
          <span>{state.audio.bass_frequency_limit * 10} Hz</span>
        </div>
        <div class="slidecontainer">
          <label for="bass-amplitude-control" class="centering">Bass amplitude </label>
          <input type="range" min="0" max="15" step="1" value={state.audio.bass_amplitude} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ bass_amplitude: Number(e.target.value) })}/>
          <span>{state.audio.bass_amplitude} dB</span>
        </div>
        <div class="slidecontainer">
          <label for="treble-freqlimit-control" class="centering">Treble frequency limit </label>
          <input type="range" min="0" max="15" step="1" value={state.audio.treble_frequency_limit} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ treble_frequency_limit: Number(e.target.value) })}/>
          <span>{state.audio.treble_frequency_limit} KHz</span>
        </div>
        <div class="slidecontainer">
          <label for="treble-amplitude-control" class="centering">Treble amplitude </label>
          <input type="range" min="-8" max="7" step="1" value={state.audio.treble_amplitude} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ treble_amplitude: Number(e.target.value) })}/>
          <span>{state.audio.treble_amplitude * 1.5} dB</span>
        </div>
      </div>

      <form class="pure-form pure-form-aligned">
        <fieldset>
          <div class="pure-control-group">
          <label for="wifi_ssid">WiFi SSID</label>
          <input id="wifi_ssid"
                 type="text"
                 placeholder=""
                 value={state.settings.wifi_ssid}
                 oninput={e => actions.settings.update({ wifi_ssid: e.target.value }) }
          />
          </div>

          <div class="pure-control-group">
            <label for="wifi_pass">WiFi password</label>
            <input id="wifi_pass"
                   type="text"
                   placeholder=""
                   value={state.settings.wifi_pass}
                   oninput={e => actions.settings.update({ wifi_pass: e.target.value }) }
            />
          </div>

          <div class="pure-controls">
            <a class="pure-button pure-button-primary" onclick={() => actions.settings.save()} disabled={state.settings.saving}>
              {state.settings.saving ? 'Saving...' : 'Save'}
            </a>
          </div>
        </fieldset>
      </form>

    </div>
  </div>
