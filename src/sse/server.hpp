#pragma once

#include "esp_http_server.h"

namespace sse {

httpd_handle_t start_webserver();

esp_err_t stop_webserver(httpd_handle_t server);

void start_async_req_workers();

} // namespace sse
