import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="home">
    <div class="header">
      <h1>Player #1234</h1>
    </div>

    <div class="content">
      <h2>{state.status.now_playing} ({state.status.percent_pos}%)</h2>
    </div>
  </div>
