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
#include <WiFi.h>
#include "BluetoothSerial.h"
#include "max6675.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Define two tasks 
extern void TaskIndicator( void *pvParameters );
extern void TaskThermalMeter( void *pvParameters );
extern void TaskwebSocket(void *pvParameters) ;
extern void TaskBatCheck(void *pvParameters ) ;

extern float    BT_AvgTemp;
extern float    ET_CurTemp;
String  BT_EVENT;



BluetoothSerial BTSerial;

void Bluetooth_Callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {

	switch (event) {
        case ESP_SPP_INIT_EVT:
			Serial.println("SPP is inited");
            BT_EVENT = "SPP INITED";
			break;
        case ESP_SPP_START_EVT:
            Serial.println("SPP server started");
            BT_EVENT = "SPP STARTED";
			break;
		case ESP_SPP_SRV_OPEN_EVT:
			Serial.println("Client Connected");
            BT_EVENT = "Client OK";           
			break;
		case ESP_SPP_CLOSE_EVT:
			Serial.println("Client disconnected");
            BT_EVENT = "Client lost";    
			break;
        case ESP_SPP_DATA_IND_EVT:
            Serial.println("SPP connection received data");
            BT_EVENT = "DATA receiving";    
			break;       
		default:
			Serial.print("Unhandle Event: ");
			Serial.println(event);
			break;
	}
}

void setup() {
  

    // Initialize serial communication at 115200 bits per second:
    Serial.begin(57600);
    while (!Serial) {
        ; // wait for serial port ready
    }

    Serial.printf("\nTC4 Thermo v1.0 - BT and WebSocket !\n");
  
    // Initial Bluetooth Serial Port Profile (SPP)
    BTSerial.register_callback(Bluetooth_Callback);
	// Setup bluetooth device name as
	if (!BTSerial.begin("TC4_Thermo")) {
		Serial.println("An error occurred during initialize");
	}
	else {
	    Serial.println("Bluetooth is ready for pairing");
        // Use FIXED pin-code for Legacy Pairing
		char pinCode[5];
		memset(pinCode, 0, sizeof(pinCode));
		pinCode[0] = '1';
  		pinCode[1] = '2';
  		pinCode[2] = '3';
 		pinCode[3] = '4';
		BTSerial.setPin(pinCode); 
	}

    /*---------- Task Definition ---------------------*/
    // Setup tasks to run independently.
    xTaskCreatePinnedToCore (
        TaskIndicator
    ,   "IndicatorTask" // 128*64 SSD1306 OLED 显示参数
    ,   2048            // This stack size can be checked & adjusted by reading the Stack Highwater
    ,   NULL
    ,   2               // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,   NULL 
    ,   tskNO_AFFINITY  // Running Core decided by FreeRTOS
    );

  xTaskCreatePinnedToCore (
        TaskThermalMeter
    ,   "ThermalMeter"  // MAX6675 thermal task to read Bean-Temperature (BT)
    ,   1024            // Stack size
    ,   NULL
    ,   3               // Priority
    ,   NULL 
    ,   tskNO_AFFINITY  // Running Core decided by FreeRTOS
    );
    
 xTaskCreatePinnedToCore (
        TaskwebSocket
    ,   "webSocketTask" // 设置wifi参数的网页处理和处理artisan websocket的处理函数
    ,   4096            // This stack size can be checked & adjusted by reading the Stack Highwater
    ,   NULL
    ,   3               // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,   NULL 
    ,   tskNO_AFFINITY  // Running Core decided by FreeRTOS
    );

 xTaskCreatePinnedToCore (
        TaskBatCheck
    ,   "bat_check"     // 测量电池电源数据，每分钟测量一次
    ,   1024            // This stack size can be checked & adjusted by reading the Stack Highwater
    ,   NULL
    ,   1               // Priority, with 1 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,   NULL 
    ,   tskNO_AFFINITY  // Running Core decided by FreeRTOS
    );

}

void loop()
{
    // This is main task which is created by Arduino to handle Artisan TC4 Commands

    if (BTSerial.available())
    {
        String msg = BTSerial.readStringUntil('\n');
/*
	The READ command requests current temperature readings on all active channels. 
	Response from TC4 is ambient temperature followed by a comma separated list of temperatures in current active channels.
	The logical channel order is : ambient,chan1,chan2,chan3,chan4
*/	

 /* READ command */ 
		if (msg.indexOf("READ")==0) {	                // READ command
			BTSerial.print(ET_CurTemp);			         // channel 1 : Environment Temperature (ET);                                 					 
    	    BTSerial.print(",");		
			BTSerial.print(BT_AvgTemp);	                // channel 2 : Bean Temperature (BT) with degree Celsius
    	    BTSerial.println(",0.00,0.00");	            // channel 3,4 : A vaule of zero indicates the channel is inactive
        }
 /* UNIT command ONLY PROVIED C NOT F */ 
 /*
        else if (msg.indexOf("UNITS;")== 0) {	 
            if (msg.substring(6,7)=="F") {   
			   
                BTSerial.println("#OK Farenheit");
				Serial.println("Artisan \"Farenheit\"");
            }
            else if (msg.substring(6,7)=="C") {  
                BTSerial.println("#OK Celsius");
			    Serial.println("Artisan \"Celsius\"");
            }
        }
*/
/* CHAN command */        
        else if (msg.indexOf("CHAN;")== 0) {    
            BTSerial.print("#OK");
	        Serial.println("Artisan \"CHAN\"");
        }
/* FILT command */        
        else if (msg.indexOf("FILT;")== 0) {   
            BTSerial.print("#OK");
			Serial.println("Artisan \"FILT\"");
		}
/* Unhandle command */           
        else {
		    Serial.println("Artisan Unhandle command");
			Serial.println(msg);
		}
   }
}