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
#include <dirent.h>
#include <dirent.h>

#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "cidLogging.h"
#include "ssh.h"
#include "sntp.h"
#include "utils.h"

TaskHandle_t loggingTaskHandle = NULL;

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
 * @brief Executed function every time an event is logged.
 * The list of arguments is added to a new line on the file.
 * The date time will also be added is a correct connection was established with the SNTP server.
 *
 * @note -
 *
 * @param  fmt 		Format specifier (like %s)
 * @param  list 	List of arguments
 *
 */
static int PRINT_TO_SD_CARD(const char *fmt, va_list list)
{
	if (log_file == NULL)
	{
		return -1;
	}
	// Get timestamp
	char current_date_time[100];
	GET_DATE_TIME(current_date_time, false);

	// Construct the modified format string with the prefix
	char modified_fmt[strlen(current_date_time) + strlen(fmt) + 1];
	sprintf(modified_fmt, "[%s] %s %s", current_date_time, DEVICE_ID, fmt);

	int res = vfprintf(log_file, modified_fmt, list);
	// Committing changes to the file on each write is slower,
	// but ensures that no data will be lost.
	// fsync after might be called every 50 log messages or so,
	// or after 100ms passed since last fsync, and so on.
	fsync(fileno(log_file));

	return res;
}

/**
 * @brief Initializes the SD card
 *
 * @note https://gist.github.com/igrr/82055d824cbbc521932f32217e0710b9
 *
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
	esp_err_t ret = esp_vfs_fat_sdmmc_mount(LOG_FILE_DIR, &host, &slot_config, &mount_config, &card);

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
	// Card has been initialized, print its properties
	sdmmc_card_print_info(stdout, card);
}

/**
 * @brief  Creates a new file with the format cardioid - timestamp and redirects the esp logging output to the created file
 *
 * @note -
 *
 */
void CREATE_LOG_FILE()
{
	char *TAG = "LOGFILE";
	// Open the specified file for reading
	char filename[100];
	// Initialize string
	strcpy(filename, "");
	strcat(filename, LOG_FILE_DIR);
	strcat(filename, "/");
	strcat(filename, DEVICE_ID);
	strcat(filename, "-");
	// Get timestamp to be used to create the file
	char current_date_time[100];
	GET_DATE_TIME(current_date_time, true);
	strcat(filename, current_date_time);
	strcat(filename, ".txt");
	// Set the file stream
	log_file = fopen(filename, "a");
	if (log_file == NULL)
	{
		ESP_LOGE(TAG, "Failed to open file %s for logging! Error code: %d", filename, errno);
	}
	else
	{
		ESP_LOGI(TAG, "File %s opened", filename);
		ESP_LOGI(TAG, "Redirecting log output to SD card!");
		esp_log_set_vprintf(PRINT_TO_SD_CARD);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////// LOG //////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Core function of the logging system. Uses the default esp logging library to create a log statement.
 *
 * @note -
 *
 * @param TAG context of the log event
 * @param message content of the log event
 * @param level type of log event to generate (Error, Warning, Information, Debug)
 *
 */
void CARDIO_LOG(char *TAG, char *message, int level)
{
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
	else if (level == 3)
	{
		ESP_LOGD(TAG, "%s", message);
	}
	else
	{
		ESP_LOGV(TAG, "%s", message);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////// SSH MANAGEMENT ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function to prepare the wifi connection and send to send the current log file (and other pending log files) via an SSH connection to an SFTP server
 *
 * @note -
 *
 */
void SEND_LOG_OVER_SSH()
{
	fclose(log_file);
	log_file = NULL;
	esp_log_set_vprintf(&vprintf);
	// Delete the logging task
	vTaskDelete(loggingTaskHandle);
	WIFI_INIT();
	SSH_INIT();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////// INIT /////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Logging task. Recurrent information to be logged should be defined here. The task will run on one of the cores only.
 *
 * @note -
 *
 * @param args Arguments passed to the task
 *
 */
void LOGGING_TASK(void *arg)
{
	int coreId = xPortGetCoreID();
	// Print the core ID before performing logging
	printf("Logging task running on Core %d\n", coreId);
	while (1)
	{
		// Task logic here
		char *TAG = "LOG_TASK_TEST";
		// CARDIO_LOG(TAG, "Error Log", 0);
		CARDIO_LOG(TAG, "Warning Log", 1);
		CARDIO_LOG(TAG, "Information Log", 2);
		CARDIO_LOG(TAG, "Debug Log", 3);
		CARDIO_LOG(TAG, "Verbose Log", 4);
		vTaskDelay(pdMS_TO_TICKS(10000)); // Delay for 10 seconds
	}
}

/**
 * @brief Initialization of the logging system. Includes:
 * Definition of the log level;
 * Mounting of the SD Card;
 * SNTP initialization;
 * LOG File creation and Log event redirection
 * Logging task creation.
 *
 * @note -
 *
 * @param level Log level to be configured with
 *
 */
void CARDIO_LOGGING_INIT()
{
	// MONITOR_SYSTEM();
	esp_log_level_set("*", ESP_LOG_VERBOSE);
	MOUNT_SD_CARD();
	SNTP_INIT();
	CREATE_LOG_FILE();
	// Create the logging task
	xTaskCreatePinnedToCore(LOGGING_TASK, "LOGGING_TASK", 4096, NULL, 10, &loggingTaskHandle, 1);
	// MONITOR_SYSTEM();
}
