#ifndef __TC4_THERMALMETER_H__
#define __TC4_THERMALMETER_H__

#include <Arduino.h>
#include "TC4.h"


void TaskRoR(void *pvParameters)

{

    /* Variable Definition */
    (void)pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = 1000 * 5 / portTICK_PERIOD_MS; // check every 60s


    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    for(;;)
    {


    }
}
#endif
