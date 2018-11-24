const actions = {
  nav_menu_toggle: value => state => ({ nav_menu_open: value }),
  nav_route_to: value => state => ({ nav_current_route: value, nav_menu_open: false }),

  fetch_settings: () => (state, actions) => {
    fetch("/settings.json")
      .then(data => data.json())
      .then((data) => {
        //setTimeout(() => { actions.fetch_settings() }, 1000)
        if(state.settings === null) {
          console.log(data)
          actions.set_settings(data)
        }
      })
  },
  set_settings: value => state => ({ settings: value }),
  save_settings: () => (state, actions) => {
    console.log("saving...")
    console.log(state.settings)

    fetch("/settings.json")
      .then(data => data.json())
      .then((data) => {
        //setTimeout(() => { actions.fetch_settings() }, 1000)
        if(state.settings != data) {
          console.log(data)
          actions.set_settings(data)
        }
      })

  },
  update_settings: value => state => {
    return { settings: { wifi_ssid: (value.wifi_ssid === undefined ? state.settings.wifi_ssid : value.wifi_ssid),
                         wifi_pass: (value.wifi_pass === undefined ? state.settings.wifi_pass : value.wifi_pass)
                       } }
  },
}

export default actions
