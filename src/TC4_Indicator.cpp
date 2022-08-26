/*
 * TC4 Simulator for Artisan Coffee Roaster Application
 *
 * Released under MIT License
 *
 * Created by Sam Chen on 2020
 * Copyright (c) 2020 sam4sharing.com, All rights reserved.
 * 
 * Blog     : https://www.sam4sharing.com
 * YouTube	: https://www.youtube.com/channel/UCN7IzeZdi7kl1Egq_a9Tb4g
 */


#include <Arduino.h>
#include "TC4.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


extern float    BT_AvgTemp;
extern float    ET_CurTemp;
extern uint8_t  pwr_level ;

// Create object for SSD1306


#define INDICATOR_INTERVEL      1000    // Task re-entry intervel (ms)

void TaskIndicator(void *pvParameters)  
{
    /***

    ***/
    
    /* Variable Definition */
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = INDICATOR_INTERVEL / portTICK_PERIOD_MS;
    

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }


  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  // Clear the buffer
  display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2,22);
  display.print("TC4 THREMO");
  display.display();

  vTaskDelay( 3000 / portTICK_RATE_MS ); //dealy 3s showup

  display.clearDisplay();
    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xIntervel);

    display.setTextSize(1);
    display.setCursor(2, 12+2);
    display.print(F("BT:"));
    display.setCursor(20, 12+2);
    display.print(BT_AvgTemp); 
    display.setCursor(20+42, 12+2);
    display.println(F("C")); 
    display.setCursor(2, 24+2);
    display.print(F("ET:")); 
    display.setCursor(20, 24+2);
    display.print(ET_CurTemp); 
    display.setCursor(20+42, 24+2);
    display.println(F("C"));

  } 
}