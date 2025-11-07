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
static CRGB leds[NUM_TOTAL_LEDS];
const int maxDmxInputRGBs = (int)((DMX_NUM_CHANNELS - 1) / 3);

// struct DmxInputMap {
//     uint8_t dimmer;
// 	uint8_t color1[3];
// 	uint8_t color2[NUM_CHANNELS_PER_COLOR];
// };


void updateDmxMap(uint8_t (&source)[DMX_NUM_CHANNELS], CRGB (&target)[NUM_TOTAL_LEDS]) {
	// ESP_LOGI(TAGFASTLED, "received new DMX buffer");
	FastLED.setBrightness((unsigned int) source[0]);
	// const unsigned int dim = (unsigned int) source[0];
	// const float multiplier = dim / 255;
	CRGB tmp;
	for(int i=0; i<NUM_TOTAL_LEDS && i<maxDmxInputRGBs; i++) {
		target[i].setRGB(
			(unsigned int) source[(i*3)+1],
			(unsigned int) source[(i*3)+2],
			(unsigned int) source[(i*3)+3]
		);
		// target[i] = tmp;
	}
}

void spiOutputTask(void *pvParameters) {
	uint8_t dmxInput[DMX_NUM_CHANNELS];  // Buffer to store DMX data

	const TickType_t xDelay = 10;
	TickType_t last_update = xTaskGetTickCount();

	for(;;) {
		const TickType_t now = xTaskGetTickCount();
		if (now - last_update >= pdMS_TO_TICKS(SPI_REFRESH_MS)) {
			bool newInput = xQueueReceive(dmxQueue, &dmxInput, 0);
			if(newInput) {
				updateDmxMap(dmxInput, leds);
				FastLED.show();
				ESP_LOGI(TAGFASTLED, "Out1 CHannels: %i %i %i", leds[0].r, leds[0].g, leds[0].b);
			}
		}
	}
}

void spiOutputTastTask(void *pvParameters) {
	uint8_t dmxInput[DMX_NUM_CHANNELS];  // Buffer to store DMX data

	const TickType_t xDelay = 10;
	TickType_t last_update = xTaskGetTickCount();

	for(;;) {
		ESP_LOGI(TAGFASTLED,"to red");
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
		
	FastLED.addLeds<LED_TYPE, LED_PIN1, COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP*0, NUM_LEDS_PER_STRIP).setCorrection(UncorrectedColor);
	FastLED.addLeds<LED_TYPE, LED_PIN2, COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP*1, NUM_LEDS_PER_STRIP).setCorrection(UncorrectedColor);
	FastLED.addLeds<LED_TYPE, LED_PIN3, COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP*2, NUM_LEDS_PER_STRIP).setCorrection(UncorrectedColor);
	FastLED.addLeds<LED_TYPE, LED_PIN4, COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP*3, NUM_LEDS_PER_STRIP).setCorrection(UncorrectedColor);
	FastLED.addLeds<LED_TYPE, LED_PIN5, COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP*4, NUM_LEDS_PER_STRIP).setCorrection(UncorrectedColor);
	// FastLED.addLeds<NEOPIXEL, 2>(leds, 0, NUM_LEDS_PER_STRIP);

	FastLED.setBrightness(BRIGHTNESS);

}