#include "web/web.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "config/config.h"
#include "wifi/wifi.h"
#include "audio/player.h"
#include "util/strings.h"
#include "util/files.h"
#include "audio/player.h"
#include "playlist/offline/scheduler.h"
#include "tcpip_adapter.h"
#include "esp_timer.h"
#include "web/auth.h"
#include "clock/clock.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "system/sysinfo.h"
#include "web/fs.h"
#include "system/status.h"

static const char *TAG = "web";

static void send_text_file(const char *start, httpd_req_t *req);
static esp_err_t root_get_handler(httpd_req_t *req);
static esp_err_t settings_get_handler(httpd_req_t *req);
static esp_err_t settings_post_handler(httpd_req_t *req);
static esp_err_t tone_get_handler(httpd_req_t *req);
static esp_err_t tone_post_handler(httpd_req_t *req);
static esp_err_t volume_post_handler(httpd_req_t *req);
static esp_err_t status_get_handler(httpd_req_t *req);
static esp_err_t playback_post_handler(httpd_req_t *req);
static esp_err_t reboot_post_handler(httpd_req_t *req);
static esp_err_t system_get_handler(httpd_req_t *req);
static esp_err_t system_post_handler(httpd_req_t *req);
static esp_err_t zones_get_handler(httpd_req_t *req);
static httpd_handle_t start_webserver();
static void render_settings(httpd_req_t *req);
static void render_status(httpd_req_t *req);

#ifndef MIN
  #define MIN(a,b) (((a)<(b))?(a):(b))
#endif

static httpd_uri_t root = {
  .uri       = "/*",
  .method    = HTTP_GET,
  .handler   = root_get_handler
};

static httpd_uri_t settings_get = {
  .uri       = "/api/settings.json",
  .method    = HTTP_GET,
  .handler   = settings_get_handler
};

static httpd_uri_t settings_post = {
  .uri       = "/api/settings.json",
  .method    = HTTP_POST,
  .handler   = settings_post_handler
};

static httpd_uri_t status_get = {
  .uri       = "/api/status.json",
  .method    = HTTP_GET,
  .handler   = status_get_handler
};

static httpd_uri_t volume_post = {
  .uri       = "/api/volume.json",
  .method    = HTTP_POST,
  .handler   = volume_post_handler
};

static httpd_uri_t tone_post = {
  .uri       = "/api/tone.json",
  .method    = HTTP_POST,
  .handler   = tone_post_handler
};

static httpd_uri_t tone_get = {
  .uri       = "/api/tone.json",
  .method    = HTTP_GET,
  .handler   = tone_get_handler
};

static httpd_uri_t playback_post = {
  .uri       = "/api/playback.json",
  .method    = HTTP_POST,
  .handler   = playback_post_handler
};

static httpd_uri_t reboot_post = {
  .uri       = "/api/reboot.json",
  .method    = HTTP_POST,
  .handler   = reboot_post_handler
};

static httpd_uri_t system_get = {
  .uri       = "/api/system.json",
  .method    = HTTP_GET,
  .handler   = system_get_handler
};

static httpd_uri_t zones_get = {
  .uri       = "/zones.json",
  .method    = HTTP_GET,
  .handler   = zones_get_handler
};

static httpd_uri_t system_post = {
  .uri       = "/api/system.json",
  .method    = HTTP_POST,
  .handler   = system_post_handler
};

bool web_init() {
  return start_webserver() != NULL;
}

static void render_settings(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");

  cJSON *root = cJSON_CreateObject();

  char *ssid = config_wifi_ssid();
  char *pass = config_wifi_pass();

  cJSON_AddItemToObject(root, "wifi_ssid", cJSON_CreateString(ssid));
  cJSON_AddItemToObject(root, "wifi_pass", cJSON_CreateString(pass));

  char *timezone = config_timezone();
  cJSON_AddItemToObject(root, "timezone", cJSON_CreateString(timezone));

  char* json = malloc(1024);

  if(cJSON_PrintPreallocated(root, json, 1024, 0)) {
    httpd_resp_send(req, json, strlen(json));
  } else {
    ESP_LOGE(TAG, "failed to build json response");
    httpd_resp_send_500(req);
  }

  free(json);
  cJSON_Delete(root);
}

