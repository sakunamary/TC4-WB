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
#include "img.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
            

extern float    BT_AvgTemp;
extern float    ET_CurTemp;
extern String   local_IP ;
extern uint8_t  charging  ; 

#define INDICATOR_INTERVEL      750    // Task re-entry intervel (ms)




void TaskIndicator(void *pvParameters)  
{
    /* Variable Definition */
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = INDICATOR_INTERVEL / portTICK_PERIOD_MS;


 if(!display.begin(SSD1306_SWITCHCAPVCC,SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    
  // Show initial display buffer contents on the screen --
   display.clearDisplay();
   display.drawBitmap(17, 19, logo_bmp, 94, 45, WHITE);
   display.display();


   vTaskDelay( 3000 / portTICK_RATE_MS ); //dealy 3s showup

  
    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle
    vTaskDelayUntil(&xLastWakeTime, xIntervel);

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);  
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

    display.setCursor(2, 36+2);
    display.print(F("IP:"));
    display.setCursor(20+6, 36+2);
    display.println(local_IP); 

    display.setCursor(2, 48+2);
    display.print(F("BAT:"));
    display.setCursor(20+6, 48+2);
    display.println(charging); 
    display.setCursor(20+42, 48+2);
    display.println(F("%"));

    display.display();
    vTaskDelay( INDICATOR_INTERVEL / portTICK_RATE_MS ); //dealy 1s showup

  } 

}