#include "sse/server.hpp"

#include "esp_log.h"

namespace sse {
namespace {

constexpr auto tag = "server";

esp_err_t index_handler(httpd_req_t* req)
{
    ESP_LOGI(tag, "uri: /");

    const char* html = "<h1>HELLO</h1>"
                       "<div><a href=\"/long\">long</a></div>"
                       "<div><a href=\"/quick\">quick</a></div>";

    httpd_resp_sendstr(req, html);
    return ESP_OK;
}

} // namespace

httpd_handle_t start_webserver()
{
    httpd_handle_t server = nullptr;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.lru_purge_enable = true;

    // It is advisable that httpd_config_t->max_open_sockets >
    // MAX_ASYNC_REQUESTS Why? This leaves at least one socket still available
    // to handle quick synchronous requests. Otherwise, all the sockets will get
    // taken by the long async handlers, and your server will no longer be
    // responsive.
    config.max_open_sockets = CONFIG_MAX_ASYNC_REQUESTS + 1;

    ESP_LOGI(tag, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGI(tag, "Error starting server!");
        return nullptr;
    }

    const httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = nullptr,
    };

    httpd_register_uri_handler(server, &index_uri);

    return server;
}

esp_err_t stop_webserver(httpd_handle_t server)
{
    return httpd_stop(server);
}

void start_async_req_workers() {}

} // namespace sse
