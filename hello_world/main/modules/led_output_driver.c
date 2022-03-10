#include "led_output_driver.h"

struct LED_driver
{
    gpio_num_t _gpio_num;
    bool _led_on;
};

void led_driver_construct(LED_driver *self, gpio_num_t gpio_num)
{
    self->_gpio_num = gpio_num;
}

LED_driver *led_driver_create(gpio_num_t gpio_num)
{
    LED_driver *result = (LED_driver *)malloc(sizeof(LED_driver));
    led_driver_construct(result, gpio_num);
    return result;
}

void led_driver_delete(LED_driver *self)
{
    if (self)
    {
        ESP_ERROR_CHECK(gpio_reset_pin(self->_gpio_num));
        ESP_ERROR_CHECK(gpio_isr_handler_remove(self->_gpio_num));
        free(self);
    }
}

void led_driver_init(LED_driver *self)
{
    ESP_ERROR_CHECK(gpio_reset_pin(self->_gpio_num));
    ESP_ERROR_CHECK(gpio_set_direction(self->_gpio_num, GPIO_MODE_OUTPUT));
    self->_led_on = false;
}

void led_driver_toggle(LED_driver *self)
{
    if(self->_led_on)
    {
        ESP_ERROR_CHECK(gpio_set_level(self->_gpio_num, 0));
        self->_led_on = false;
    }
    else
    {
        ESP_ERROR_CHECK(gpio_set_level(self->_gpio_num, 1));
        self->_led_on = true;
    }
}