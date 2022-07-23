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



/*D1 mini  PINOUT table 
 5+           USB IN
 GND          USB GND
 TX IO1  TX0  Serial(PAD)
 D4 IO2       serial debug 
 RX IO3  RX0  Serial(PAD)
 D2 IO4  SDA   I2C
 D1 IO5  SCL   I2C
 D7 IO13      
 D6 IO12      RX1   serial_to_drumer
 D5 IO14      TX1 serial_to_drumer        
 D8 IO15      
 D0 IO16      
*/

#ifndef __TC4_CONFIG_H__
#define __TC4_CONFIG_H__

#include <SoftwareSerial.h>
#include "MAX6675.h"

#ifdef  TC4_GLOBALS
#define TC4_EXT
#else
#define TC4_EXT  extern
#endif

#define BAUD 57600 //serial port baudrate 


#define PRINT_TEAMPERATURE_EACH_READING         1	    // Set, to print temperature vaule on serial debug port
#define PRINT_ARTISAN_WHOLE_MESSAGE		        0       // set, to print Artisan commands on serial debug port

#define CS_BT_PIN 	 D8      // 
#define CS_ET_PIN    D7       //
#define SOFT_TX      D5       // 
#define SOFT_RX      D6       // 

/* 
the default sample rate of Artisan is 3 seconds, although the setting value can be modified by user.
I think this value is generated from lots of experimental, so uses 3 seconds as our program algorithm
thus, 1) Set polling intervel 750ms and 2) Array length is 4. that is, the reported temperature is
avaraged from 4 times MAX6675 temperature reading. (750ms x 4 = 3s)
*/
#define TEMPERATURE_ARRAY_LENGTH		        4	    // for averagging temperature purpose
#define MAX6675_READING_INTERVEL 			    750000  // read MAX6675 value every "INTERVEL" ms
TC4_EXT float       BT_TempArray[TEMPERATURE_ARRAY_LENGTH] = {0.0};	    // temperature array
TC4_EXT int			BT_ArrayIndex = 0;                  // A pointer of temperature array
TC4_EXT float		BT_CurTemp = 0.0;
TC4_EXT float		BT_AvgTemp = 0.0;
TC4_EXT bool		isReady = false;                    // flag for temperature array ready for reading
TC4_EXT bool 		unit_C = true;                      // indicate temperature unit from Artisan requested 

#define ABNORMAL_TEMPERATURE_DEGREE             10      // A arrange for abnormal temperature value
TC4_EXT bool		abnormalValue = false;              // indicate temperature value is unexpect or not

// Variables for Environment Temperature (ET) if exist
#if ENVIRONMENT_TEMPERATURE_EXIST
TC4_EXT float		ET_CurTemp = 0.0;
#endif

#endif /*__TC4_CONFIG_H__*/
