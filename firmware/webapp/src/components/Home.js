import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="home">
    <div class="header">
      <h1></h1>
    </div>

    <div class="content">
      <div class="player-control">
        <h2>{state.status.now_playing}</h2>
        <input disabled type="range" min="0" max="100" value={state.status.percent_pos} class="slider" id="percent-position"/>

        <img src="skip_previous-24px.svg" class="centering" width="96" height="96" onclick={() => actions.home.prev()} />
        <img src="skip_next-24px.svg" class="centering" width="96" height="96" onclick={() => actions.home.next()} />
      </div>


      <br />
      <br />
      <br />

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
          <input type="range" min="0" max="100" step="5" value={state.audio.bass_frequency_limit} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ bass_frequency_limit: e.target.value })}/>
        </div>
        <div class="slidecontainer">
          <label for="bass-amplitude-control" class="centering">Bass amplitude </label>
          <input type="range" min="0" max="100" step="5" value={state.audio.bass_amplitude} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ bass_amplitude: e.target.value })}/>
        </div>
        <div class="slidecontainer">
          <label for="treble-freqlimit-control" class="centering">Treble frequency limit </label>
          <input type="range" min="0" max="100" step="5" value={state.audio.treble_frequency_limit} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ treble_frequency_limit: e.target.value })}/>
        </div>
        <div class="slidecontainer">
          <label for="treble-amplitude-control" class="centering">Treble amplitude </label>
          <input type="range" min="0" max="100" step="5" value={state.audio.treble_amplitude} class="slider" id="bass-freqlimit-control" class="centering" oninput={e => actions.audio.update({ treble_amplitude: e.target.value })}/>
        </div>

      </div>
    </div>
  </div>
