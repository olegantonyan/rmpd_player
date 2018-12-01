import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="home">
    <div class="header">
      <h1>Device #1234</h1>
    </div>

    <div class="content">
      <p>Now playing: Dimmu Borgir - 51k</p>
      <p>wifi/server status, etc</p>
    </div>
  </div>