static void render_status(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");

  cJSON *root = cJSON_CreateObject();

  char now_playing[512] = { 0 };
  if (player_get_now_playing(now_playing, sizeof(now_playing))) {
    cJSON_AddItemToObject(root, "now_playing", cJSON_CreateString(now_playing));
  } else {
    cJSON_AddItemToObject(root, "now_playing", cJSON_CreateString(""));
  }
  cJSON_AddItemToObject(root, "percent_pos", cJSON_CreateNumber(player_get_position_percents()));
  cJSON_AddItemToObject(root, "seconds_pos", cJSON_CreateNumber(player_get_position_seconds()));
  cJSON_AddItemToObject(root, "volume", cJSON_CreateNumber(config_volume()));
  cJSON_AddItemToObject(root, "random", cJSON_CreateBool(offline_scheduler_random()));
  cJSON_AddItemToObject(root, "synchronizing", cJSON_CreateBool(status_synchronizing()));
  time_t now = time(NULL);
  struct tm timeinfo = { 0 };
  localtime_r(&now, &timeinfo);
  char time_buf[80] = { 0 };
  strftime(time_buf, sizeof(time_buf), "%H:%M:%S %d-%m-%Y %Z", &timeinfo);
  cJSON_AddItemToObject(root, "time", cJSON_CreateString(time_buf));

  cJSON_AddItemToObject(root, "online", cJSON_CreateBool(status_online()));

  char* json = malloc(1024);

  if(cJSON_PrintPreallocated(root, json, 1024, 0)) {
    httpd_resp_send(req, json, strlen(json));
  } else {
    ESP_LOGE(TAG, "failed to build json response");
    httpd_resp_send_500(req);
  }

  free(json);
  cJSON_Delete(root);
}

static esp_err_t settings_get_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  httpd_resp_set_hdr(req, "Connection", "close");
  render_settings(req);
  return ESP_OK;
}

static esp_err_t volume_post_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  httpd_resp_set_hdr(req, "Connection", "close");
  if (req->content_len > 128) {
    ESP_LOGE(TAG, "too big request body %d", req->content_len);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char *buffer = malloc(req->content_len + 4);
  int ret = httpd_req_recv(req, buffer, req->content_len);
  if (ret <= 0) {
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  buffer[ret] = '\0';

  cJSON *json = cJSON_Parse(buffer);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "json parse error: %s", error_ptr);
    }
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  const cJSON *volume = cJSON_GetObjectItemCaseSensitive(json, "volume");
  if (!cJSON_IsNumber(volume)) {
    ESP_LOGE(TAG, "error getting volume from json");
    free(buffer);
    cJSON_Delete(json);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  player_set_volume(volume->valueint);
  free(buffer);
  cJSON_Delete(json);

  render_status(req);
  return ESP_OK;
}

static esp_err_t settings_post_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  httpd_resp_set_hdr(req, "Connection", "close");
  bool ok = true;

  if (req->content_len > 2048) {
    ESP_LOGE(TAG, "too big request body %d", req->content_len);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char *buffer = malloc(req->content_len + 4);
  int ret = httpd_req_recv(req, buffer, req->content_len);
  if (ret <= 0) {
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  buffer[ret] = '\0';

  cJSON *json = cJSON_Parse(buffer);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "json parse error: %s", error_ptr);
      ok = false;
      goto exit;
    }
  } else {
    const cJSON *timezone = cJSON_GetObjectItemCaseSensitive(json, "timezone");
    if (cJSON_IsString(timezone) && (timezone->valuestring != NULL)) {
      if (strcmp(timezone->valuestring, config_timezone()) != 0) {
        if(!config_save_timezone(timezone->valuestring)) {
          ok = false;
          goto exit;
        }
        clock_set_timezone_from_config();
      }
    }

    const cJSON *ssid = cJSON_GetObjectItemCaseSensitive(json, "wifi_ssid");
    if (!cJSON_IsString(ssid) || (ssid->valuestring == NULL)) {
      ok = false;
      goto exit;
    }
    const cJSON *pass = cJSON_GetObjectItemCaseSensitive(json, "wifi_pass");
    if (!cJSON_IsString(pass) || (pass->valuestring == NULL)) {
      ok = false;
      goto exit;
    }
    if (strlen(ssid->valuestring) > 31 || strlen(pass->valuestring) > 63) {
      ok = false;
      goto exit;
    }
    if (config_wifi_ssid() == NULL || config_wifi_pass() == NULL || (config_wifi_ssid() != NULL && strcmp(ssid->valuestring, config_wifi_ssid()) != 0) || ( config_wifi_pass() != NULL && strcmp(pass->valuestring, config_wifi_pass()) != 0)) {
      ok = config_save_wifi_ssid(ssid->valuestring) && config_save_wifi_pass(pass->valuestring);
      wifi_reconfig();
    }
  }

