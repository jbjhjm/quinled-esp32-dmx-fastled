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
const char *TAGFASTLED = "DMX";  // The log tagline.
static CRGB leds[NUM_LEDS];

// struct DmxInputMap {
//     uint8_t dimmer;
// 	uint8_t color1[3];
// 	uint8_t color2[NUM_CHANNELS_PER_COLOR];
// };


void updateDmxMap(uint8_t (&source)[DMX_NUM_CHANNELS], CRGB (&target)[NUM_LEDS]) {
	// ESP_LOGI(TAGFASTLED, "received new DMX buffer");
	FastLED.setBrightness((unsigned int) source[0]);
	// const unsigned int dim = (unsigned int) source[0];
	// const float multiplier = dim / 255;
	CRGB tmp;
	for(int i=0; i<NUM_LEDS; i++) {
		target[i].setRGB(
			(unsigned int) source[(i*3)+1],
			(unsigned int) source[(i*3)+2],
			(unsigned int) source[(i*3)+3]
		);
		// target[i] = tmp;
	}
}

void spiOutputTask(void *pvParameters) {

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

void prepareSpiOutputTask() {
		
	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(UncorrectedColor);
	FastLED.setBrightness(BRIGHTNESS);

}