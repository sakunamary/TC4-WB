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

/*
 * Include the standard HeliOS header for Arduino sketches. This header
 * includes the required HeliOS header files automatically.
 */




#include "HeliOS.h"
#include "TC4_config.h"

#define TC4_GLOBALS

extern void taskCmdHandle(int);
extern void taskThermo(int);

// Create objects for Beam Temperature (BT) and Environment Temperature (ET)
MAX6675 BT_Thermocouple(CS_BT_PIN);

// Variables for Environment Temperature (ET) if exist
#if ENVIRONMENT_TEMPERATURE_EXIST
MAX6675 ET_Thermocouple(CS_ET_PIN);
#endif

#if SEGMENT_DISPLAY_EXIST
// Create object for a seven segment controller
SevSeg sevseg; 
#endif

// Create Software Serial port for debugging purpose
SoftwareSerial SerialDebug(SOFT_RX, SOFT_TX);       // RX, TX : D7, D8

void setup() {

  // Declare and initialize an int to hold the task id
  int id = 0;


  // Serial ports initialize
  Serial.begin(115200);
  SerialDebug.begin(115200);
  // Try to print something on serial port
//Serial.println("On board UART2USB port is used to communicate with Artisan App");
  SerialDebug.println(" ");
  SerialDebug.println("TC4 Simulator with UNO, MAX6675 v0.02");


  /* Call xSystemInit() to initialize any interrupt handlers and/or
  memory required by HeliOS to execute on the target platform/architecture. */
  xSystemInit();


    /* Create two task to print all task information every

    */
   xTask task = xTaskCreate("TASKCMDHANDLE",taskCmdHandle,NULL);
   xTask task = xTaskCreate("TASKTHERMO",TaskIndicator,NULL);


    /* Pass control to the HeliOS scheduler. */
    xTaskStartScheduler();

}

void loop() {

  /*
   * Momentarily pass control to HeliOS by calling the
   * xHeliOSLoop() function call. xHeliOSLoop() should be
   * the only code inside of the sketch's loop() function.
   */
  xHeliOSLoop();
}

