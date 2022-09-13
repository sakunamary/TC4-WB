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

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
            

extern float    BT_AvgTemp;
extern float    ET_CurTemp;
//extern uint8_t  pwr_level ;
extern uint8_t  charging  ; 

#define INDICATOR_INTERVEL      750    // Task re-entry intervel (ms)

static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };




void TaskIndicator(void *pvParameters)  
{
    /* Variable Definition */
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = INDICATOR_INTERVEL / portTICK_PERIOD_MS;


  Serial.printf("\n start ssd1306 session !\n");
    
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
    //display.clearDisplay();
   // text display tests
    //display.drawBitmap(10,10,logo_bmp,16,16,SSD1306_WHITE);
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

    display.print(F("BAT:"));
  // if (charging == HIGH) 
  // {
    display.setCursor(20+6, 36+2);
    display.print(charging); 
    display.setCursor(20+42, 36+2);
    display.println(F("%")); 
 //  } else 
 //     {
 //       display.setCursor(20+6, 36+2);
 //       display.print("Chargin..."); 
 //     }

    display.display();
    vTaskDelay( INDICATOR_INTERVEL / portTICK_RATE_MS ); //dealy 1s showup

  } 

}