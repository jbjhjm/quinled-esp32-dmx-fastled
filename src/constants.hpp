#define ESP32

#define TX_PIN 15  // The DMX transmit pin.
#define RX_PIN 16  // The DMX receive pin.
#define EN_PIN -1  // The DMX transmit enable pin. Set to -1 if the used RS48 board manages mode switching internally. (RTS, DE/RE pins)


// NOTE: different DMX senders may be incompatible.
// a setup with QLCplus + a cheap generic usb-dmx interface did not work.
// I then tested with a GrandMA 2port node and everything is running perfectly fine.
// It is recommended to first validate the DMX output is working as intended using a simple DMX controlled light.

#define LED_PIN     0
#define NUM_LEDS    2
#define BRIGHTNESS  150
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB

#define DMX_REFRESH_MS 20
#define DMX_START_OFFSET 2
#define DMX_NUM_CHANNELS 32