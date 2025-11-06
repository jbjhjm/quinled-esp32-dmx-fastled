#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_dmx.h"
#include "esp_log.h"

#define ESP32
#include <FastLED.h>
// #include "esp_system.h"
// #include "freertos/task.h"

#define TX_PIN 15  // The DMX transmit pin.
#define RX_PIN 14  // The DMX receive pin.
#define EN_PIN 16  // The DMX transmit enable pin.


#define LED_PIN     0
#define NUM_LEDS    2
#define BRIGHTNESS  150
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define DMX_REFRESH_MS 20

CRGB leds[NUM_LEDS];
static const char *TAG = "APP_MAIN";  // The log tagline.
static uint8_t data[DMX_PACKET_SIZE] = {};  // Buffer to store DMX data

dmx_packet_t packet;
bool is_connected = false;
int packet_count = 0;
TickType_t last_update = xTaskGetTickCount();

TaskHandle_t dmxTaskHandle = NULL;
TaskHandle_t fastLedTaskHandle = NULL;

void dmxTask(void *pvParameters) {
	int core = xPortGetCoreID();
	Serial.print("begin to wait for DMX input");

	while (true) {
		// vTaskDelay(1000);

		// Block until a packet is received
		if (dmx_receive(DMX_NUM_1, &packet, DMX_TIMEOUT_TICK)) {
			const TickType_t now = xTaskGetTickCount();
			++packet_count;
			
			if (!is_connected) {
				// Log when we first connect
				ESP_LOGI(TAG, "DMX is connected.");
				is_connected = true;
			}
			
			// throttle updates  1000ms
			if (now - last_update >= pdMS_TO_TICKS(DMX_REFRESH_MS)) {
				dmx_read(DMX_NUM_1, data, DMX_PACKET_SIZE);

				ESP_LOGI(TAG, "Start code: %02x, Size: %i, Packets/second: %i", packet.sc, packet.size, packet_count); 
				ESP_LOG_BUFFER_HEX(TAG, data, 16);  // Log first 16 bytes last_update = now; packet_count = 0;
			}
				
		} else if (is_connected) {
			// DMX timed out after having been previously connected
			ESP_LOGI(TAG, "DMX was disconnected.");
			break;
		}
	}
}

void wait_for_serial_connection() {
	uint32_t timeout_end = millis() + 2000;
	Serial.begin(115200);
	while(!Serial && timeout_end > millis()) {}  //wait until the connection to the PC is established
}

void fastledTask(void *pvParameters) {
	const TickType_t xDelay = 1000;
	for(;;) {
		ESP_LOGI(TAG,"to red");
		leds[0] = CRGB::Red;
		FastLED.show();
		vTaskDelay(xDelay);
		
		ESP_LOGI(TAG,"to black");
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