exit:
  free(buffer);
  cJSON_Delete(json);
  if (ok) {
    render_settings(req);
    return ESP_OK;
  } else {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
}

static esp_err_t status_get_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  httpd_resp_set_hdr(req, "Connection", "close");
  render_status(req);
  return ESP_OK;
}

static esp_err_t tone_get_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  httpd_resp_set_hdr(req, "Connection", "close");
  httpd_resp_set_type(req, "application/json");

  cJSON *root = cJSON_CreateObject();

  cJSON_AddItemToObject(root, "bass_frequency_limit", cJSON_CreateNumber(config_bass_freqlimit()));
  cJSON_AddItemToObject(root, "bass_amplitude", cJSON_CreateNumber(config_bass_amplitude()));
  cJSON_AddItemToObject(root, "treble_frequency_limit", cJSON_CreateNumber(config_treble_freqlimit()));
  cJSON_AddItemToObject(root, "treble_amplitude", cJSON_CreateNumber(config_treble_amplitude()));

  char* json = malloc(1024);

  if(cJSON_PrintPreallocated(root, json, 1024, 0)) {
    httpd_resp_send(req, json, strlen(json));
  } else {
    ESP_LOGE(TAG, "failed to build json response");
    httpd_resp_send_500(req);
  }

  free(json);
  cJSON_Delete(root);

  return ESP_OK;
}

static esp_err_t tone_post_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  httpd_resp_set_hdr(req, "Connection", "close");
  if (req->content_len > 256) {
    ESP_LOGE(TAG, "too big request body %d", req->content_len);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char *buffer = malloc(req->content_len + 4);
  int ret = httpd_req_recv(req, buffer, req->content_len);
  if (ret <= 0) {
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  buffer[ret] = '\0';

  cJSON *json = cJSON_Parse(buffer);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "json parse error: %s", error_ptr);
    }
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  const cJSON *bass_frequency_limit = cJSON_GetObjectItemCaseSensitive(json, "bass_frequency_limit");
  if (cJSON_IsNumber(bass_frequency_limit)) {
    player_set_bass_freqlimit(bass_frequency_limit->valueint);
  }
  const cJSON *bass_amplitude = cJSON_GetObjectItemCaseSensitive(json, "bass_amplitude");
  if (cJSON_IsNumber(bass_amplitude)) {
    player_set_bass_amplitude(bass_amplitude->valueint);
  }
  const cJSON *treble_frequency_limit = cJSON_GetObjectItemCaseSensitive(json, "treble_frequency_limit");
  if (cJSON_IsNumber(treble_frequency_limit)) {
    player_set_treble_freqlimit(treble_frequency_limit->valueint);
  }
  const cJSON *treble_amplitude = cJSON_GetObjectItemCaseSensitive(json, "treble_amplitude");
  if (cJSON_IsNumber(treble_amplitude)) {
    player_set_treble_amplitude(treble_amplitude->valueint);
  }
  free(buffer);
  cJSON_Delete(json);

  render_status(req);
  return ESP_OK;
}

