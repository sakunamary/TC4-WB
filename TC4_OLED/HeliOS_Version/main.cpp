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





#include <Arduino.h>
#include <HeliOS.h>
#include "TC4_config.h"


// Create objects for Beam Temperature (BT) and Environment Temperature (ET)
#include "max6675.h"
MAX6675 thermocouple_BT(THERMO_SCL, THERMO_CS_BT, THERMO_DO);
MAX6675 thermocouple_ET(THERMO_SCL, THERMO_CS_ET, THERMO_DO);


#include <SoftwareSerial.h>
// Create Software Serial port for debugging purpose
SoftwareSerial Serial_Blueno(SOFT_RX,SOFT_TX);// D0 IO16 RX_Blueno  D5 IO14 TX_Blueno

HardwareSerial &Serial_debug = Serial;


#include <U8g2lib.h>
#include <Wire.h>
// Create object for SH306
U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED



/*Tasks function delear for PlatformIO*/
void TaskIndicator(xTask task_, xTaskParm parm_)  ;
void taskCmdHandle(xTask task_, xTaskParm parm_) ;
void taskThermo(xTask task_, xTaskParm parm_) ;





void setup() {


  // Serial ports initialize
  Serial_debug.begin(BAUD);
  Serial_Blueno.begin(BAUD); 
  // Try to print something on serial port
//Serial.println("On board UART2USB port is used to communicate with Artisan App");
  Serial_debug.println(" ");
  Serial_debug.println("TC4 OLED MAX6675 v1.0");


  /* Call xSystemInit() to initialize any interrupt handlers and/or
  memory required by HeliOS to execute on the target platform/architecture. */
  xSystemInit();


    /* Create two task to print all task information every */
   xTask CmdHandle = xTaskCreate("TASKCMDHANDLE",taskCmdHandle,NULL);
   xTask Thermo = xTaskCreate("TASKTHERMO",taskThermo,NULL);
   xTask Indicator = xTaskCreate("TASKINDICATOR",TaskIndicator,NULL);
  



   if (CmdHandle && Indicator && Thermo ) {
    /* Place the task in the waiting state. */
    xTaskWait(CmdHandle);
    xTaskWait(Indicator);
    xTaskWait(Thermo);



    xTaskChangePeriod(Indicator, 1000); //1000 = 1 second
    xTaskChangePeriod(CmdHandle, 1000); //
    xTaskChangePeriod(Thermo, 1000); //
    /* Pass control to the HeliOS scheduler. The HeliOS scheduler will
    not relinquish control unless xTaskSuspendAll() is called. */
    xTaskStartScheduler();


    /* If the scheduler relinquishes control, do some clean-up by
    deleting the task. */
    xTaskDelete(CmdHandle);
    xTaskDelete(Indicator);
    xTaskDelete(Thermo);
   }
  /* Halt the system. Once called, the system must be reset to
  recover. */
  xSystemHalt();

}

void loop() {
  /* The loop function is not used and should remain empty. */
}


/* TC4_task Indicator */




void TaskIndicator(xTask task_, xTaskParm parm_)  
{
    /***
    Artisan Command Handler
    (2) Displays Beam Temperature (BT) on 4 * 7-segment LEDs if installed
    ***/
    
	// Start SSD1315 OLED Display
	u8g2.begin();
    // Show-up message on display
	u8g2.clearBuffer();                   		    // clear display the internal memory
	u8g2.setFont(u8g2_font_ncenB08_tr);   		    // choose font
    u8g2.drawStr(0, 16, "   Thermal Monitor v2");   // write string to buffer
    u8g2.sendBuffer();                    		    // transfer buffer string to display internal memory to show-up

    for (;;) // A Task shall never return or exit.
    {

	    memset(printBuf, 0, sizeof(printBuf));
   		snprintf(printBuf, sizeof(printBuf)-1, "%3.1f", BT_AvgTemp);

	    u8g2.clearBuffer();                   		    // clear display the internal memory
	    u8g2.setFont(u8g2_font_ncenB08_tr);   		    // choose font
        u8g2.drawStr(0, 16, "   Thermal Monitor v2");   // write string to buffer
	    u8g2.setFont(u8g2_font_ncenB24_tr);   		    // choose font
        if (BT_AvgTemp>=100)
            u8g2.drawStr(24, 56, printBuf);                 // write string to buffer
        else
            u8g2.drawStr(32, 56, printBuf);                 // write string to buffer
        u8g2.sendBuffer();                    		    // transfer buffer string to display internal memory to show-up
    }
}

/* end TC4_task Indicator */

/*task  detail*/

float averageTemperature ( float *);

/*
 * The task definition for taskCmdHander() which handles 
 * Artisan TC4 commands from native UART port and  
 * displays BT temperature on 7-segment LEDs if installed
 */
