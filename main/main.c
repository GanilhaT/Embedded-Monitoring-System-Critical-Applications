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

	/*

	while (true)
	{
		char *TAG = "MyModule";
		CARDIO_LOG(TAG, "Test", 0);
		sleep(20);
	}

	*/

	SEND_LOG_OVER_SSH();
}
