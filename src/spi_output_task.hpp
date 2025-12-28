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
const char *TAGFASTLED = "FASTLED";  // The log tagline.
static CRGB ledOutputValues[NUM_LED_OUTPUTS_TOTAL];
const int maxDmxInputRGBs = (int)((DMX_NUM_CHANNELS - 1) / 3);

enum ColorChannel {
	R,
	G,
	B
};

// const uint8_t PROGMEM gamma8[] = {
//     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
//     1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
//     2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
//     5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
//    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
//    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
//    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
//    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
//    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
//    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
//    90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
//   115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
//   144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
//   177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
//   215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 
// };

// const uint8_t PROGMEM gammaR[] = {
//     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,
//     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,
//     2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,
//     5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,
//     9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14,
//    15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
//    23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33,
//    33, 34, 35, 36, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 45, 46,
//    46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
//    62, 63, 65, 66, 67, 68, 69, 70, 71, 73, 74, 75, 76, 78, 79, 80,
//    81, 83, 84, 85, 87, 88, 89, 91, 92, 94, 95, 97, 98, 99,101,102,
//   104,105,107,109,110,112,113,115,116,118,120,121,123,125,127,128,
//   130,132,134,135,137,139,141,143,145,146,148,150,152,154,156,158,
//   160,162,164,166,168,170,172,174,177,179,181,183,185,187,190,192,
//   194,196,199,201,203,206,208,210,213,215,218,220,223,225,227,230 };
  
// const uint8_t PROGMEM gammaG[] = {
//     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
//     1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
//     2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
//     5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
//    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
//    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
//    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
//    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
//    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
//    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
//    90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
//   115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
//   144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
//   177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
//   215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 }; 


// const uint8_t PROGMEM gammaB[] = {
//     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,
//     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,
//     2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,
//     4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  8,
//     8,  8,  8,  9,  9,  9, 10, 10, 10, 10, 11, 11, 12, 12, 12, 13,
//    13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 19,
//    20, 20, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28,
//    29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
//    40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 51, 52, 53,
//    54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 69, 70,
//    71, 72, 73, 74, 75, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
//    90, 92, 93, 94, 96, 97, 98,100,101,103,104,106,107,109,110,112,
//   113,115,116,118,119,121,122,124,126,127,129,131,132,134,136,137,
//   139,141,143,144,146,148,150,152,153,155,157,159,161,163,165,167,
//   169,171,173,175,177,179,181,183,185,187,189,191,193,196,198,200 };


// struct DmxInputMap {
//     uint8_t dimmer;
// 	uint8_t color1[3];
// 	uint8_t color2[NUM_CHANNELS_PER_COLOR];
// };

unsigned int mapDimmer(unsigned int input) {
	return input;
}

float applyDimmingMultiplier(uint8_t value, float dimmer, ColorChannel colorChannel) {
	const float dimmed = dimmer * value;
	if(dimmed < 1.0) return 0.0;

	// pgm_read_byte(&gammaR[buf[0]]); 
	
	// for low brightness values, the LEDs will show no output.
	// thus, we need to remap the output value to be high enough to start output.
	const float minimumValue = 38.0;
	const float maximumValue = 255.0;
	const float mapped = minimumValue + (((dimmed) / 255.0) * (maximumValue - minimumValue));
	const float quadraticInterpolation = mapped * mapped / 255.0;
	return quadraticInterpolation;
	// return (uint8_t) std::round(mapped);
	// const uint8_t fromLookupMap = colorChannel==ColorChannel::R 
	// 	? pgm_read_byte(&gammaR[mapped]) 
	// 	: colorChannel==ColorChannel::G 
	// 		? pgm_read_byte(&gammaG[mapped]) 
	// 		: pgm_read_byte(&gammaB[mapped]);
}



