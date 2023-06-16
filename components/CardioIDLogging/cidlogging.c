#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "cidLogging.h"
#include "ssh.h"
#include "sntp.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////// SD CARD //////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This file stream will be used for logging
static FILE *log_file;
// This example can use SDMMC and SPI peripherals to communicate with SD card.
// By default, SDMMC peripheral is used.
// To enable SPI mode, uncomment the following line:

// #define USE_SPI_MODE

// When testing SD and SPI modes, keep in mind that once the card has been
// initialized in SPI mode, it can not be reinitialized in SD mode without
// toggling power to the card.
#ifdef USE_SPI_MODE
// Pin mapping when using SPI mode.
// With this mapping, SD card can be used both in SPI and 1-line SD mode.
// Note that a pull-up on CS line is required in SD mode.
#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK 14
#define PIN_NUM_CS 13
#endif // USE_SPI_MODE

/**
 * @brief executed function every time an event is logged
 *
 * @note TBD
 *
 * @see TBD
 *
 * @param  fmt 		TBD
 * @param  list 	TBD
 *
 */
static int PRINT_TO_SD_CARD(const char *fmt, va_list list)
{
	if (log_file == NULL)
	{
		return -1;
	}
	int res = vfprintf(log_file, fmt, list);
	// Committing changes to the file on each write is slower,
	// but ensures that no data will be lost.
	// fsync after might be called every 50 log messages or so,
	// or after 100ms passed since last fsync, and so on.
	fsync(fileno(log_file));
	return 0;
}

/**
 * @brief function to initialize the SD card and redirect the logging output to a file
 *
 * @note TBD
 *
 * @see TBD
 */
void MOUNT_SD_CARD()
{
	char *TAG = "SDCARD";
	ESP_LOGI(TAG, "Initializing SD card");

#ifndef USE_SPI_MODE
	ESP_LOGI(TAG, "Using SDMMC peripheral");
	sdmmc_host_t host = SDMMC_HOST_DEFAULT();

	// To use 1-line SD mode, uncomment the following line:
	// host.flags = SDMMC_HOST_FLAG_1BIT;

	// This initializes the slot without card detect (CD) and write protect (WP) signals.
	// Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
	sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

	// GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.
	// Internal pull-ups are not sufficient. However, enabling internal pull-ups
	// does make a difference some boards, so we do that here.
	gpio_set_pull_mode(15, GPIO_PULLUP_ONLY); // CMD, needed in 4- and 1- line modes
	gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);  // D0, needed in 4- and 1-line modes
	gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);  // D1, needed in 4-line mode only
	gpio_set_pull_mode(12, GPIO_PULLUP_ONLY); // D2, needed in 4-line mode only
	gpio_set_pull_mode(13, GPIO_PULLUP_ONLY); // D3, needed in 4- and 1-line modes

#else
	ESP_LOGI(TAG, "Using SPI peripheral");

	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
	slot_config.gpio_miso = PIN_NUM_MISO;
	slot_config.gpio_mosi = PIN_NUM_MOSI;
	slot_config.gpio_sck = PIN_NUM_CLK;
	slot_config.gpio_cs = PIN_NUM_CS;
	// This initializes the slot without card detect (CD) and write protect (WP) signals.
	// Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
#endif // USE_SPI_MODE

	// Options for mounting the filesystem.
	// If format_if_mount_failed is set to true, SD card will be partitioned and
	// formatted in case when mounting fails.
	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
		.format_if_mount_failed = true,
		.max_files = 5};

	// Use settings defined above to initialize SD card and mount FAT filesystem.
	// Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
	// Please check its source code and implement error recovery when developing
	// production applications.
	sdmmc_card_t *card;
	esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

	if (ret != ESP_OK)
	{
		if (ret == ESP_FAIL)
		{
			ESP_LOGE(TAG, "Failed to mount filesystem. "
						  "If you want the card to be formatted, set format_if_mount_failed = true.");
		}
		else
		{
			ESP_LOGE(TAG, "Failed to initialize the card (%d). "
						  "Make sure SD card lines have pull-up resistors in place.",
					 ret);
		}
		return;
	}
	// Set the file stream
	log_file = fopen("/sdcard/log.txt", "a");
	if (log_file == NULL)
	{
		ESP_LOGE(TAG, "Failed to open file for logging!");
	}
	else
	{
		ESP_LOGI(TAG, "Redirecting log output to SD card!");
		esp_log_set_vprintf(&PRINT_TO_SD_CARD);
	}
	// Card has been initialized, print its properties
	sdmmc_card_print_info(stdout, card);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////// LOG //////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief sample function to infinitly generate logs
 *
 * @note TBD
 *
 * @see TBD
 *
 * @param arg text to pass to the log message
 */
void CARDIO_LOG(char *TAG, char *message, int level)
{
	// char *TAG = (char *)arg;
	// char *message = "Test";
	char current_date_time[100];
	int coreId = xPortGetCoreID();
	// Get timestamp
	GET_DATE_TIME(current_date_time);
	// Print the core ID before performing logging
	printf("Logging task running on Core %d\n", coreId);

	if (level == 0)
	{
		ESP_LOGE(TAG, "%s", message);
	}
	else if (level == 1)
	{
		ESP_LOGW(TAG, "%s", message);
	}
	else if (level == 2)
	{
		ESP_LOGI(TAG, "%s", message);
	}
	else
	{
		ESP_LOGD(TAG, "%s", message);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////// SSH MANAGEMENT ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Task to prepare wifi connection and send the log file via TLS connection
 *
 * @note TBD
 *
 * @see TBD
 *
 */
void SEND_LOG_OVER_SSH()
{
	WIFI_INIT();
	SSH_INIT();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////// MAIN /////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialization of the logging system
 *
 * @note TBD
 *
 * @see TBD
 *
 */
void CARDIO_LOGGING_INIT()
{
	SNTP_INIT();
	// MOUNT_SD_CARD();
}
