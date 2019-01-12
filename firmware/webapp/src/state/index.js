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
    ap_static_addr: "",
    reset_reason: "",
    chip_revision: 0,
    heap_free: 0,
    heap_free_min: 0,
    uptime: 0,
    "task_list": "Task Name\tStatus\tPrio\tHWM\tTask#\tAffinity\nhttpd          \tR\t5\t916\t16\t-1\r\nIDLE0          \tR\t0\t988\t6\t0\r\nIDLE1          \tR\t0\t980\t7\t1\r\nstream         \tB\t5\t500\t22\t-1\r\ntiT            \tB\t18\t1536\t13\t-1\r\nTmr Svc        \tB\t1\t1648\t8\t0\r\neventTask      \tB\t20\t1044\t14\t0\r\nipc1           \tB\t24\t468\t3\t1\r\nesp_timer      \tB\t22\t3396\t1\t0\r\nwifi           \tB\t23\t1328\t15\t0\r\nplayer         \tB\t15\t1876\t17\t-1\r\nipc0           \tB\t24\t516\t2\t0\r\nscheduler      \tB\t6\t2540\t18\t-1\r\n",
    "runtime_stats": "httpd          \t945558\t\t<1%\r\nIDLE0          \t356257127\t\t45%\r\nIDLE1          \t383036121\t\t49%\r\ntiT            \t5470599\t\t<1%\r\nstream         \t1718242\t\t<1%\r\nTmr Svc        \t43\t\t<1%\r\nipc1           \t73488\t\t<1%\r\nesp_timer      \t767277\t\t<1%\r\nwifi           \t16676701\t\t2%\r\nplayer         \t12779432\t\t1%\r\nipc0           \t58120\t\t<1%\r\nscheduler      \t6149\t\t<1%\r\neventTask      \t4739\t\t<1%\r\n"
  }
};
