#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "esp_wifi.h"

#include "utils.h"

/**
 * @brief Utils class to check if a string ends with a specific value
 *
 * @note -
 *
 * @param str String value to be searched on
 * @param suffix Suffix to search on the string passed
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
 * @brief Reset wifi configuration and connection
 *
 * @note -
 *
 */
esp_err_t RESET_WIFI()
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

/**
 * @brief System monitorization function
 *
 * @note -
 *
 */
void MONITOR_SYSTEM()
{

    printf("DRAM usage %d \n", esp_get_free_heap_size());
    printf("Toal RAM usage %d \n", esp_get_free_internal_heap_size());
    // TODO configurable delay
    // vTaskDelay(5000 / portTICK_PERIOD_MS);
}