#include "common.h"
#include "FreeRTOS.h"
#include "task.h"

millis_t milliseconds;

void millis_init(void)
{
    milliseconds = 0;
}

millis_t millis_get(void)
{
    return (millis_t)xTaskGetTickCount();
}