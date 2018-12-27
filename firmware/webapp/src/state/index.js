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
    time: "",
    volume: 0
  },

  audio: {
    bass_frequency_limit: 0,
    bass_amplitude: 0,
    treble_frequency_limit: 0,
    treble_amplitude: 0
  }
};
