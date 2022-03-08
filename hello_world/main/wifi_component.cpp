#include "wifi_component.hpp"

const char* TAG = "wifi_component";

wifi_component::wifi_component(uint8_t ssid[32], uint8_t password[64])
{
    
}

void wifi_component::init()
{
    wifi_sta_config_t sta_conf;
    esp_wifi_scan_start()
}

void wifi_component::setScanMethod(wifi_scan_method_t scan_method)
{
    _scan_method = scan_method;
}

void wifi_component::setChannel(uint8_t channel)
{
    if(channel > 13 && channel < 1)
    {
        ESP_LOGE(TAG, "Error. setChannel()-function is trying to change channel outside of channel range (1-13)");
        return;
    }
    _channel = channel;
}

void wifi_component::setSortMethod(wifi_sort_method_t sort_method)
{
    _sort_method = sort_method;
}

void wifi_component::setMinimumRssi(int8_t rssi)
{
    _rssi = rssi;
}

void wifi_component::setAuthMode(wifi_auth_mode_t authmode)
{
    _authmode = authmode;
}

void wifi_component::setPMFMode(bool capable, bool required)
{
    _capable = capable;
    _required = required;
}
