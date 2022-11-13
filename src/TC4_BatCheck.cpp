/*
    TC4-WB for Artisan Coffee Roaster Application

    Released under BSD-NC-3.0 License

    Created by Sakunamary on 2022

    TC4_batCheck.cpp

    This a FreeRTOS task to check the battery onboard every 60s
    this task is lowest priority ,so sometime  battery value on OLED
    is not currect ,but will be currect in next loop .

*/

#include <Arduino.h>
#include "TC4.h"

#include <Battery18650Stats.h>

#define ADC_PIN 34

Battery18650Stats battery(ADC_PIN);

uint8_t charging;
float volts;

void TaskBatCheck(void *pvParameters)
{

    /* Variable Definition */
    (void)pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = 1000 * 5 / portTICK_PERIOD_MS; // check every 60s

    /* Task Setup and Initialize */
    charging = battery.getBatteryChargeLevel();
    volts = battery.getBatteryVolts();

    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    vTaskDelay(3000 / portTICK_RATE_MS); // dealy 60s showup

    for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle (intervel 750ms).
        vTaskDelayUntil(&xLastWakeTime, xIntervel);
        Serial.print(F("Pwr_level %: "));
        charging = battery.getBatteryChargeLevel();
        volts = battery.getBatteryVolts();
        
        Serial.println(charging);
    }
}
