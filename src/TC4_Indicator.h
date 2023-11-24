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
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include "img.h"
//#include "Fonts/FreeMonoBold9pt7b.h"
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SSD1306Wire display(SCREEN_ADDRESS, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h


extern String local_IP;
extern String BT_EVENT;
extern uint8_t charging;
extern float volts;
extern int b_drop;
extern bool bAbnormalValue;

static char buffer[64];
int init_countdown =30 ;

//#define TASKINDICATOR_INDICATOR_INTERVEL 750 // Task re-entry intervel (ms)

void TaskIndicator(void *pvParameters)
{
    /* Variable Definition */
    (void)pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = 1000 / portTICK_PERIOD_MS;

    String ver = VERSION;

   display.init();
   display.flipScreenVertically();
   display.setContrast(255);

    // Show initial display buffer contents on the screen --
  
   display.clear();
   display.setFont(ArialMT_Plain_10);
   display.drawString(86, 0 + 2,ver);
   display.drawXbm(17, 19,  94, 43,logo_bmp);
   display.display();


    vTaskDelay(3000 / portTICK_RATE_MS); // dealy 3s showup

    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xIntervel);
        //countdown init screen 如果倒数没到1 and 电量 大于5 

        if ( init_countdown >=1 ) 
        {//倒计时界面
         if (charging <= 5) //处理电量过低警示
            {
            display.clear();
            display.setFont(ArialMT_Plain_16);
            display.drawString(48, 14-4 + 4,"LOW");
            display.drawString(28, 30-4 + 4,"BATTERY");
            display.drawRect(2, 2, 128-2, 64-2);
            display.display();
            display.setFont(ArialMT_Plain_10);   
           // vTaskDelay(user_wifi.sampling_time / portTICK_RATE_MS); // dealy 1s showup

            } else {

            display.clear();
            display.setFont(ArialMT_Plain_10);
            display.drawString(16, 14,"Warm up Thermos");
            display.drawString(28, 36,"Wait            s");
            display.setFont(ArialMT_Plain_16);
            display.drawStringf(58, 32,buffer,"%2d",init_countdown);
            display.display();
       
            //vTaskDelay(1000 / portTICK_RATE_MS); // dealy 1s showup
            init_countdown--;
            }
        } 

        else { //正常显示界面
                display.setFont(ArialMT_Plain_10);
                display.clear();

                if (charging <= 5) //处理电量过低警示
                {
                
                    display.clear();
                    display.setFont(ArialMT_Plain_16);
                    display.drawString(48, 14-4 + 4,"LOW");
                    display.drawString(28, 30-4 + 4,"BATTERY");
                    display.drawRect(2, 2, 128-2, 64-2);
                    display.display();
                    display.setFont(ArialMT_Plain_10);   
                    //vTaskDelay(user_wifi.sampling_time / portTICK_RATE_MS); // dealy 1s showup
                }

                else
                {
                
                    //显示logo

                    display.drawXbm(0, 0, 16, 16, BEAN_LOGO);
                    display.drawXbm(0, 16, 16, 16, DRUMMER_LOGO);

                    //显示温度

                    if (bAbnormalValue == true)
                    {
                    display.invertDisplay();
                    }
                    else
                        display.normalDisplay();

                    if (xSemaphoreTake(xThermoDataMutex, xIntervel) == pdPASS) // Mutex to make the data more clean
                    {
                        display.drawStringf(2 + 16, 0 + 2,buffer,"BT:%4.2f",temperature_data.BT_AvgTemp);
                        display.drawStringf(2 + 16, 18 + 2,buffer,"ET:%4.2f",temperature_data.ET_AvgTemp);
                        display.drawStringf(128-48-12, 0 + 2,buffer,"dB:%4.2f",temperature_data.BT_ROR);
                        display.drawStringf(128-48-12, 18 + 2,buffer,"dE:%4.2f",temperature_data.ET_ROR);
                        xSemaphoreGive(xThermoDataMutex);
                        }      


        #if defined(FULL_VERSION) // full version
                    //显示IP地址和蓝牙状态
                    display.drawXbm(0, 32, 16, 16, WIFI_LOGO);
                    display.drawXbm(0, 48, 16, 16, BT_LOGO);
                    display.drawString(2 + 16, 38,"IP:");
                    display.drawString(2 + 30, 38,local_IP);
                    //display.drawStringf(2 + 16, 36 + 2,buffer,"IP:%s",local_IP);
                    display.drawStringf(2 + 16, 54,buffer,"%s",BT_EVENT);
        #endif

        #if defined(WIFI_VERSION) // wifi version
                    //显示IP地址和蓝牙状态
                    display.drawXbm(0, 32, 16, 16, WIFI_LOGO);
                    display.drawString(2 + 16, 38,"IP:");
                    display.drawString(2 + 30, 38,local_IP);
                    //display.drawStringf(2 + 16, 36 + 2,buffer,"IP:%s",local_IP);

        #endif

        #if defined(BLUETOOTH_VERSION) // wifi version
                    //显示IP地址和蓝牙状态
                    display.drawXbm(0, 32, 16, 16, BT_LOGO);
                    display.drawStringf(2 + 16, 36 + 2,buffer,"%s",BT_EVENT);

        #endif

                    //显示电池电量情况
                    if (charging >= 75)
                    {
                        display.drawXbm(SCREEN_WIDTH - 17, SCREEN_HEIGHT - 14, 16, 16,BAT_100);
                    }
                    else if (charging >= 55)
                    {
                        display.drawXbm(SCREEN_WIDTH - 17, SCREEN_HEIGHT - 14, 16, 16, BAT_75);
                    }
                    else if (charging >= 35)
                    {
                        display.drawXbm(SCREEN_WIDTH - 17, SCREEN_HEIGHT - 14, 16, 16, BAT_50);
                    }
                    else if (charging >= 15)
                    {
                        display.drawXbm(SCREEN_WIDTH - 17, SCREEN_HEIGHT - 14, 16, 16, BAT_25);
                    }
                    else
                    {
                        display.drawXbm(SCREEN_WIDTH - 17, SCREEN_HEIGHT - 14, 16, 16, BAT_0);
                    }
                    
                    display.display();
                    //vTaskDelay(user_wifi.sampling_time / portTICK_RATE_MS); // dealy 1s showup
                }
                }
                
    }
}

#endif