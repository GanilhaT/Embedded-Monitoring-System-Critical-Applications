#include <stdio.h>
#include <unistd.h>

#include "cidlogging.h"

void app_main(void)
{
	CARDIO_LOGGING_INIT();

	char *TAG = "LOG_TEST";
	int count = 0;
	int lower_bound = 10;
	int upper_bound = 60;
	CARDIO_LOG("STATUS", "SENSOR=ON", 2);
	while (count < 10)
	{
		CARDIO_LOG(TAG, "Warning Log", 1);

		int cpu = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
		int cpu2 = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
		char formatted_string[20]; // Adjust the size as needed based on your expected string length
		sprintf(formatted_string, "CPU0=%d CPU1=%d", cpu, cpu2);
		CARDIO_LOG("TEMPERATURE", formatted_string, 2);

		cpu = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
		sprintf(formatted_string, "SENSOR=%d", cpu);
		CARDIO_LOG("TEMPERATURE", formatted_string, 2);

		count++; // Increment the count by 1 in each iteration

		sleep(10);
	}
	CARDIO_LOG("STATUS", "SENSOR=OFF", 2);
	// CARDIO_LOG(TAG, "Error Log", 0);
	SEND_LOG_OVER_SSH();

	while (1)
	{
		// Sleep for 10 seconds
		sleep(10);
		printf("Terminated...");
	}
}
