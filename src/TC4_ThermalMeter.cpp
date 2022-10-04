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
#include "max6675.h"

#define THERMAL_READING_INTERVEL 			    750     // read MAX6675 value every 750 ms
#define TEMPERATURE_ARRAY_LENGTH		        4	    // for averagging temperature purpose
#define ABNORMAL_TEMPERATURE_DEGREE             5     // defin abnormal temperature value

float       BT_TempArray[TEMPERATURE_ARRAY_LENGTH] = {0.0};	    // temperature array
int			BT_ArrayIndex = 0;                          // A pointer of temperature array
float		BT_CurTemp = 0.0;
float		BT_AvgTemp = 0.0;
float		ET_CurTemp = 0.0;
bool		bReady = false;                             // flag to indicate temperature array whether is ready or not
bool 		bUnit_C = true;                             // flag to indicate temperature unit from Artisan requested 
bool		bAbnormalValue = false;                     // indicate temperature value is unexpect or not
int         b_drop = 0;
int thermoDO = 19;
int thermoCLK = 5;
int thermoCS_ET =16;
int thermoCS_BT = 17;


extern float etemp_fix_in ;
extern float btemp_fix_in ;

// Create Variables and Object for Environment Temperature (ET) if exist

MAX6675 thermocouple_ET(thermoCLK, thermoCS_ET, thermoDO);

// Create object for Beam Temperature (BT)
MAX6675 thermocouple_BT(thermoCLK, thermoCS_BT, thermoDO);

float averageTemperature ( float *pTemp );


void TaskThermalMeter(void *pvParameters) 
{ 
    /***
    ThermalMeter Task
    The default sample rate of Artisan is 3 seconds, although the setting value can be modified by user.
    I think this value is generated from lots of experimental, so keeps polling-time as 3 seconds.
    Thus, In the ThermalMeter Task will (750ms x 4 = 3s)
    (1) read MAX6675 temperature evey 750ms, and
    (2) Average-Array length is 4.
    That is, the measured temperature is avaraged from 4 times of MAX6675 temperature reading. 
    ***/

    /* Variable Definition */
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = THERMAL_READING_INTERVEL / portTICK_PERIOD_MS;

    /* Task Setup and Initialize */
    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount ();

    for (;;) // A Task shall never return or exit.
    {

        // Wait for the next cycle (intervel 750ms).
        vTaskDelayUntil(&xLastWakeTime, xIntervel);

        // Perform task actions from here
        // Read BT from MAX6675 thermal couple
        BT_CurTemp = thermocouple_BT.readCelsius() + btemp_fix_in;

	    if ( bReady )
        {
            // Means, first round of temperature array is done,
            // The averaged temperyure is ready for reading  
	        BT_AvgTemp = averageTemperature( &BT_TempArray[0] );

            // Filter out abnormal temperature-up only (Bypass temperature-down)
	        // Because in "CHARGE" period, the temperature-down may large than 10 degree
	        if (BT_CurTemp < (BT_AvgTemp + ABNORMAL_TEMPERATURE_DEGREE))
            {
	            // temperature is in-arrange, store it
	    	    BT_TempArray[BT_ArrayIndex] = BT_CurTemp;
                b_drop =0 ;
	        }
	        else
            {
	            // set abnormal flag
	    	    bAbnormalValue = true;
                b_drop++;
	    	    // print ? with temperature value in newline
                Serial.println(" ");
                Serial.print(" ?");
                Serial.println(BT_CurTemp);
	        }
	    }
	    else
        {
	   	    // just read current temperature
	   	    BT_TempArray[BT_ArrayIndex] = BT_CurTemp;
	    }

        if ( !bAbnormalValue )
        {
            // Normal temperature will into this loop
#if PRINT_TEAMPERATURE_EACH_READING
            // print MAX6675 reading value on serial monitor
	   	    if ( BT_ArrayIndex == 0 ) 
            {
			    Serial.println(" ");
	   		    Serial.print("Temperature: ");
	   	    }

            Serial.print(" ");			
	   	    Serial.print(BT_CurTemp);
#endif
            BT_ArrayIndex++;
            if ( BT_ArrayIndex >= TEMPERATURE_ARRAY_LENGTH ) 
            {
                BT_ArrayIndex = 0;

                if ( !bReady )
                {
                    bReady = true;
                }
#if PRINT_TEAMPERATURE_EACH_READING          
                Serial.println(" ");	
                Serial.print("Average: ");
                Serial.print(BT_AvgTemp);
                Serial.print("BT compensate:");
                Serial.print(btemp_fix_in);     
                Serial.println(" ");	        
#endif     

                // The ET is reference temperature, don't need averaging
                // just read ET from MAX6675 thermal couple every 3 seconds
               ET_CurTemp = thermocouple_ET.readCelsius() + etemp_fix_in;
       
                //Serial.println(" ");	
                //Serial.printf("ET:%f ,ET compensate:%f",ET_CurTemp,etemp_fix_in);
                //Serial.println("");

            } 
        }
        else
        {
            // After bypass abnormal value, reset flag here
	  	    bAbnormalValue = false;
        }
    }
}

// A function to average temperature array 
float averageTemperature ( float *pTemp )
{
	float avg = *pTemp;

    pTemp++;
	avg = (avg + *pTemp)/2;
    pTemp++;
	avg = (avg + *pTemp)/2;
    pTemp++;
	avg = (avg + *pTemp)/2;
	return avg;
}
