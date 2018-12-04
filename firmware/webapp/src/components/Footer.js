import { h } from "hyperapp"

export default ({ state, actions }) =>
  <footer class="footer">
    <p>Now playing: {state.status.now_playing}</p>
  </footer>
