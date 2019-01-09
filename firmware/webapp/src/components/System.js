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
            <td>Reset reason</td>
            <td>{state.system.reset_reason}</td>
          </tr>

          <tr>
            <td>Chip revision</td>
            <td>{state.system.chip_revision}</td>
          </tr>

          <tr>
            <td>MAC address</td>
            <td>{state.system.mac_addr}</td>
          </tr>

          <tr>
            <td>IP address (station iface)</td>
            <td>{state.system.ip_addr}</td>
          </tr>

          <tr>
            <td>IP address (static AP iface)</td>
            <td>{state.system.ap_static_addr}</td>
          </tr>

          <tr>
            <td>Uptime</td>
            <td>{seconds_to_string(state.system.uptime)}</td>
          </tr>
        </tbody>
      </table>

      <br />
      <button class="pure-button pure-button-primary" onclick={() => actions.system.reboot()}>Reboot</button>
    </div>
  </div>

  function seconds_to_string (seconds) {
    var years = Math.floor(seconds / 31536000);
    var max =2;
    var current = 0;
    var str = "";
    if (years && current<max) {
        str+= years + 'y ';
        current++;
    }
    var days = Math.floor((seconds %= 31536000) / 86400);
    if (days && current<max) {
        str+= days + 'd ';
        current++;
    }
    var hours = Math.floor((seconds %= 86400) / 3600);
    if (hours && current<max) {
        str+= hours + 'h ';
        current++;
    }
    var minutes = Math.floor((seconds %= 3600) / 60);
    if (minutes && current<max) {
        str+= minutes + 'm ';
        current++;
    }
    var seconds = seconds % 60;
    if (seconds && current<max) {
        str+= seconds + 's ';
        current++;
    }
    return str;
  }
