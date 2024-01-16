#include "sse/server.hpp"
#include "sse/wifi.hpp"

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi_default.h"
#include "nvs_flash.h"

namespace {

constexpr auto tag = "main";

void disconnect_handler(
    void* arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void* event_data
)
{
    auto* server = static_cast<httpd_handle_t*>(arg);
    if (*server != nullptr) {
        ESP_LOGI(tag, "Stopping webserver");
        if (sse::stop_webserver(*server) == ESP_OK) {
            *server = nullptr;
        } else {
            ESP_LOGE(tag, "Failed to stop http server");
        }
    }
}

void connect_handler(
    void* arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void* event_data
)
{
    auto* server = static_cast<httpd_handle_t*>(arg);
    if (*server == nullptr) {
        ESP_LOGI(tag, "Starting webserver");
        *server = sse::start_webserver();
    }
}

} // namespace

extern "C" void app_main()
{
    httpd_handle_t server = nullptr;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();
    sse::wifi_init_softap();

    /* Register event handlers to stop the server when Wi-Fi is
     * disconnected, and re-start it upon connection.
     */
    // maybe these can be moved into sse/wifi code
    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server
    ));
    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server
    ));

    // start workers
    sse::start_async_req_workers();

    /* Start the server for the first time */
    server = sse::start_webserver();
}