void taskCmdHandle(xTask task_, xTaskParm parm_) {

    if ( Serial_Blueno.available() ) {
		String msg = Serial_Blueno.readStringUntil('\n');

#if PRINT_ARTISAN_WHOLE_MESSAGE
		Serial_debug.println(msg);                 			// print whole message
#endif

		if (msg.indexOf("READ")==0) {	            		// READ command
/*
	The READ command requests current temperature readings on all active channels. 
	Response from TC4 device is ambient temperature followed by a comma separated list of temperatures in current active units
	The logical channel order is : ambient,chan1,chan2,chan3,chan4
*/	
			Serial.print("0.00,");			      			// ambient temperature
#ifdef ENVIRONMENT_TEMPERATURE_EXIST    
      		if (unit_C)  
				Serial_Blueno.print(ET_CurTemp);	      			// channel 1 : Environment Temperature (ET) with degree Celsius
      		else 
      			Serial_Blueno.print(ET_CurTemp * 9.0/5.0 + 32);	// channel 1 : Environment Temperature (ET) with degree Farenheit
#else
      		if (unit_C)
			 	Serial_Blueno.print(BT_AvgTemp);			        // channel 1 : Environment Temperature (ET); no ET sensor, so uses BT instead	
      		else
        		Serial_Blueno.print(BT_AvgTemp * 9.0/5.0 + 32);    // channel 1 : Environment Temperature (ET); no ET sensor, so uses BT instead	
#endif      				 
    		Serial_Blueno.print(",");		
      		if (unit_C)		
			  	Serial_Blueno.print(BT_AvgTemp);	            	// channel 2 : Bean Temperature (BT) with degree Celsius
      		else 
        		Serial_Blueno.print(BT_AvgTemp * 9.0/5.0 + 32);	// channel 2 : Bean Temperature (BT) with degree Farenheit 
    		Serial_Blueno.println(",0.00,0.00");	          		// channel 3,4 : A vaule of zero indicates the channel is inactive
			
// The READ command be sent from Artisan every 3 seconds (set by sample rate), unmark below code carefully
//			SerialDebug.println("Artisan \"READ\"");						
    	} else if (msg.indexOf("UNITS;")== 0) {	  			// UNIT command 
      		if (msg.substring(6,7)=="F") {   
			  	unit_C = false;
        		Serial_Blueno.println("#OK Farenheit");
				Serial_debug.println("Artisan \"Farenheit\"");
      		}
      		else if (msg.substring(6,7)=="C") {  
        		unit_C = true;
        		Serial_Blueno.println("#OK Celsius");
			  	Serial_debug.println("Artisan \"Celsius\"");
      		}
    	} else if (msg.indexOf("CHAN;")== 0) {    			// CHAN command
      		Serial_Blueno.print("#OK");
	    	Serial_debug.println("Artisan \"CHAN\"");
    	} else if (msg.indexOf("FILT;")== 0) {    			// FILT command
      		Serial_Blueno.print("#OK");
			Serial_debug.println("Artisan \"FILT\"");
		} else {
		  	Serial_debug.println("Artisan Unhandle command");
			Serial_debug.println(msg);
		}
  	}

}

/*
 * The task definition for taskThermo() which will be 
 * executed by HeliOS every MAX6675_READING_INTERVEL (750ms) to read MAX6675 thermocouple
 */
void taskThermo(xTask task_, xTaskParm parm_) {

    // read BT from MAX6675 thermal couple
    BT_CurTemp = thermocouple_BT.readCelsius();

	if ( isReady ) {
    	// means, first round of temperature array is done, can do averaging and filter out operation  
	    BT_AvgTemp = averageTemperature( &BT_TempArray[0] );

        // filter out abnormal temperature-up only, bypass temperature-down
	    // because "CHARGE" period, the temperature-down may large than 10 degree
	    if ( BT_CurTemp < (BT_AvgTemp + ABNORMAL_TEMPERATURE_DEGREE) ) {
	    	// temperature is in-arrange, store it
	    	BT_TempArray[BT_ArrayIndex] = BT_CurTemp;
	     }
	     else {
	     	// set abnormal flag
	    	abnormalValue = true;
	    	// print ? with temperature value in newline
            Serial_debug.println(" ");
            Serial_debug.print(" ?");
            Serial_debug.println(BT_CurTemp);
	   	}
	}
	else {
	   	// just read current temperature
	   	BT_TempArray[BT_ArrayIndex] = BT_CurTemp;
	}

    if ( !abnormalValue ) {
    	// Normal temperature will into this loop
#if PRINT_TEAMPERATURE_EACH_READING
        // print MAX6675 reading value on serial monitor
	   	if ( BT_ArrayIndex == 0 ) {
			Serial_debug.println(" ");
	   		Serial_debug.print("Temperature: ");
	  	}

        Serial_debug.print(" ");			
	   	Serial_debug.print(BT_CurTemp);
#endif
        BT_ArrayIndex++;
        if ( BT_ArrayIndex >= TEMPERATURE_ARRAY_LENGTH ) {
        	BT_ArrayIndex = 0;

            if ( !isReady ) {
                isReady = true;
            }
#if PRINT_TEAMPERATURE_EACH_READING          
            Serial_debug.print(" ");	
            Serial_debug.print("Average: ");
            Serial_debug.print(BT_AvgTemp);
#endif     
#if ENVIRONMENT_TEMPERATURE_EXIST
            // The ET is reference temperature, don't need averaging
            // just read ET from MAX6675 thermal couple every 3 seconds
            ET_CurTemp = ET_Thermocouple.readTempC();
#if PRINT_TEAMPERATURE_EACH_READING           
            Serial_debug.print(" ");	
            Serial_debug.print("ET: ");
            Serial_debug.print(ET_CurTemp);
#endif   
#endif
        }            
    }
    else {
        // After bypass abnormal value, reset flag here
	  	  abnormalValue = false;
    }
}

// A function to average temperature array which values are from MAX6675 temperature reading
float averageTemperature ( float *pTemp ) {

	float avg = *pTemp;

#if 0 
    // Below code works on ESP32 and ESP8266 but get OVF error on UNO,
    // I think this is because UNO is 8-bit microcontroller
    pTemp++;
    for (int i=0; i<=(TEMPERATURE_ARRAY_LENGTH-1); i++) {
		avg = avg + *pTemp;
      	avg =  avg / 2;
      	pTemp++;
}
#else
    pTemp++;
	avg = avg + *pTemp;
    avg =  avg / 2;
    pTemp++;
	avg = avg + *pTemp;
    avg =  avg / 2;
    pTemp++;
	avg = avg + *pTemp;
    avg =  avg / 2;
#endif  

	return avg;
}




/*end of task  detail*/