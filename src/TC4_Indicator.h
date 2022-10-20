/*
    TC4-WB for Artisan Coffee Roaster Application

    Released under BSD-NC-3.0 License

    Created by Sakunamary on 2022
  
    TC4_Indicator.cpp 
    
    this is a FreeRTOS task for showing infomation on SSD1306 OLED monitor
    the Indicator task will reflash data every 750 ms 
    using Adafruit_GFX and OLED driver

*/

#ifndef __TC4_INDICATOR_H__
#define __TC4_INDICATOR_H__

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
extern String   BT_EVENT;
extern uint8_t  charging  ; 
extern int      b_drop;  
extern bool bAbnormalValue ;


#define TASKINDICATOR_INDICATOR_INTERVEL      750    // Task re-entry intervel (ms)



void TaskIndicator(void *pvParameters)  
{
    /* Variable Definition */
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = user_wifi.sampling_time / portTICK_PERIOD_MS;


 if(!display.begin(SSD1306_SWITCHCAPVCC,SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    
String ver= VERSION ;

  // Show initial display buffer contents on the screen --
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);  
    display.setTextSize(1);
    display.setCursor(86, 0+2);


    display.print(ver);
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


    if (charging<=5) {
            display.clearDisplay();
            display.setTextColor(SSD1306_WHITE);  


            display.setFont(&FreeMonoBold9pt7b);
            display.setTextSize(1);
            //display.drawLine(64,0,64,64,WHITE);
           // display.drawLine(0,32,128,32,WHITE);
            display.setCursor(48,24+4);
            display.print(F("LOW"));
            display.setCursor(26,40+4);
            display.print(F("BATTERY"));
            display.drawRoundRect( 19,7,90,45,3,WHITE);

            display.display();
            display.setFont();
    vTaskDelay(  user_wifi.sampling_time / portTICK_RATE_MS ); //dealy 1s showup
    }

    else {
    display.setTextColor(SSD1306_WHITE);  
    display.setTextSize(1);

//显示logo


    display.drawBitmap(0, 0, BEAN_LOGO, 16, 16, WHITE);
    display.drawBitmap(0, 16, DRUMMER_LOGO, 16, 16, WHITE);


//显示温度

if (bAbnormalValue == true )  {
    display.invertDisplay(true);
} else     display.invertDisplay(false);


    display.setCursor(2+16, 0+2);
    display.print(F("BT:"));


    display.setCursor(20+16, 0+2);
    display.print(BT_AvgTemp); 
    display.setCursor(62+16, 0+2);
    display.println(F("C")); 

    display.setCursor(2+16, 18+2);
    display.print(F("ET:")); 
    display.setCursor(20+16, 18+2);
    display.print(ET_CurTemp); 
    display.setCursor(62+16, 18+2);
    display.println(F("C"));

#if defined(FULL_VERSION) //full version 
//显示IP地址和蓝牙状态
    display.drawBitmap(0, 32, WIFI_LOGO, 16, 16, WHITE);
    display.drawBitmap(0, 48, BT_LOGO, 16, 16, WHITE);
    display.setCursor(2+16, 36+2);
    display.print(F("IP:"));
    display.setCursor(20+16,36+2);
    display.println(local_IP); 
    display.setCursor(2+16, 54);
    display.print(BT_EVENT);
 #endif 

#if defined(WIFI_VERSION) //wifi version 
//显示IP地址和蓝牙状态
    display.drawBitmap(0, 32, WIFI_LOGO, 16, 16, WHITE);
    display.setCursor(2+16, 36+2);
    display.print(F("IP:"));
    display.setCursor(20+16,36+2);
    display.println(local_IP); 

 #endif 


#if defined(BLUETOOTH_VERSION) //wifi version 
//显示IP地址和蓝牙状态
    display.drawBitmap(0, 32, BT_LOGO, 16, 16, WHITE);
    display.setCursor(2+16, 36+2);
    display.print(BT_EVENT);

 #endif 


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


    vTaskDelay(  user_wifi.sampling_time / portTICK_RATE_MS ); //dealy 1s showup
    }
  } 

}


#endif