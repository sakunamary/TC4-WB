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



/*
LGT8f
*/

#ifndef __TC4_CONFIG_H__
#define __TC4_CONFIG_H__


#define BAUD 57600 //serial port baudrate 


#define PRINT_TEAMPERATURE_EACH_READING         1	    // Set, to print temperature vaule on serial debug port
#define PRINT_ARTISAN_WHOLE_MESSAGE		        0       // set, to print Artisan commands on serial debug port

#define THERMO_DO         4
#define THERMO_SCL       6
#define THERMO_CS_BT 	   5    // 
#define THERMO_CS_ET      7     //
#define SOFT_TX        2     // 
#define SOFT_RX        3    // 

/* 
the default sample rate of Artisan is 3 seconds, although the setting value can be modified by user.
I think this value is generated from lots of experimental, so uses 3 seconds as our program algorithm
thus, 1) Set polling intervel 750ms and 2) Array length is 4. that is, the reported temperature is
avaraged from 4 times MAX6675 temperature reading. (750ms x 4 = 3s)
*/
#define TEMPERATURE_ARRAY_LENGTH		        4	    // for averagging temperature purpose
#define MAX6675_READING_INTERVEL 			    750000  // read MAX6675 value every "INTERVEL" ms
 float       BT_TempArray[TEMPERATURE_ARRAY_LENGTH] = {0.0};	    // temperature array
 int			BT_ArrayIndex = 0;                  // A pointer of temperature array
 float		BT_CurTemp = 0.0;
 float		BT_AvgTemp = 0.0;
 float		ET_CurTemp = 0.0;
 float		ET_AvgTemp = 0.0;

 bool		isReady = false;                    // flag for temperature array ready for reading
 bool 		unit_C = true;                      // indicate temperature unit from Artisan requested 

 char 			printBuf[64];			// a string buffer for OLED 

#define ABNORMAL_TEMPERATURE_DEGREE             10      // A arrange for abnormal temperature value
 bool		abnormalValue = false;              // indicate temperature value is unexpect or not


#endif /*__TC4_CONFIG_H__*/
