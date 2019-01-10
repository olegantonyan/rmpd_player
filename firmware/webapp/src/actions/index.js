import { HOME, SYSTEM, SETTINGS } from "../routes"

export default {
  nav_menu_toggle: value => state => ({ nav_menu_open: value }),
  nav_route_to: value => (state, actions) => {
    switch(value) {
      case HOME:
        break
      case SYSTEM:
        actions.system.fetch()
        break
      case SETTINGS:
        actions.settings.fetch()
        actions.audio.fetch()
        actions.settings.fetch_timezones()
        break
    }
    return { nav_current_route: value, nav_menu_open: false }
  },

  settings: {
    fetch: () => (state, actions) => {
      fetch("/api/settings.json", { headers: { "Connection": "close" } })
        .then(data => data.json())
        .then((data) => actions.update(data))
    },
    save: () => (state, actions) => {
      actions.saving(true)
      fetch("/api/settings.json", { method: "POST", body: JSON.stringify({wifi_ssid: state.wifi_ssid, wifi_pass: state.wifi_pass, timezone: state.timezone}), headers: { "Content-Type": "application/json", "Connection": "close" } })
        .then(data => data.json())
        .catch(error => actions.saving(false))
        .then((data) => { actions.update(data); actions.saving(false) })
        .catch(error => actions.saving(false))
    },
    update: value => state => { return value },
    saving: value => state => ({ saving: value }),

    fetch_timezones: () => (state, actions) => {
      fetch("/zones.json", { headers: { "Connection": "close" } })
        .then(data => data.json())
        .then((data) => actions.update({ all_timezones: data }))
    },
  },

  status: {
    fetch: () => (state, actions) => {
      fetch("/api/status.json", { headers: { "Connection": "close" } })
        .then(data => data.json())
        .then((data) => actions.update(data))
    },
    update: value => state => { return value },

    update_volume: value => (state, actions) => {
      fetch("/api/volume.json", { method: "POST", body: JSON.stringify({volume: Number(value)}), headers: { "Content-Type": "application/json", "Connection": "close" } })
        .then(data => data.json())
        .then((data) => { actions.update(data) })
      return { volume: value }
    },

    prev: () => (state, actions) => {
      fetch("/api/playback.json", { method: "POST", body: JSON.stringify({ action: "prev" }), headers: { "Content-Type": "application/json", "Connection": "close" } })
        .then(data => data.json())
        .then((data) => { actions.update(data) })
    },
    next: () => (state, actions) => {
      fetch("/api/playback.json", { method: "POST", body: JSON.stringify({ action: "next" }), headers: { "Content-Type": "application/json", "Connection": "close" } })
        .then(data => data.json())
        .then((data) => { actions.update(data) })
    },
    set_random: value => (state, actions) => {
      fetch("/api/playback.json", { method: "POST", body: JSON.stringify({ random: value }), headers: { "Content-Type": "application/json", "Connection": "close" } })
        .then(data => data.json())
        .then((data) => { actions.update(data) })
    }
  },

  audio: {
    update: value => state => {
      fetch("/api/tone.json", { method: "POST", body: JSON.stringify(value), headers: { "Content-Type": "application/json", "Connection": "close" } })
      return value
    },

    set: value => state => { return value },

    fetch: () => (state, actions) => {
      fetch("/api/tone.json", { headers: { "Connection": "close" } })
        .then(data => data.json())
        .then((data) => actions.set(data))
    },
  },

  system: {
    reboot: value => state => {
      fetch("/api/reboot.json", { method: "POST", headers: { "Connection": "close" } })
      return value
    },

    set: value => state => { return value },

    fetch: () => (state, actions) => {
      fetch("/api/system.json", { headers: { "Connection": "close" } })
        .then(data => data.json())
        .then((data) => actions.set(data))
    },
  }
}
