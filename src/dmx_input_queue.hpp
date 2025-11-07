#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "constants.hpp"

static QueueHandle_t dmxQueue;

bool createDmxInputQueue();