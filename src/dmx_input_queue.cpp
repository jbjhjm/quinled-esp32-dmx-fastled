#include <stdint.h>
#include "dmx_input_queue.hpp"
#include "esp_log.h"

static const char *TAG = "DMX_QUEUE";  // The log tagline.

bool createDmxInputQueue() {
	dmxQueue = xQueueCreate(1, DMX_NUM_CHANNELS * sizeof(uint8_t));
	if(!dmxQueue){
    	ESP_LOGE(TAG, "Impossible to create the queue");
		return false;
	}
	return true;
}
