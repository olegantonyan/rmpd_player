import { h } from 'hyperapp'

export default ({ state, actions }) =>
  <div id="system">
    <br />

    <div class="content">
      <table class="pure-table pure-table-horizontal">
        <tbody>
          <tr>
            <td>Heap free</td>
            <td>{Math.round(state.system.heap_free / 1024)} Kb</td>
          </tr>

          <tr>
            <td>Heap free minimum</td>
            <td>{Math.round(state.system.heap_free_min / 1024)} Kb</td>
          </tr>

          <tr>
            <td>MAC address</td>
            <td>{state.system.mac_addr}</td>
          </tr>

          <tr>
            <td>IP address (station iface)</td>
            <td>{state.system.ip_addr}</td>
          </tr>
        </tbody>
      </table>

      <br />
      <button class="pure-button pure-button-primary" onclick={() => actions.system.reboot()}>Reboot</button>
    </div>
  </div>
