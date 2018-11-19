#include "wifi/wifi.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "rom/ets_sys.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "wifi";

static esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        //xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR" join, AID=%d", MAC2STR(event->event_info.sta_connected.mac), event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR" leave, AID=%d", MAC2STR(event->event_info.sta_disconnected.mac), event->event_info.sta_disconnected.aid);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "sta disconnected");
        //ESP_ERROR_CHECK(esp_wifi_stop());
        //ESP_ERROR_CHECK(esp_wifi_start());
        esp_wifi_connect();
        //xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void dhcp_server_init() {
  ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
  tcpip_adapter_ip_info_t ipinfo;
  memset(&ipinfo, 0, sizeof(ipinfo));
  IP4_ADDR(&ipinfo.ip, 10,10,0,1);
  IP4_ADDR(&ipinfo.gw, 10,10,0,1);
  IP4_ADDR(&ipinfo.netmask, 255,255,255,0);
  ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ipinfo));
  ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
}

bool wifi_init() {
    //wifi_event_group = xEventGroupCreate();

    esp_wifi_set_storage(WIFI_STORAGE_RAM);

    tcpip_adapter_init();

    dhcp_server_init();

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t sta_config;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA)); //

    strcpy((char *)sta_config.sta.ssid, "flhome");
    strcpy((char *)sta_config.sta.password, "password_changed");
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));

    wifi_config_t ap_config;
    strcpy((char *)ap_config.ap.ssid, "rmpd_player");
    ap_config.ap.ssid_len = strlen("rmpd_player");
    strcpy((char *)ap_config.ap.password, "12345678");
    ap_config.ap.max_connection = 5;
    ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));


    //if (strlen( "12345678") == 0) {
    //    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    //}


    ESP_ERROR_CHECK(esp_wifi_start());

  //  xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);

    //ESP_LOGI(TAG, "wifi init finished");

    return true;
}
