#ifndef __TC4_ROR_H__
#define __TC4_ROR_H__


#include "TC4.h"
extern float BT_AvgTemp;
extern float ET_CurTemp;


float Temp_last ;
float BT_ROR = 0.0;
float ET_ROR = 0.0;



void TaskROR(void *pvParameters)
{

     /* Variable Definition */
    (void)pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = (user_wifi.sampling_time * 1000) / portTICK_PERIOD_MS;

        // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

        for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xIntervel);
       if (xSemaphoreTake(xThermoDataMutex, xIntervel) == pdPASS) 
       {
        
       }

    }
}



#endif