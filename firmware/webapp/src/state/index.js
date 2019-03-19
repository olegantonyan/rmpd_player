import { HOME } from "../routes"

export default {
  // "router"
  nav_menu_open: false,
  nav_current_route: HOME,

  settings: {
    wifi_ssid: null,
    wifi_pass: null,
    timezone: "",
    saving: false,
    all_timezones: {},
  },

  status: {
    now_playing: "",
    percent_pos: 0,
    seconds_pos: 0,
    time: "",
    volume: 0,
    random: false,
    online: false
  },

  audio: {
    bass_frequency_limit: 0,
    bass_amplitude: 0,
    treble_frequency_limit: 0,
    treble_amplitude: 0
  },

  system: {
    mac_addr: "",
    ip_addr: "",
    ap_static_addr: "",
    reset_reason: "",
    heap_free: 0,
    heap_free_min: 0,
    uptime: 0,
    task_list: "",
    runtime_stats: "",
    useragent: "",
    cloud_addr: "",
    disable_tls_certs_verification: false
  }
};