void updateDmxMap(uint8_t (&source)[DMX_NUM_CHANNELS], CRGB (&target)[NUM_LED_OUTPUTS_TOTAL]) {
	// ESP_LOGI(TAGFASTLED, "received new DMX buffer");
	// FastLED.setBrightness((unsigned int) source[0]);
	const unsigned int dim = mapDimmer(source[0]);
	const float multiplier = source[0] / 255.0;

	CRGB tmp;
	int sourceOffset;
	for(int currBlockIndex=0; currBlockIndex<NUM_LED_BLOCKS_TOTAL; currBlockIndex++) {
		sourceOffset = currBlockIndex*3 + 1;
		if(sourceOffset+3 >= DMX_NUM_CHANNELS) break;
		for(int currLedIndex=0; currLedIndex<NUM_LED_BLOCK_SIZE; currLedIndex++) {
			const int currentAbsoluteIndex = currBlockIndex * NUM_LED_BLOCK_SIZE + currLedIndex;
			if(SIMULATE) {
				if(currentAbsoluteIndex!=0) continue;
				ESP_LOGI(TAGFASTLED, 
					"Out1 CHannels: Dim: %i -> %f - raw RGB input: %i %i %i - final RGB: %f %f %f", 
					source[0],
					multiplier,
					source[(sourceOffset)],
					source[(sourceOffset)+1],
					source[(sourceOffset)+2],
					applyDimmingMultiplier(source[(sourceOffset)], multiplier, ColorChannel::R),
					applyDimmingMultiplier(source[(sourceOffset)+1], multiplier, ColorChannel::G),
					applyDimmingMultiplier(source[(sourceOffset)+2], multiplier, ColorChannel::B)
				);
			} else {
				target[currentAbsoluteIndex].setRGB(
					std::round(applyDimmingMultiplier(source[(sourceOffset)], multiplier, ColorChannel::R)),
					std::round(applyDimmingMultiplier(source[(sourceOffset)+1], multiplier, ColorChannel::G)),
					std::round(applyDimmingMultiplier(source[(sourceOffset)+2], multiplier, ColorChannel::B))
				);
			}
		}
		// target[i] = tmp;
	}
}

void spiOutputTask(void *pvParameters) {
	uint8_t dmxInput[DMX_NUM_CHANNELS];  // Buffer to store DMX data

	const TickType_t xDelay = 10;
	TickType_t last_update = xTaskGetTickCount();
	const TickType_t updateAfterTicks = pdMS_TO_TICKS(SPI_REFRESH_MS);
	ESP_LOGI(TAGFASTLED, "updateAfterTicks %i", updateAfterTicks);

	for(;;) {
		// if ((now - last_update) >= pdMS_TO_TICKS(SPI_REFRESH_MS)) {
		// ESP_LOGI(TAGFASTLED, "Refresh %i", now);
		bool newInput = xQueueReceive(dmxQueue, &dmxInput, 0);
		if(newInput == pdPASS) {
			const TickType_t now = xTaskGetTickCount();
			ESP_LOGI(TAGFASTLED, "New Input available, ticks since last update: %i", now - last_update);
			// 	ESP_LOGI(TAGFASTLED, "Out1 CHannels: %i %i %i", ledOutputValues[0].r, ledOutputValues[0].g, ledOutputValues[0].b);
			updateDmxMap(dmxInput, ledOutputValues);
			if(!SIMULATE) FastLED.show();
			last_update = xTaskGetTickCount();
		}

		vTaskDelay(20);
		// }
		// vTaskDelay(xDelay);
	}
}

void spiOutputTastTask(void *pvParameters) {
	uint8_t dmxInput[DMX_NUM_CHANNELS];  // Buffer to store DMX data

	const TickType_t xDelay = 10;
	TickType_t last_update = xTaskGetTickCount();

	for(;;) {
		ESP_LOGI(TAGFASTLED,"to red");
		ledOutputValues[0] = CRGB::Red;
		FastLED.show();
		vTaskDelay(xDelay);
		
		// ESP_LOGI(TAGFASTLED,"to black");
		ledOutputValues[0] = CRGB::Black;
		FastLED.show();
		vTaskDelay(xDelay);
		last_update = xTaskGetTickCount();
	}
}

void prepareSpiOutputTask() {

	
		
	FastLED.addLeds<LED_TYPE, LED_PIN1, COLOR_ORDER>(ledOutputValues, NUM_LEDS_PER_STRIP*0, NUM_LEDS_PER_STRIP).setCorrection(UncorrectedColor);
	FastLED.addLeds<LED_TYPE, LED_PIN2, COLOR_ORDER>(ledOutputValues, NUM_LEDS_PER_STRIP*1, NUM_LEDS_PER_STRIP).setCorrection(UncorrectedColor);
	FastLED.addLeds<LED_TYPE, LED_PIN3, COLOR_ORDER>(ledOutputValues, NUM_LEDS_PER_STRIP*2, NUM_LEDS_PER_STRIP).setCorrection(UncorrectedColor);
	FastLED.addLeds<LED_TYPE, LED_PIN4, COLOR_ORDER>(ledOutputValues, NUM_LEDS_PER_STRIP*3, NUM_LEDS_PER_STRIP).setCorrection(UncorrectedColor);
	FastLED.addLeds<LED_TYPE, LED_PIN5, COLOR_ORDER>(ledOutputValues, NUM_LEDS_PER_STRIP*4, NUM_LEDS_PER_STRIP).setCorrection(UncorrectedColor);
	// FastLED.addLeds<NEOPIXEL, 2>(ledOutputValues, 0, NUM_LEDS_PER_STRIP);

	FastLED.setBrightness(100);

}