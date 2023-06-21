#include <stdio.h>
#include "esp_err.h"
#include "esp_ipc.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#include "cidlogging.h"

void app_main(void)
{
	/*
	char* TAG = "MyModule";
	esp_ipc_call(1, CARDIO_LOG, TAG);
	*/

	CARDIO_LOGGING_INIT();

	char *TAG = "LOG_TEST";
	CARDIO_LOG(TAG, "Error Log", 0);
	CARDIO_LOG(TAG, "Warning Log", 1);
	CARDIO_LOG(TAG, "Information Log", 2);
	CARDIO_LOG(TAG, "Debug Log", 3);

	SEND_LOG_OVER_SSH();

	while (1)
	{
		// Sleep for 10 seconds
		sleep(10);
		printf("Terminated...");
	}
}
