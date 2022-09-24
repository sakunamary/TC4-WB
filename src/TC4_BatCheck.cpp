
#include <Arduino.h>
//#include <Wire.h>
#include "TC4.h"


#include <Battery18650Stats.h>

#define ADC_PIN 34

Battery18650Stats battery(ADC_PIN);

 uint8_t  charging =15  ; 


void TaskBatCheck(void *pvParameters) 
{ 

    /* Variable Definition */
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = 1000*3 / portTICK_PERIOD_MS; //check every 60s

    /* Task Setup and Initialize */
    charging  = battery.getBatteryChargeLevel();

    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount ();

     
   vTaskDelay( 3000 / portTICK_RATE_MS ); //dealy 3s showup

  for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle (intervel 750ms).
        vTaskDelayUntil(&xLastWakeTime, xIntervel);
      Serial.print(F("Pwr_level %: "));
      charging  = battery.getBatteryChargeLevel();
      Serial.println(charging);

    }

}

