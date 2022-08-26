





#include <Arduino.h>
#include <Wire.h>
#include "TC4.h"

#include "IP5306.h"

#define I2C_SDA   21
#define I2C_SCL   22
#define PWR_IQR   4

IP5306 ip5306;
uint8_t pwr_level ;
//uint8_t charging   ;

void TaskIP5306(void *pvParameters) 
{ 

    /* Variable Definition */
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = 10000/ portTICK_PERIOD_MS; //check every 60s

    /* Task Setup and Initialize */
    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount ();


ip5306.begin(I2C_SDA, I2C_SCL);
ip5306.setup();
//pinMode(PWR_IQR,INPUT) ;


  for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle (intervel 750ms).
        vTaskDelayUntil(&xLastWakeTime, xIntervel);
        pwr_level = ip5306.getBatteryLevel();
        //charging = digitalRead(PWR_IQR) ;
    }

}

