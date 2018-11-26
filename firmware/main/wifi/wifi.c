#include "wifi/wifi.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "storage/nvs.h"

// TODO use device uniq ID to set these
#define AP_SSID       "rmpd_player"
#define AP_PASS       "12345678"
#define HOSTNAME      "rmpd_player"

/* FreeRTOS event group to signal when we are connected*/
//static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "wifi";

static esp_err_t event_handler(void *ctx, system_event_t *event) {
  switch(event->event_id) {
  case SYSTEM_EVENT_STA_START:
    tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, HOSTNAME);
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
    ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d", MAC2STR(event->event_info.sta_disconnected.mac), event->event_info.sta_disconnected.aid);
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    vTaskDelay(5000);
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

static void softap() {
    wifi_config_t wifi_config = {
      .ap = {
        .ssid = AP_SSID,
        .ssid_len = strlen(AP_SSID),
        .password = AP_PASS,
        .max_connection = 5,
        .authmode = WIFI_AUTH_WPA_WPA2_PSK
      },
    };
    if (strlen(AP_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP, HOSTNAME);
    ESP_LOGI(TAG, "softap SSID:%s password:%s", AP_PASS, AP_PASS);
}

bool wifi_sta_connect() {
  wifi_config_t wifi_config;

  nvs_read_string("wifi_ssid", (char *)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid));
  nvs_read_string("wifi_pass", (char *)wifi_config.sta.password, sizeof(wifi_config.sta.password));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
  ESP_LOGI(TAG, "connect to ap SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);

  return true;
}

bool wifi_init() {
  //wifi_event_group = xEventGroupCreate();
  esp_wifi_set_storage(WIFI_STORAGE_RAM);

  tcpip_adapter_init();
  dhcp_server_init();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA) );

  softap();
  wifi_sta_connect();
  ESP_ERROR_CHECK(esp_wifi_start());

  return true;
}
