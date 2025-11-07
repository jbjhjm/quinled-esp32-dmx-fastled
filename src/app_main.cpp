#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "constants.hpp"
#include "dmx_input_queue.hpp"
#include "esp_dmx.h"
#include "esp_log.h"
#include "spi_output_task.hpp"
#include "dmx_input_task.hpp"
#include <FastLED.h>

static const char *TAG = "MAIN";


TaskHandle_t dmxTaskHandle = NULL;
TaskHandle_t fastLedTaskHandle = NULL;


void wait_for_serial_connection() {
	uint32_t timeout_end = millis() + 2000;
	Serial.begin(115200);
	while(!Serial && timeout_end > millis()) {}  //wait until the connection to the PC is established
}

extern "C" void app_main() {
	wait_for_serial_connection(); // Optional, but seems to help Teensy out a lot.
	ESP_LOGI(TAG, "=== serial connection established ===");

	if(!createDmxInputQueue()) return;
	
	prepareDmxTask();
	ESP_LOGI(TAG, "DMX configurated");
	
	prepareSpiOutputTask();
	ESP_LOGI(TAG, "FastLED configurated");

	xTaskCreatePinnedToCore( // xTaskCreate() would run on random core
		dmxTask, 
		"DMX",
		2048, // stack size
		NULL, // task parameters
		1, // priority
		&dmxTaskHandle,
		1 // core number to run on (0|1)
	);
	ESP_LOGI(TAG, "created DMX task");

	xTaskCreatePinnedToCore( // xTaskCreate() would run on random core
		spiOutputTask, 
		"spiOut",
		2048, // stack size
		NULL, // task parameters
		1, // priority
		&fastLedTaskHandle,
		0 // core number to run on (0|1)
	);
	ESP_LOGI(TAG, "created FastLED task");

	// while(true) {
	// 	fastledTask();
	// }

}