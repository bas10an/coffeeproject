/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
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

#include "modules/wifi_station.h"
#include "modules/button_input_driver.h"
#include "modules/led_output_driver.h"

void app_main(void)
{
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  uint8_t ssid[32] = "Velkommen-2.4G";
  uint8_t password[64] = "Stochholm97";

  // Create C objects
  WiFi_station *wifi_station = WiFi_sta_create(ssid, password);
  LED_driver *led_driver = led_driver_create(GPIO_NUM_19);
  Button_driver *button_driver = button_driver_create(GPIO_NUM_18, led_driver);

  // Init simple tasks
  led_driver_init(led_driver);
  button_driver_init(button_driver);

  // Create Task handles
  TaskHandle_t wifi_task_handle;

  // Start Tasks
  xTaskCreate(WiFi_start_task, "WIFI_TASK", 4096, wifi_station, tskIDLE_PRIORITY, &wifi_task_handle);
}