static esp_err_t playback_post_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  httpd_resp_set_hdr(req, "Connection", "close");
  if (req->content_len > 128) {
    ESP_LOGE(TAG, "too big request body %d", req->content_len);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char *buffer = malloc(req->content_len + 4);
  int ret = httpd_req_recv(req, buffer, req->content_len);
  if (ret <= 0) {
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  buffer[ret] = '\0';

  cJSON *json = cJSON_Parse(buffer);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "json parse error: %s", error_ptr);
    }
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  const cJSON *action = cJSON_GetObjectItemCaseSensitive(json, "action");
  if (cJSON_IsString(action) && (action->valuestring != NULL)) {
    if(strcmp(action->valuestring, "next") == 0) {
      offline_scheduler_next();
    } else if(strcmp(action->valuestring, "prev") == 0) {
      offline_scheduler_prev();
    }
  }
  const cJSON *random = cJSON_GetObjectItemCaseSensitive(json, "random");
  if (cJSON_IsTrue(random)) {
    offline_scheduler_set_random(true);
  } else if (cJSON_IsFalse(random)) {
    offline_scheduler_set_random(false);
  }
  free(buffer);
  cJSON_Delete(json);

  render_status(req);
  return ESP_OK;
}

static esp_err_t reboot_post_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  esp_restart();
  return ESP_OK;
}

static esp_err_t system_get_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  httpd_resp_set_hdr(req, "Connection", "close");
  httpd_resp_set_type(req, "application/json");

  cJSON *root = cJSON_CreateObject();
  cJSON_AddItemToObject(root, "heap_free", cJSON_CreateNumber(esp_get_free_heap_size()));
  cJSON_AddItemToObject(root, "heap_free_min", cJSON_CreateNumber(esp_get_minimum_free_heap_size()));

  uint8_t mac[6] = { 0 };
  esp_efuse_mac_get_default(mac);
  char mac_str[20] = { 0 };
  snprintf(mac_str, sizeof(mac_str), "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  cJSON_AddItemToObject(root, "mac_addr", cJSON_CreateString(mac_str));

  char ip[64] = { 0 };
  sysinfo_sta_ip_addr(ip, sizeof(ip));
  cJSON_AddItemToObject(root, "ip_addr", cJSON_CreateString(ip));

  config_ip_addr_t ap_ad = config_ap_static_ip();
  snprintf(mac_str, sizeof(mac_str), "%d.%d.%d.%d", ap_ad.oct1, ap_ad.oct2, ap_ad.oct3, ap_ad.oct4);
  cJSON_AddItemToObject(root, "ap_static_addr", cJSON_CreateString(mac_str));

  cJSON_AddItemToObject(root, "reset_reason", cJSON_CreateString(sysinfo_reset_reason()));

  cJSON_AddItemToObject(root, "uptime", cJSON_CreateNumber(esp_timer_get_time() / 1000000));

  cJSON_AddItemToObject(root, "cloud_addr", cJSON_CreateString(config_server_url()));

  cJSON_AddItemToObject(root, "disable_tls_certs_verification", cJSON_CreateBool(config_disable_tls_certs_verification()));

  char ua[128] = { 0 };
  sysinfo_useragent(ua, sizeof(ua));
  cJSON_AddItemToObject(root, "useragent", cJSON_CreateString(ua));

  bool task_list_ok = true;
  const char *task_list_header = "Task Name\tStatus\tPrio\tStack Free\tTask#\tAffinity\n";
  size_t bytes_for_tasks_list = uxTaskGetNumberOfTasks() * 40 + strlen(task_list_header);
  char *task_list = malloc(bytes_for_tasks_list);
  if (task_list == NULL) {
    task_list = "malloc failed";
    task_list_ok = false;
  } else {
    task_list[0] = '\0';
    strcat(task_list, task_list_header);
    vTaskList(task_list + strlen(task_list_header));
  }
  cJSON_AddItemToObject(root, "task_list", cJSON_CreateString(task_list));

  bool runtime_stats_ok = true;
  const char *runtime_stats_header = "Task Name\tAbs Time\t% Time\n";
  char *runtime_stats = malloc(bytes_for_tasks_list);
  if (runtime_stats == NULL) {
    runtime_stats = "malloc failed";
    runtime_stats_ok = false;
  } else {
    runtime_stats[0] = '\0';
    strcat(runtime_stats, runtime_stats_header);
    vTaskGetRunTimeStats(runtime_stats + strlen(runtime_stats_header));
  }
  cJSON_AddItemToObject(root, "runtime_stats", cJSON_CreateString(runtime_stats));

  size_t json_size = 1200 + bytes_for_tasks_list * 2;
  char* json = malloc(json_size);
  if (json == NULL) {
    ESP_LOGE(TAG, "json allocation failed");
    httpd_resp_send_500(req);
  } else {
    if(cJSON_PrintPreallocated(root, json, json_size, 0)) {
      httpd_resp_send(req, json, strlen(json));
      cJSON_Delete(root);
    } else {
      ESP_LOGE(TAG, "failed to build json response");
      httpd_resp_send_500(req);
    }
    free(json);
  }

  if (task_list_ok) {
    free(task_list);
  }
  if (runtime_stats_ok) {
    free(runtime_stats);
  }

  return ESP_OK;
}

