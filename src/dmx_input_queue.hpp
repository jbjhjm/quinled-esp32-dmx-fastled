#pragma once
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "constants.hpp"

// #define NUM_CHANNELS_PER_COLOR 3

// define as inline to indicate this is a global var and linker should not create it anywhere else
inline QueueHandle_t dmxQueue;

	// DmxInputMap inputMap;
	// 				inputMap.dimmer = data[0];
	// 				memcpy( &inputMap.color1[0], &data[1], 3*sizeof(uint8_t));
	// 				memcpy( &inputMap.color2[0], &data[4], 3*sizeof(uint8_t));


// struct DmxInputMap {
//     uint8_t dimmer;
// 	uint8_t color1[3];
// 	uint8_t color2[NUM_CHANNELS_PER_COLOR];
// };

bool createDmxInputQueue();

bool creatdmxQueue();
