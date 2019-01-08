import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="home">
    <br />

    <div class="content">
      <div class="player-control">
        <h2>{state.status.now_playing}</h2>
        <div>
          <input disabled type="range" min="0" max="100" value={state.status.percent_pos} class="slider" id="percent-position"/>
          <span>{seconds_to_hh_mm_ss(state.status.seconds_pos)}</span>
        </div>

        <div class="centering" onclick={() => actions.status.prev()}>
          <svg xmlns="http://www.w3.org/2000/svg" width="64" height="64" viewBox="0 0 24 24"><path d="M6 6h2v12H6zm3.5 6l8.5 6V6z"/><path d="M0 0h24v24H0z" fill="none"/></svg>
        </div>

        <div class="centering" onclick={() => actions.status.next()}>
          <svg xmlns="http://www.w3.org/2000/svg" width="64" height="64" viewBox="0 0 24 24"><path d="M6 18l8.5-6L6 6v12zM16 6v12h2V6h-2z"/><path d="M0 0h24v24H0z" fill="none"/></svg>
        </div>

        <div>
          <input type="checkbox" name="vehicle" checked={state.status.random} onclick={e => actions.status.set_random(e.target.checked)}/> Random
        </div>
      </div>

    </div>
  </div>

  function seconds_to_hh_mm_ss(seconds_total) {
    let hours = Math.floor(seconds_total / 3600);
    seconds_total %= 3600;
    let minutes = Math.floor(seconds_total / 60);
    let seconds = seconds_total % 60;

    minutes = String(minutes).padStart(2, "0");
    hours = String(hours).padStart(2, "0");
    seconds = String(seconds).padStart(2, "0");
    return hours + ":" + minutes + ":" + seconds;
  }