static esp_err_t system_post_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  httpd_resp_set_hdr(req, "Connection", "close");
  if (req->content_len > 128) {
    ESP_LOGE(TAG, "too big request body %d", req->content_len);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char *buffer = malloc(req->content_len + 4);
  int ret = httpd_req_recv(req, buffer, req->content_len);
  if (ret <= 0) {
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  buffer[ret] = '\0';

  cJSON *json = cJSON_Parse(buffer);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "json parse error: %s", error_ptr);
    }
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  const cJSON *disable_tls_certs_verification = cJSON_GetObjectItemCaseSensitive(json, "disable_tls_certs_verification");
  if (cJSON_IsTrue(disable_tls_certs_verification)) {
    config_save_disable_tls_certs_verification(true);
  } else if (cJSON_IsFalse(disable_tls_certs_verification)) {
    config_save_disable_tls_certs_verification(false);
  }
  free(buffer);
  cJSON_Delete(json);

  render_status(req);
  return ESP_OK;
}

static esp_err_t zones_get_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  httpd_resp_set_hdr(req, "Connection", "close");

  send_text_file(clock_zones(), req);

  return ESP_OK;
}

static esp_err_t root_get_handler(httpd_req_t *req) {
  if (!auth_check(req)) {
    return ESP_OK;
  }
  httpd_resp_set_hdr(req, "Connection", "close");

  if(strcmp("/", req->uri) == 0 || strcmp("/index.html", req->uri) == 0) {
    httpd_resp_set_type(req, "text/html");
    send_text_file(index_html_start, req);
  } else if(strcmp("/favicon.ico", req->uri) == 0) {
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, "", 0); // stub empty response
  } else {
    httpd_resp_send_404(req);
  }

  return ESP_OK;
}

static void send_text_file(const char *data, httpd_req_t *req) {
  static const size_t HTTP_CHUNK_SIZE = 4096;
  size_t data_len = strlen(data);
  if(data_len <= HTTP_CHUNK_SIZE) {
    httpd_resp_send(req, data, data_len);
  } else {
    for(size_t bytes_sent = 0; bytes_sent < data_len; bytes_sent += HTTP_CHUNK_SIZE) {
      httpd_resp_send_chunk(req, data + bytes_sent, MIN(data_len - bytes_sent, HTTP_CHUNK_SIZE));
    }
    httpd_resp_send_chunk(req, NULL, 0);
  }
}

static httpd_handle_t start_webserver() {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_uri_handlers = 20;
  config.max_open_sockets = 8;
  config.stack_size = 3500;

  /* Use the URI wildcard matching function in order to
     * allow the same handler to respond to multiple different
     * target URIs which match the wildcard scheme */
  config.uri_match_fn = httpd_uri_match_wildcard;

  ESP_LOGI(TAG, "starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) != ESP_OK) {
    ESP_LOGI(TAG, "error starting server!");
    return NULL;
  }
  httpd_register_uri_handler(server, &settings_get);
  httpd_register_uri_handler(server, &settings_post);
  httpd_register_uri_handler(server, &volume_post);
  httpd_register_uri_handler(server, &status_get);
  httpd_register_uri_handler(server, &tone_get);
  httpd_register_uri_handler(server, &tone_post);
  httpd_register_uri_handler(server, &playback_post);
  httpd_register_uri_handler(server, &reboot_post);
  httpd_register_uri_handler(server, &system_get);
  httpd_register_uri_handler(server, &zones_get);
  httpd_register_uri_handler(server, &system_post);
  httpd_register_uri_handler(server, &root);
  return server;
}
