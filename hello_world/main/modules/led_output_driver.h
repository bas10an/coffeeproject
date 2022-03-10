#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_intr_alloc.h"

typedef struct LED_driver LED_driver;
LED_driver *led_driver_create(gpio_num_t gpio_num);
void led_driver_delete(LED_driver *self);
void led_driver_init(LED_driver *self);
void led_driver_toggle(LED_driver *self);