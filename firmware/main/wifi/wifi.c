#include "wifi/wifi.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "config/config.h"

static const char *TAG = "wireless";

static esp_err_t event_handler(void *ctx, system_event_t *event);
static void dhcp_server_init();
static void softap();
static void station();
static bool configure();
static char *hostname();
static void reconnect_thread_hack(void *params);

bool wifi_init() {
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_ps(WIFI_PS_NONE); // disable powersave

  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );
  return configure();
}

bool wifi_reconfig() {
  station();
  return true;
}

static bool configure() {
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA) );

  station();
  softap();
  ESP_ERROR_CHECK(esp_wifi_start());

  return true;
}

static void station() {
  wifi_config_t wifi_config;

  wifi_config.sta.ssid[0] = '\0';
  wifi_config.sta.password[0] = '\0';
  if (config_wifi_ssid() != NULL) {
    strcpy((char *)wifi_config.sta.ssid, config_wifi_ssid());
  }
  if (config_wifi_pass() != NULL) {
    strcpy((char *)wifi_config.sta.password, config_wifi_pass());
  }

  if (wifi_config.sta.ssid) {
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(TAG, "connect to ap ssid:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
  }
}

static esp_err_t event_handler(void *ctx, system_event_t *event) {
  switch(event->event_id) {
  case SYSTEM_EVENT_STA_START:
    tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, hostname());
    esp_wifi_connect();
    break;
  case SYSTEM_EVENT_AP_START:
    ESP_LOGI(TAG, "ap start");
    dhcp_server_init();
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
    break;
  case SYSTEM_EVENT_AP_STACONNECTED:
    ESP_LOGI(TAG, "station:"MACSTR" join, AID=%d", MAC2STR(event->event_info.sta_connected.mac), event->event_info.sta_connected.aid);
    break;
  case SYSTEM_EVENT_AP_STADISCONNECTED:
    ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d", MAC2STR(event->event_info.sta_disconnected.mac), event->event_info.sta_disconnected.aid);
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    ESP_LOGI(TAG, "disconnected");
    static uint32_t counter = 0;
    vTaskDelay(pdMS_TO_TICKS(2000));
    if (counter++ % 5 == 0) {
      xTaskCreate(reconnect_thread_hack, "reconnect", 2048, NULL, 5, NULL);
    } else {
      esp_wifi_connect();
    }
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
  config_ip_addr_t c = config_ap_static_ip();
  IP4_ADDR(&ipinfo.ip, c.oct1, c.oct2, c.oct3, c.oct4);
  IP4_ADDR(&ipinfo.gw, c.oct1, c.oct2, c.oct3, c.oct4);
  IP4_ADDR(&ipinfo.netmask, 255,255,255,0);
  ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ipinfo));
  ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
}

static void softap() {
  wifi_config_t wifi_config = {
    .ap = {
      .max_connection = 5,
      .authmode = WIFI_AUTH_WPA_WPA2_PSK
    },
  };
  wifi_config.ap.ssid[0] = '\0';
  wifi_config.ap.password[0] = '\0';
  strcpy((char *)wifi_config.ap.ssid, hostname());
  strcpy((char *)wifi_config.ap.password, config_ap_password());
  wifi_config.ap.ssid_len = strlen((char *)wifi_config.ap.ssid);

  if (strlen((char *)wifi_config.ap.password) == 0) {
      wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  }
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
  tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP, hostname());
  ESP_LOGI(TAG, "softap ssid:%s password:%s", wifi_config.ap.ssid, wifi_config.ap.password);
}

static char *hostname() {
  static char h[32] = { 0 };
  if (strlen(h) > 0) {
    return h;
  }
  strcat(h, "slon_");
  strncat(h, config_deviceid(), sizeof(h) - 6);
  return h;
}

static void reconnect_thread_hack(void *params) {
  vTaskDelay(pdMS_TO_TICKS(2000));
  station();
  esp_wifi_connect();
  vTaskDelete(NULL);
}
