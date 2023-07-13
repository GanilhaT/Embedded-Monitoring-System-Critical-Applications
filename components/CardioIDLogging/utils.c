#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "esp_wifi.h"

#include "utils.h"

/**
 *
 */
int ENDSWITH(const char *str, const char *suffix)
{
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if (suffix_len > str_len)
        return 0;

    return strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

/**
 *
 */
esp_err_t wifi_reset_config()
{
    esp_err_t ret = esp_wifi_restore();
    if (ret != ESP_OK)
    {
        return ret;
    }
    ret = esp_wifi_deinit();
    if (ret != ESP_OK)
    {
        return ret;
    }
    return ESP_OK;
}