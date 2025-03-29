#pragma once
extern "C" {
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
}
//ESP_WIFI_SAE_MODE = ESP_WPA3_SAE_PWE_BOTH;
//ESP_WIFI_PW_ID = "";
//ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD = ESP_WIFI_AUTH_WPA2_PSK;
#define EXAMPLE_ESP_WIFI_SSID     "RmC32"
#define EXAMPLE_ESP_WIFI_PASS     "yhydg6tceggnzc5"
#define EXAMPLE_ESP_MAXIMUM_RETRY 5

static EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char* WIFI_TAG = "wifi station";

static int s_retry_num      = 0;

void wifi_init_sta(void);
