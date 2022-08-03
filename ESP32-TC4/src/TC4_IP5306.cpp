





#include <Arduino.h>
#include <Wire.h>
#include "TC4.h"

#include "IP5306.h"

#define I2C_SDA   21
#define I2C_SCL   22

IP5306 ip5306;
uint8_t pwr_level ;

void TaskIP5306(void *pvParameters) 
{ 

    /* Variable Definition */
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = 60000*5 / portTICK_PERIOD_MS; //check every 60s

    /* Task Setup and Initialize */
    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount ();


ip5306.begin(I2C_SDA, I2C_SCL);
ip5306.setup();



  for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle (intervel 750ms).
        vTaskDelayUntil(&xLastWakeTime, xIntervel);
        pwr_level = ip5306.getBatteryLevel();
        
    }

}

