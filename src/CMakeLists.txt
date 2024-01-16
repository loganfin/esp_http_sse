idf_component_register(SRCS
                           "main.cpp"
                           "sse/server.cpp"
                           "sse/wifi.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES
                           esp_http_server
                           nvs_flash
                           esp_netif
                           esp_wifi
                           )
