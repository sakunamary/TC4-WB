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
#include "Fonts/FreeMonoBold9pt7b.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
            

extern float    BT_AvgTemp;
extern float    ET_CurTemp;
extern String   local_IP ;
extern String    BT_EVENT;
extern uint8_t  charging  ; 
uint8_t charg = 5 ;


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


    if (charg <=5) {
            display.clearDisplay();
            display.setTextColor(SSD1306_WHITE);  


            display.setFont(&FreeMonoBold9pt7b);
            display.setTextSize(1);
            display.setCursor(48,8+12);
            display.print(F("LOW"));
            display.setCursor(22,8+12+14);
            display.print(F("BATTERY"));
            display.drawRoundRect( 19,7,90,50,3,WHITE);

            display.drawBitmap(SCREEN_WIDTH-17, SCREEN_HEIGHT-14,  BAT_50, 16, 14, WHITE);

            display.display();
    vTaskDelay( INDICATOR_INTERVEL / portTICK_RATE_MS ); //dealy 1s showup
    }

    else {
    display.setTextColor(SSD1306_WHITE);  
    display.setTextSize(1);

//显示logo


    display.drawBitmap(0, 0, BEAN_LOGO, 16, 16, WHITE);
    display.drawBitmap(0, 16, DRUMMER_LOGO, 16, 16, WHITE);
    display.drawBitmap(0, 32, WIFI_LOGO, 16, 16, WHITE);
    display.drawBitmap(0, 48, BT_LOGO, 16, 16, WHITE);

//显示温度
    display.setCursor(2+16, 0+2);
    display.print(F("BT:"));
    display.setCursor(20+16, 0+2);
    display.print(BT_AvgTemp); 
    display.setCursor(20+42+16, 0+2);
    display.println(F("C")); 

    display.setCursor(2+16, 18+2);
    display.print(F("ET:")); 
    display.setCursor(20+16, 18+2);
    display.print(ET_CurTemp); 
    display.setCursor(20+42+16, 18+2);
    display.println(F("C"));


//显示IP地址和蓝牙状态
    display.setCursor(2+16, 36+2);
    display.print(F("IP:"));
    display.setCursor(20+16,36+2);
    display.println(local_IP); 

    display.setCursor(2+16, 54);
    display.print(BT_EVENT);
 


//显示电池电量情况
     if (charging >= 85){
            display.drawBitmap(SCREEN_WIDTH-17, SCREEN_HEIGHT-14, BAT_100, 16, 14, WHITE);
     }else if (charging >= 55) {
            display.drawBitmap(SCREEN_WIDTH-17, SCREEN_HEIGHT-14,  BAT_75, 16, 14, WHITE);
     }else if (charging >=35) {
            display.drawBitmap(SCREEN_WIDTH-17, SCREEN_HEIGHT-14,  BAT_50, 16, 14, WHITE);
     }else {
            display.drawBitmap(SCREEN_WIDTH-17, SCREEN_HEIGHT-14,  BAT_25, 16, 14, WHITE);
     }  

    display.display();


    vTaskDelay( INDICATOR_INTERVEL / portTICK_RATE_MS ); //dealy 1s showup
    }
  } 

}