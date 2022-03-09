#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

typedef struct WiFi_station WiFi_station;
WiFi_station *WiFi_sta_create(uint8_t ssid[32], uint8_t password[64]);
void WiFi_sta_delete(WiFi_station *self);
void WiFi_sta_init(WiFi_station *self);
bool WiFi_connected_status(WiFi_station *self);
void WiFi_start_task(void* parameters);