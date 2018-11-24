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
        break
    }
    return { nav_current_route: value, nav_menu_open: false }
  },

  settings: {
    fetch: () => (state, actions) => {
      fetch("/settings.json")
        .then(data => data.json())
        .then((data) => actions.update(data))
    },
    save: () => (state, actions) => {
      console.log("saving...")
      fetch("/settings.json", { method: "POST", body: JSON.stringify(state), headers: { "Content-Type": "application/json" } })
        .then(data => data.json())
        .catch(error => console.error('Error:', error))
        .then((data) => {
          console.log("saved!", JSON.stringify(data))
          //actions.fetch()
        })
    },
    update: value => state => { return value },
  }
}
