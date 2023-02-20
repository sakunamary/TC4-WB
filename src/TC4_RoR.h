#ifndef __TC4_ROR_H__
#define __TC4_ROR_H__


#include "TC4.h"

extern float BT_AvgTemp;
extern float ET_CurTemp;
float ROR_TempArray[TEMPERATURE_ARRAY_LENGTH] = {0.0}; // temperature array

float Temp_last ;
float BT_ROR = 0.0;
float ET_ROR = 0.0;

float ROR_Temp(float Temp_in,float count_time);

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
        BT_ROR = ROR_Temp(BT_AvgTemp  ,4);
        ET_ROR = ROR_Temp(ET_CurTemp ,4);
        xSemaphoreGive(xThermoDataMutex);  //end of lock mutex
       }

    }
}

float ROR_Temp(float Temp_in , float count_time)
{

  float ROR_out;

  if (Temp_in == 0) 
  {
    Temp_last= Temp_in;
  }
   else {
   ROR_out = (Temp_in-Temp_last ) * (60/count_time);
   Temp_last= Temp_in;
   }

   return ROR_out;

}


#endif