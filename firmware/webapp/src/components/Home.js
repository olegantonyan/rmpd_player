import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="home">
    <br />

    <div class="content">
      <div class="player-control">
        <h2>{state.status.now_playing}</h2>
        <input disabled type="range" min="0" max="100" value={state.status.percent_pos} class="slider" id="percent-position"/>

        <img src="skip_previous-24px.svg" class="centering" width="96" height="96" onclick={() => actions.status.prev()} />
        <img src="skip_next-24px.svg" class="centering" width="96" height="96" onclick={() => actions.status.next()} />
      </div>

    </div>
  </div>
