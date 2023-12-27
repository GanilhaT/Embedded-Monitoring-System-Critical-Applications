#include <stdio.h>
#include "esp_err.h"
#include "esp_ipc.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#include "cidlogging.h"

#define TOTAL_CALL_AMOUNT 200
#define PERFMON_TRACELEVEL -1 // -1 - will ignore trace level

void app_main(void)
{
	// Test commit
	CARDIO_LOGGING_INIT();

	char *TAG = "LOG_TEST";
	CARDIO_LOG(TAG, "Warning Log", 1);
	CARDIO_LOG(TAG, "Information Log", 2);

	/* The while loop was used to test long logging sessions
	while (1) 
	{
		CARDIO_LOG(TAG, "Warning Log", 1);
		CARDIO_LOG(TAG, "Information Log", 2);
		sleep(1);
	}
	*/
	//CARDIO_LOG(TAG, "Error Log", 0);
	//CARDIO_LOG(TAG, "Debug Log", 3);
	//CARDIO_LOG(TAG, "Verbose Log", 4);

	SEND_LOG_OVER_SSH();

	while (1)
	{
		// Sleep for 10 seconds
		sleep(10);
		printf("Terminated...");
	}
}
