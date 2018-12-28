import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="home">
    <br />

    <div class="content">
      <div class="player-control">
        <h2>{state.status.now_playing}</h2>
        <input disabled type="range" min="0" max="100" value={state.status.percent_pos} class="slider" id="percent-position"/>

        <div class="centering" onclick={() => actions.status.prev()}>
          <svg xmlns="http://www.w3.org/2000/svg" width="64" height="64" viewBox="0 0 24 24"><path d="M6 6h2v12H6zm3.5 6l8.5 6V6z"/><path d="M0 0h24v24H0z" fill="none"/></svg>
        </div>

        <div class="centering" onclick={() => actions.status.next()}>
          <svg xmlns="http://www.w3.org/2000/svg" width="64" height="64" viewBox="0 0 24 24"><path d="M6 18l8.5-6L6 6v12zM16 6v12h2V6h-2z"/><path d="M0 0h24v24H0z" fill="none"/></svg>
        </div>
      </div>

    </div>
  </div>
