import { h } from "hyperapp"

export default ({ state, actions }) =>
  <footer class="footer">
    <p>Now playing: {state.status.now_playing} ({state.status.percent_pos}%) Time: {state.status.time}</p>
  </footer>
