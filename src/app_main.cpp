#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_dmx.h"
#include "esp_log.h"

#define ESP32
#include <FastLED.h>
#include "constants.hpp"


CRGB leds[NUM_LEDS];
static const char *TAG = "MAIN";  // The log tagline.
static uint8_t data[DMX_PACKET_SIZE] = {};  // Buffer to store DMX data

dmx_packet_t packet;
bool is_connected = false;
int packet_count = 0;
TickType_t last_update = xTaskGetTickCount();

TaskHandle_t dmxTaskHandle = NULL;
TaskHandle_t fastLedTaskHandle = NULL;

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
	static const char *TAGDMX = "DMX";  // The log tagline.
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
					dmx_read(DMX_NUM_1, data, DMX_PACKET_SIZE);
					writeBufferValuesToStringAsHex(dmxDebugData, data, 0, 8, sizeof(dmxDebugData));
					// const String dmxData = "127 125 000 000";
					// ESP_LOGI(TAGDMX, "%s", dmxData.c_str());
					ESP_LOGI(TAGDMX, "Start code: %02x, Size: %i, data: %s", packet.sc, packet.size, dmxDebugData); 
					// ESP_LOG_BUFFER_HEX(TAG, data, 8);  // Log first 8 bytes 
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

void wait_for_serial_connection() {
	uint32_t timeout_end = millis() + 2000;
	Serial.begin(115200);
	while(!Serial && timeout_end > millis()) {}  //wait until the connection to the PC is established
}

void fastledTask(void *pvParameters) {
	static const char *TAGFASTLED = "DMX";  // The log tagline.
	const TickType_t xDelay = 1000;
	for(;;) {
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

void shutdown() {
	ESP_LOGI(TAG, "Uninstalling the DMX driver.");
	dmx_driver_delete(DMX_NUM_1);
}



extern "C" void app_main() {
	wait_for_serial_connection(); // Optional, but seems to help Teensy out a lot.
	ESP_LOGI(TAG, "=== serial connection established ===");
	
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
		fastledTask, 
		"FastLED",
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