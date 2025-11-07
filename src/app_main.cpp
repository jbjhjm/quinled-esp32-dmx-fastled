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

void shutdown() {
	ESP_LOGI(TAG, "Uninstalling the DMX driver.");
	dmx_driver_delete(DMX_NUM_1);
}

extern "C" void app_main() {
	wait_for_serial_connection(); // Optional, but seems to help Teensy out a lot.
	ESP_LOGI(TAG, "=== serial connection established ===");

	if(!createDmxInputQueue()) return;
	
	dmx_config_t config = DMX_CONFIG_DEFAULT;
	dmx_personality_t personalities[] = {
		{1, "Default Personality"}
	};
	const int personality_count = 1;
	dmx_driver_install(DMX_NUM_1, &config, personalities, personality_count);
	dmx_set_pin(DMX_NUM_1, TX_PIN, RX_PIN, EN_PIN);
	// dmx_set_start_address(DMX_NUM_1, 0);
	// dmx_set_current_personality(DMX_NUM_1, 0);
	ESP_LOGI(TAG, "DMX configurated");
	
	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(UncorrectedColor);
	FastLED.setBrightness(BRIGHTNESS);
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