export default {
  nav_menu_toggle: value => state => ({ nav_menu_open: value }),
  nav_route_to: value => state => ({ nav_current_route: value, nav_menu_open: false }),
}
