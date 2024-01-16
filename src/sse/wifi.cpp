#include "sse/wifi.hpp"

#include "esp_log.h"
#include "esp_mac.h"
#include "lwip/inet.h"

namespace sse {
namespace {

constexpr auto tag = "wifi";

} // namespace

void wifi_event_handler(
    void* arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void* event_data
)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        auto* event = static_cast<wifi_event_ap_staconnected_t*>(event_data);
        ESP_LOGI(
            tag,
            "station " MACSTR " join, AID=%d",
            MAC2STR(event->mac),
            event->aid
        );
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        auto* event = static_cast<wifi_event_ap_stadisconnected_t*>(event_data);
        ESP_LOGI(
            tag,
            "station " MACSTR " leave, AID=%d",
            MAC2STR(event->mac),
            event->aid
        );
    }
}

void wifi_init_softap()
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, nullptr
    ));

    wifi_config_t wifi_config = {
        .ap =
            {.ssid = CONFIG_ESP_WIFI_SSID,
             .password = CONFIG_ESP_WIFI_PASSWORD,
             .ssid_len = static_cast<uint8_t>(strlen(CONFIG_ESP_WIFI_SSID)),
             .authmode = WIFI_AUTH_WPA_WPA2_PSK,
             .max_connection = CONFIG_ESP_MAX_STA_CONN},
    };
    if (strlen(CONFIG_ESP_WIFI_PASSWORD) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(
        esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info
    );

    char ip_addr[16];
    inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
    ESP_LOGI(tag, "Set up softAP with IP: %s", ip_addr);

    ESP_LOGI(
        tag,
        "wifi_init_softap finished. SSID:'%s' password:'%s'",
        CONFIG_ESP_WIFI_SSID,
        CONFIG_ESP_WIFI_PASSWORD
    );
}

} // namespace sse
