#pragma once
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "constants.hpp"
#include "dmx_input_queue.hpp"
#include "esp_log.h"
#include <FastLED.h>

// local only
static CRGB leds[NUM_LEDS];

void spiOutputTask(void *pvParameters) {
	uint8_t data[DMX_NUM_CHANNELS];  // Buffer to store DMX data
	static const char *TAGFASTLED = "DMX";  // The log tagline.
	const TickType_t xDelay = 1000;
	for(;;) {
		bool newInput = xQueueReceive(dmxQueue, &data, 0);
		if(newInput) {
			ESP_LOGI(TAGFASTLED, "received new DMX buffer");

		}
		// ESP_LOGI(TAGFASTLED,"to red");
		leds[0] = CRGB::Red;
		FastLED.show();
		vTaskDelay(xDelay);
		
		// ESP_LOGI(TAGFASTLED,"to black");
		leds[0] = CRGB::Black;
		FastLED.show();
		vTaskDelay(xDelay);
	}
}