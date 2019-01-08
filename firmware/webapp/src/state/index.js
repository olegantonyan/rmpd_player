import { HOME } from "../routes"

export default {
  // "router"
  nav_menu_open: false,
  nav_current_route: HOME,

  settings: {
    wifi_ssid: null,
    wifi_pass: null,
    saving: false
  },

  status: {
    now_playing: "",
    percent_pos: 0,
    seconds_pos: 0,
    time: "",
    volume: 0,
    random: false
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
    reset_reason: "",
    chip_revision: 0,
    heap_free: 0,
    heap_free_min: 0,
    uptime: 0
  }
};
