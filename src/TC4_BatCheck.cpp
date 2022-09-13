
#include <Arduino.h>
//#include <Wire.h>
#include "TC4.h"


#include <Battery18650Stats.h>

#define ADC_PIN 34

Battery18650Stats battery(ADC_PIN);


 //uint8_t  pwr_level ;
 uint8_t  charging  ; 


void TaskBatCheck(void *pvParameters) 
{ 

    /* Variable Definition */
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = 1000*60 / portTICK_PERIOD_MS; //check every 60s

    /* Task Setup and Initialize */

     charging  = battery.getBatteryChargeLevel(true);
    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount ();


  for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle (intervel 750ms).
        vTaskDelayUntil(&xLastWakeTime, xIntervel);
      //Serial.print(F("pwr_level : "));
     // pwr_level = battery.getBatteryVolts();
      //Serial.println(pwr_level);
      Serial.print(F("Pwr_level %: "));
      charging  = battery.getBatteryChargeLevel(true);
      Serial.println(charging);

    }

}

