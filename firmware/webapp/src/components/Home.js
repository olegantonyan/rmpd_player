import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="home">
    <div class="header">
      <h1></h1>
    </div>

    <div class="content">
      <h2>{state.status.now_playing}</h2>
      <input disabled type="range" min="0" max="100" value={state.status.percent_pos} class="slider" id="percent-position" />

      <div class="slidecontainer">
        <label for="volume-control">Volume</label>
        <input type="range" min="1" max="100" value="100" class="slider" id="volume-control" />
      </div>
    </div>
  </div>
