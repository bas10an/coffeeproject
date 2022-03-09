#include "wifi_station.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define WIFI_CONNECT_MAXIMUM_RETRY 5

const char *WIFI_TAG = "WiFi_Station";

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

struct WiFi_station
{
    uint8_t _ssid[32];
    uint8_t _password[64];
    bool _connected;
};

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    WiFi_station *self = (WiFi_station *)arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_id == WIFI_EVENT_STA_CONNECTED)
    {
        wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t *)event_data;
        ESP_LOGI(WIFI_TAG, "Connected to ssid: %s, password: %s", self->_ssid, self->_password);
        self->_connected = true;
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;
        self->_connected = false;
        ESP_LOGW(WIFI_TAG, "Disonnected from ssid: %s", self->_ssid);
        if (s_retry_num < WIFI_CONNECT_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(WIFI_TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(WIFI_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void WiFi_sta_construct(WiFi_station *self, uint8_t ssid[32], uint8_t password[64])
{
    memcpy(&self->_ssid[0], &ssid[0], sizeof(uint8_t[32]));
    memcpy(&self->_password[0], &password[0], sizeof(uint8_t[64]));
}

WiFi_station *WiFi_sta_create(uint8_t ssid[32], uint8_t password[64])
{
    WiFi_station *result = (WiFi_station *)malloc(sizeof(WiFi_station));
    WiFi_sta_construct(result, ssid, password);
    return result;
}

void WiFi_sta_delete(WiFi_station *self)
{
    if(self)
    {
        free(self);
    }
}

bool WiFi_connected_status(WiFi_station *self)
{
    ESP_LOGW(WIFI_TAG, "WiFi connection status: %s", self->_connected ? "connected" : "disconnected");
    return self->_connected;
}

void WiFi_sta_init(WiFi_station *self)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        self,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        self,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };
    memcpy(&wifi_config.sta.ssid[0], &self->_ssid[0], sizeof(self->_ssid));
    memcpy(&wifi_config.sta.password[0], &self->_password[0], sizeof(self->_password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(WIFI_TAG, "wifi_init_sta finished. Sta ssid: %s, Sta password: %s", wifi_config.sta.ssid, wifi_config.sta.password);

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(WIFI_TAG, "connected to ap SSID:%s password:%s",
                 self->_ssid, self->_password);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(WIFI_TAG, "Failed to connect to SSID:%s, password:%s",
                 self->_ssid, self->_password);
    }
    else
    {
        ESP_LOGE(WIFI_TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}

void WiFi_start_task(void* parameters)
{
    WiFi_station *self = (WiFi_station *)parameters;
    TickType_t xDelay = 100 / portTICK_RATE_MS;

    WiFi_sta_init(self);

    for(;;)
    {
        vTaskDelay(xDelay);
    }
}