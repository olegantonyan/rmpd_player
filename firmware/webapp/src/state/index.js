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
    percent_pos: 0
  }
};
