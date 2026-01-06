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

const uint8_t gammaR[] = {
0, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 89, 89, 89, 89, 89, 89, 89, 90, 90, 90, 90, 90, 91, 91, 91, 91, 91, 92, 92, 92, 92, 92, 93, 93, 93, 93, 94, 94, 94, 95, 95, 95, 95, 96, 96, 96, 97, 97, 97, 98, 98, 98, 99, 99, 99, 100, 100, 100, 101, 101, 102, 102, 102, 103, 103, 104, 104, 104, 105, 105, 106, 106, 107, 107, 107, 108, 108, 109, 109, 110, 110, 111, 111, 112, 112, 113, 113, 114, 114, 115, 115, 116, 117, 117, 118, 118, 119, 119, 120, 121, 121, 122, 122, 123, 124, 124, 125, 125, 126, 127, 127, 128, 129, 129, 130, 131, 131, 132, 133, 133, 134, 135, 135, 136, 137, 138, 138, 139, 140, 141, 141, 142, 143, 144, 144, 145, 146, 147, 147, 148, 149, 150, 151, 151, 152, 153, 154, 155, 156, 156, 157, 158, 159, 160, 161, 162, 163, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 208, 209, 210, 211, 212, 213, 214, 215, 217, 218, 219, 220, 221, 222, 224, 225, 226, 227, 228, 230, 231, 232, 233, 235, 236, 237, 238, 240, 241, 242, 243, 245, 246, 247, 248, 250, 251, 252, 254, 255
};

const uint8_t gammaG[] = {
0, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 85, 85, 85, 85, 85, 85, 85, 85, 86, 86, 86, 86, 86, 86, 87, 87, 87, 87, 87, 87, 88, 88, 88, 88, 88, 89, 89, 89, 89, 90, 90, 90, 90, 91, 91, 91, 91, 92, 92, 92, 92, 93, 93, 93, 94, 94, 94, 95, 95, 95, 95, 96, 96, 96, 97, 97, 98, 98, 98, 99, 99, 99, 100, 100, 100, 101, 101, 102, 102, 102, 103, 103, 104, 104, 104, 105, 105, 106, 106, 107, 107, 108, 108, 108, 109, 109, 110, 110, 111, 111, 112, 112, 113, 113, 114, 114, 115, 115, 116, 116, 117, 118, 118, 119, 119, 120, 120, 121, 121, 122, 123, 123, 124, 124, 125, 125, 126, 127, 127, 128, 129, 129, 130, 130, 131, 132, 132, 133, 134, 134, 135, 136, 136, 137, 138, 138, 139, 140, 140, 141, 142, 142, 143, 144, 145, 145, 146, 147, 148, 148, 149, 150, 151, 151, 152, 153, 154, 154, 155, 156, 157, 157, 158, 159, 160, 161, 161, 162, 163, 164, 165, 166, 166, 167, 168, 169, 170, 171, 172, 172, 173, 174, 175, 176, 177, 178, 179, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220
};

const uint8_t gammaB[] = {
0, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 85, 85, 85, 85, 85, 85, 85, 86, 86, 86, 86, 86, 86, 87, 87, 87, 87, 87, 88, 88, 88, 88, 89, 89, 89, 89, 90, 90, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 93, 93, 94, 94, 95, 95, 95, 96, 96, 96, 97, 97, 97, 98, 98, 99, 99, 100, 100, 100, 101, 101, 102, 102, 103, 103, 103, 104, 104, 105, 105, 106, 106, 107, 107, 108, 108, 109, 109, 110, 111, 111, 112, 112, 113, 113, 114, 114, 115, 116, 116, 117, 117, 118, 119, 119, 120, 120, 121, 122, 122, 123, 124, 124, 125, 126, 126, 127, 128, 128, 129, 130, 130, 131, 132, 133, 133, 134, 135, 136, 136, 137, 138, 139, 139, 140, 141, 142, 143, 143, 144, 145, 146, 147, 147, 148, 149, 150, 151, 152, 152, 153, 154, 155, 156, 157, 158, 159, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 175, 176, 177, 178, 179, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 199, 200, 201, 202, 203, 204, 205, 206, 208, 209, 210, 211, 212, 213, 215, 216, 217, 218, 219, 221, 222, 223, 224, 225, 227, 228, 229, 230, 232, 233, 234, 235, 237, 238, 239, 240, 242, 243, 244, 246, 247, 248, 250, 251, 252, 254, 255
};


// struct DmxInputMap {
//     uint8_t dimmer;
// 	uint8_t color1[3];
// 	uint8_t color2[NUM_CHANNELS_PER_COLOR];
// };

unsigned int mapDimmer(unsigned int input) {
	return input;
}

float applyDimmingMultiplier(uint8_t value, uint8_t dimmer, ColorChannel colorChannel) {

	
	// for low brightness values, the LEDs will show no output.
	// thus, we need to remap the output value to be high enough to start output.
	// const float minimumValue = 80.0;
	// const float maximumValue = 255.0;
	
	const float dimmed = value * dimmer / 255.0 ;
	const int lutIndex = std::round(dimmed);
	if(colorChannel==ColorChannel::R ) return gammaR[lutIndex];
	if(colorChannel==ColorChannel::G ) return gammaG[lutIndex];
	return gammaB[lutIndex];

	// if(dimmed <= 0.5) return 0.0;
	// // pgm_read_byte(&gammaR[buf[0]]); 
	
	// const float projectedValue = (((dimmed)) * (maximumValue - minimumValue) / 255.0); // WITHOUT adding minimumValue!
	// const float projectedValueInterp = projectedValue * projectedValue / 255.0;
	// const float mapped = minimumValue + projectedValueInterp;
	// return mapped;
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
	const uint8_t multiplier = source[0];

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
					"Out1 CHannels: Dim: %i -> %i - raw RGB input: %i %i %i - final RGB: %f %f %f", 
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