import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="system">
    <br />

    <div class="content">
      <button class="pure-button pure-button-primary" onclick={() => actions.system.reboot()}>Reboot</button>
    </div>
  </div>
