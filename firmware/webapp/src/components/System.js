import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="system">
    <div class="header">
      <h1>System</h1>
    </div>

    <div class="content">
      <button class="pure-button pure-button-primary">Reboot</button>
    </div>
  </div>
