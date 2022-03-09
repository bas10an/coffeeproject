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

typedef struct Button_driver Button_driver;
Button_driver *button_driver_create(gpio_num_t gpio_num);
void button_driver_delete(Button_driver *self);
void button_driver_init(Button_driver *self);
void button_pressed_event(void *args);;