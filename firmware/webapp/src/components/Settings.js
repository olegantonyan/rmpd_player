import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="settings">
    <br />

    <div class="content">

      <h3>Звук</h3>
      <div class="audio-control">
        <form class="pure-form pure-form-aligned">
          <fieldset>
            <div class="pure-control-group">
              <label for="bass-freqlimit-control" class="centering">НЧ лимит </label>
              <input type="range" min="2" max="15" step="1" value={state.audio.bass_frequency_limit} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ bass_frequency_limit: Number(e.target.value) })}/>
              <span>{state.audio.bass_frequency_limit * 10} Гц</span>
            </div>

            <div class="pure-control-group">
              <label for="bass-amplitude-control" class="centering">НЧ амплитуда </label>
              <input type="range" min="0" max="15" step="1" value={state.audio.bass_amplitude} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ bass_amplitude: Number(e.target.value) })}/>
              <span>{state.audio.bass_amplitude} дБ</span>
            </div>

            <div class="pure-control-group">
              <label for="treble-freqlimit-control" class="centering">ВЧ лимит </label>
              <input type="range" min="0" max="15" step="1" value={state.audio.treble_frequency_limit} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ treble_frequency_limit: Number(e.target.value) })}/>
              <span>{state.audio.treble_frequency_limit} КГц</span>
            </div>

            <div class="pure-control-group">
              <label for="treble-amplitude-control" class="centering">ВЧ амплитуда </label>
              <input type="range" min="-8" max="7" step="1" value={state.audio.treble_amplitude} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ treble_amplitude: Number(e.target.value) })}/>
              <span>{state.audio.treble_amplitude * 1.5} дБ</span>
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
            <label for="wifi_pass">Пароль</label>
            <input id="wifi_pass"
                   type="password"
                   placeholder=""
                   value={state.settings.wifi_pass}
                   oninput={e => actions.settings.update({ wifi_pass: e.target.value }) }
            />
          </div>

          <div class="pure-controls">
            <a class="pure-button pure-button-primary" onclick={() => actions.settings.save()} disabled={state.settings.saving}>
              {state.settings.saving ? 'Сохраняем...' : 'Сохранить'}
            </a>
            <em> будет использован DHCP</em>
          </div>
        </fieldset>
      </form>

      <h3>Время и дата</h3>
      <form class="pure-form pure-form-aligned">
        <fieldset>
          <div class="pure-control-group">
            <label for="timezone-select">Часовой пояс</label>
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
