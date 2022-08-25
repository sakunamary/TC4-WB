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
#include "U8g2lib.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

extern float    BT_AvgTemp;
extern float    ET_CurTemp;
extern uint8_t  pwr_level ;

// Create object for SSD1306
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   
//U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED

#define INDICATOR_INTERVEL      1000    // Task re-entry intervel (ms)

void TaskIndicator(void *pvParameters)  
{
    /***

    ***/
    
    /* Variable Definition */
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = INDICATOR_INTERVEL / portTICK_PERIOD_MS;
    
	// Start SSD1315 OLED Display
	u8g2.begin();

    // Show-up message on display
   u8g2.clearDisplay();
   u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_helvB14_tf); //,u8g2_font_abel_mr//u8g2_font_frigidaire_mr
    u8g2.setCursor(2, 20+2);
    u8g2.print("TC4 THREMO");
  } while ( u8g2.nextPage() );
  vTaskDelay( 3000 / portTICK_RATE_MS ); //dealy 3s showup

    u8g2.clearDisplay();

    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xIntervel);

  u8g2.firstPage();
  do {
    u8g2.setFont( u8g2_font_nokiafc22_tf); //,u8g2_font_abel_mr
    u8g2.setCursor(2, 12+2);
    u8g2.print(F("BT:"));
    u8g2.setCursor(20, 12+2);
    u8g2.print(BT_AvgTemp); 
    u8g2.setCursor(20+42, 12+2);
    u8g2.println(F("C")); 
    u8g2.setCursor(2, 24+2);
    u8g2.print(F("ET:")); 
    u8g2.setCursor(20, 24+2);
    u8g2.print(ET_CurTemp); 
    u8g2.setCursor(20+42, 24+2);
    u8g2.println(F("C"));

  } while ( u8g2.nextPage() );

    }
}