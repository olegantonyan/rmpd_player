import { HOME, SYSTEM, SETTINGS } from "../routes"

export default {
  nav_menu_toggle: value => state => ({ nav_menu_open: value }),
  nav_route_to: value => (state, actions) => {
    switch(value) {
      case HOME:
        break
      case SYSTEM:
        break
      case SETTINGS:
        actions.settings.fetch()
        actions.audio.fetch()
        break
    }
    return { nav_current_route: value, nav_menu_open: false }
  },

  home: {
    prev: () => (state, actions) => {
      console.log("prev");
    },
    next: () => (state, actions) => {
      console.log("next");
    },
  },

  settings: {
    fetch: () => (state, actions) => {
      fetch("/api/settings.json")
        .then(data => data.json())
        .then((data) => actions.update(data))
    },
    save: () => (state, actions) => {
      actions.saving(true)
      fetch("/api/settings.json", { method: "POST", body: JSON.stringify({wifi_ssid: state.wifi_ssid, wifi_pass: state.wifi_pass}), headers: { "Content-Type": "application/json" } })
        .then(data => data.json())
        .catch(error => actions.saving(false))
        .then((data) => { actions.update(data); actions.saving(false) })
        .catch(error => actions.saving(false))
    },
    update: value => state => { return value },
    saving: value => state => ({ saving: value })
  },

  status: {
    fetch: () => (state, actions) => {
      fetch("/api/status.json")
        .then(data => data.json())
        .then((data) => actions.update(data))
    },
    update: value => state => { return value },

    update_volume: value => (state, actions) => {
      fetch("/api/volume.json", { method: "POST", body: JSON.stringify({volume: Number(value)}), headers: { "Content-Type": "application/json" } })
      return { volume: value }
    },
  },

  audio: {
    update: value => state => {
      fetch("/api/tone.json", { method: "POST", body: JSON.stringify(value), headers: { "Content-Type": "application/json" } })
      return value
    },

    set: value => state => { return value },

    fetch: () => (state, actions) => {
      fetch("/api/tone.json")
        .then(data => data.json())
        .then((data) => actions.set(data))
    },
  }
}
