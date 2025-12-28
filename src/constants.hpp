#define ESP32

#define TX_PIN 15  // The DMX transmit pin.
#define RX_PIN 16  // The DMX receive pin.
#define EN_PIN -1  // The DMX transmit enable pin. Set to -1 if the used RS48 board manages mode switching internally. (RTS, DE/RE pins)


// NOTE: different DMX senders may be incompatible.
// a setup with QLCplus + a cheap generic usb-dmx interface did not work.
// I then tested with a GrandMA 2port node and everything is running perfectly fine.
// It is recommended to first validate the DMX output is working as intended using a simple DMX controlled light.

#define LED_PIN1     0
#define LED_PIN2     1
#define LED_PIN3     2
#define LED_PIN4     3
#define LED_PIN5     4
#define LED_PIN6     5
#define LED_PIN7     12
#define LED_PIN8     13

#define NUM_LED_BLOCKS_PER_STRIP 1
#define NUM_LED_BLOCK_SIZE 2
#define NUM_STRIPS 5
#define NUM_LED_BLOCKS_TOTAL NUM_STRIPS * NUM_LED_BLOCKS_PER_STRIP
#define NUM_LEDS_PER_STRIP NUM_LED_BLOCKS_PER_STRIP * NUM_LED_BLOCK_SIZE
#define NUM_LED_OUTPUTS_TOTAL NUM_LEDS_PER_STRIP * NUM_STRIPS

#define BRIGHTNESS  150
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB

#define SPI_REFRESH_MS 15

#define DMX_REFRESH_MS 15
#define DMX_START_OFFSET 1 // channel 0 will always be zero, index 1-512 will be set
#define DMX_NUM_CHANNELS 32