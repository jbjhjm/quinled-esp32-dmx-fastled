#include <stdint.h>
#include "dmx_input_queue.hpp"
#include "esp_log.h"

static const char *TAG = "DMX_QUEUE";  // The log tagline.

bool createDmxInputQueue() {
	dmxQueue = xQueueCreate(1, sizeof(uint8_t) * DMX_NUM_CHANNELS);
	if(!dmxQueue){
    	ESP_LOGE(TAG, "Impossible to create the queue");
		return false;
	}
	return true;
}
