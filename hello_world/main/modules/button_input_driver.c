#include "button_input_driver.h"

const char *BUTTON_TAG = "Button_driver";

struct Button_driver
{
    gpio_num_t _gpio_num;
    LED_driver *_led_driver;
};

void button_pressed_event(void *args)
{
    Button_driver *self = (Button_driver *)args;
    led_driver_toggle(self->_led_driver);
}

void button_driver_construct(Button_driver *self, gpio_num_t gpio_num, LED_driver *led_driver)
{
    self->_gpio_num = gpio_num;
    self->_led_driver = led_driver;
}

Button_driver *button_driver_create(gpio_num_t gpio_num, LED_driver *led_driver)
{
    Button_driver *result = (Button_driver *)malloc(sizeof(Button_driver));
    button_driver_construct(result, gpio_num, led_driver);
    return result;
}

void button_driver_delete(Button_driver *self)
{
    if (self)
    {
        ESP_ERROR_CHECK(gpio_reset_pin(self->_gpio_num));
        ESP_ERROR_CHECK(gpio_isr_handler_remove(self->_gpio_num));
        free(self);
    }
}

void button_driver_init(Button_driver *self)
{
    ESP_ERROR_CHECK(gpio_reset_pin(self->_gpio_num));
    ESP_ERROR_CHECK(gpio_set_direction(self->_gpio_num, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_intr_type(self->_gpio_num, GPIO_INTR_NEGEDGE));

    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_LOWMED));
    ESP_ERROR_CHECK(gpio_isr_handler_add(self->_gpio_num, button_pressed_event, self));
    ESP_LOGI(BUTTON_TAG, "Button interrupt successfully initialized for GPIO %i", self->_gpio_num);
}