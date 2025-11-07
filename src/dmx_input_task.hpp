#pragma once
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "constants.hpp"
#include "dmx_input_queue.hpp"
#include "esp_dmx.h"
#include "esp_log.h"

static const char *TAGDMX = "DMX";  // The log tagline.

dmx_packet_t packet;
bool is_connected = false;
int packet_count = 0;
TickType_t last_update = xTaskGetTickCount();


// extern QueueHandle_t dmxQueue;

// similar to ESP_LOG_BUFFER_HEX, but does not write to serial and has some extras like reading from a offset
void writeBufferValuesToStringAsHex(char * out, uint8_t* source, int sourceOffset, int numBytesToPrint, int maxSize) {
	size_t offset;
	memset(out, 0, maxSize); // empty the out chars so that we can start clean
	for (int i = 0; i < numBytesToPrint; i++) {
		offset = strlen(out);
		snprintf(&out[offset], maxSize - offset, "%x ", source[sourceOffset+i]);
	};
}


void dmxTask(void *pvParameters) {
	int core = xPortGetCoreID();
	ESP_LOGI(TAGDMX,"begin to wait for DMX input");
	char dmxDebugData[40] = "";
	
	while (true) {
		// vTaskDelay(1000);

		// Block until a packet is received
		if (dmx_receive(DMX_NUM_1, &packet, DMX_TIMEOUT_TICK)) {
			const TickType_t now = xTaskGetTickCount();
			++packet_count;
			
			if (!is_connected) {
				// Log when we first connect
				ESP_LOGI(TAGDMX, "DMX is connected.");
				is_connected = true;
			}
			
			// throttle updates  1000ms
			if (now - last_update >= pdMS_TO_TICKS(DMX_REFRESH_MS)) {
				if (packet.err == DMX_OK) {
					uint8_t data[DMX_NUM_CHANNELS] = {};  // Buffer to store DMX data
					dmx_read_offset(DMX_NUM_1, DMX_START_OFFSET, data, DMX_NUM_CHANNELS);
					writeBufferValuesToStringAsHex(dmxDebugData, data, 0, 8, sizeof(dmxDebugData));
					xQueueSend(dmxQueue, &data, 0);
					// xQueueOverwrite(dmxQueue, &data);

					// note that package size will likely be 513 all the time cause thats the standard for DMX data...
					ESP_LOGI(TAGDMX, "Received packet size: %i, selecting %i-%i, data: %s [...]", 
						packet.size, DMX_START_OFFSET, DMX_START_OFFSET+DMX_NUM_CHANNELS, dmxDebugData 
					); 
				} else {
					// may happen when adding physical dmx connection?

					ESP_LOGI(TAGDMX, "An error occurred receiving DMX!");
				}
			}
				
		} else if (is_connected) {
			// DMX timed out after having been previously connected
			ESP_LOGI(TAGDMX, "DMX was disconnected.");
			// breaking ends task and enforces an application restart.
			// break; 
		} else  {
			ESP_LOGI(TAGDMX, "no connection found.");
		}
	}
}

