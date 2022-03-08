#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_netif.h"
#include "esp_wifi_default.h"
#include "string"

class wifi_component
{
public:
    wifi_component(uint8_t ssid[32], uint8_t password[64]);
    void init();
    void setScanMethod(wifi_scan_method_t scan_method);
    void setChannel(uint8_t channel);
    void setSortMethod(wifi_sort_method_t sort_method);
    void setMinimumRssi(int8_t rssi);
    void setAuthMode(wifi_auth_mode_t authmode);
    void setPMFMode(bool capable, bool required);

private:
    uint8_t _ssid[32];                   /**< SSID of target AP. */
    uint8_t _password[64];               /**< Password of target AP. */
    wifi_scan_method_t _scan_method = 0; /**< do all channel scan or fast scan */
    bool _bssid_set = 0;                 /**< whether set MAC address of target AP or not. Generally, station_config.bssid_set needs to be 0; and it needs to be 1 only when users need to check the MAC address of the AP.*/
    uint8_t _bssid[6];                   /**< MAC address of target AP*/
    uint8_t _channel = 0;                /**< channel of target AP. Set to 1~13 to scan starting from the specified channel before connecting to AP. If the channel of AP is unknown, set it to 0.*/
    uint16_t _listen_interval = 0;       /**< Listen interval for ESP32 station to receive beacon when WIFI_PS_MAX_MODEM is set. Units: AP beacon intervals. Defaults to 3 if set to 0. */
    wifi_sort_method_t _sort_method = 0; /**< sort the connect AP in the list by rssi or security mode */
    int8_t _rssi;                        /**< The minimum rssi to accept in the fast scan mode */
    wifi_auth_mode_t _authmode = 0;      /**< The weakest authmode to accept in the fast scan mode */
    bool _capable = false;               /**< Advertizes support for Protected Management Frame. Device will prefer to connect in PMF mode if other device also advertizes PMF capability. */
    bool _required = false;              /**< Advertizes that Protected Management Frame is required. Device will not associate to non-PMF capable devices. */
    uint32_t _rm_enabled = 1;            /**< Whether Radio Measurements are enabled for the connection */
    uint32_t _btm_enabled = 1;           /**< Whether BSS Transition Management is enabled for the connection */
    uint32_t _mbo_enabled = 1;           /**< Whether MBO is enabled for the connection */
    uint32_t _reserved = 29;             /**< Reserved for future feature set */
}