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
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include "BluetoothSerial.h"

#include "max6675.h"


//#include "WebServer.h"
//Websockets Lib by links2004
#include <WebSocketsServer.h>
//JSON for Artisan Websocket implementation
#include "ArduinoJson.h"


#include <EEPROM.h>



#define VERSION "1.0.4"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Define four tasks 
extern void TaskIndicator( void *pvParameters );
extern void TaskThermalMeter( void *pvParameters );
//extern void TaskwebSocket(void *pvParameters) ;
extern void TaskBatCheck(void *pvParameters ) ;


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
String IpAddressToString(const IPAddress& ipAddress);//转换IP地址格式
void handlePortal();//处理设置网页处理模块


extern float    BT_AvgTemp;
extern float    ET_CurTemp;

float btemp_fix_in;
float etemp_fix_in;

String  BT_EVENT;

String local_IP;

//定义 网页wifi 内容
struct settings {
  char ssid[60]; //增加到30个字符
  char password[60]; //增加到30个字符
  float  btemp_fix;
  float  etemp_fix;
} user_wifi = {"","",0.0,0.0};

//webserver declare 
AsyncWebServer server_OTA(80);

WebSocketsServer webSocket = WebSocketsServer(8080); //构建websockets类
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
            Serial.println("SPP  received data");
            BT_EVENT = "DATA receiving";    
			break; 
         case ESP_SPP_WRITE_EVT:
            Serial.println("SPP  write data");
            BT_EVENT = "DATA writing";    
			break;            
		default:
			Serial.print("Unhandle Event: ");
			Serial.println(event);
			break;
	}
}



String IpAddressToString(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}


const char wifi_sussce_html[] PROGMEM = R"rawliteral(
<!doctype html><html lang='en'><head>
    <meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>
        <title>TC4 THREMO Wifi Setup</title>
        <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> 
        </head> 
<body>
    <main class='form-signin'> 
        <h1>TC4-WB Setup OK</h1> <br/> 
        <p>Your settings have been saved successfully!<br />
        IF settings not working, Please do it again.<br />
        Please RESTART the device.<br />

        </p>
    </main>
</body></html>
)rawliteral";


const char index_html[] PROGMEM = R"rawliteral(
<!doctype html><html lang='en'>
<head>
    <meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>
        <title>TC4-WB Setup</title>
        <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> 
</head> 
<body>
    <main class='form-signin'> 
        <form action='/get' method='get'>
            <h1 class=''>TC4 THRMO SETTING </h1>
            <br/>
            <h2 class=''>WIFI SETUP </h2>
            <br/>
            <div class='form-floating'>
            <label>SSID NAME</label>
            <input type='text' class='form-control' name='ssid'> 
            </div>
            <div class='form-floating'>
            <br/>
            <label>Password</label>
            <input type='password' class='form-control' name='password'>
            </div>
            <br/>
            <h2 class=''>Thermo compensate SETUP </h2>
            <div class='form-floating'>
            <label>Bean Temp</label>
            <input type='text' class='form-control' name='Btemp_fix'> 
            </div>
            <br/>
            <div class='form-floating'>
            <label>Env Temp</label>
            <input type='text' class='form-control' name='Etemp_fix'> 
            </div>
            <br/>
            <br/>
            <button type='submit'>Save</button>
        </form></main> 
</body></html>
)rawliteral";


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Opps....Not found");
}

//Define Artisan Websocket events to exchange data
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  //Artisan schickt Anfrage als TXT
  //TXT zu JSON lt. https://forum.arduino.cc/t/assistance-parsing-and-reading-json-array-payload-websockets-solved/667917

  const size_t capacity = JSON_OBJECT_SIZE(3) + 60; //Memory pool
  DynamicJsonDocument doc(capacity);
  String temp_cmd_out = "" ;//from websockets recived drumer control command and send out ;
    switch(type) {
        case WStype_DISCONNECTED:
            webSocket.sendTXT(num, "Disonnected");
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        
                // send message to client
                webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            { 
            //DEBUG WEBSOCKET
            Serial.printf("[%u] get Text: %s\n", num, payload);

            //Extract Values lt. https://arduinojson.org/v6/example/http-client/
            //Artisan Anleitung: https://artisan-scope.org/devices/websockets/

            deserializeJson(doc, (char *)payload);

            //char* entspricht String
            String command = doc["command"].as<char*>();
            
            //Serial_debug.printf("Command received: %s \n",command);  
            
            long ln_id = doc["id"].as<long>();

            //Send Values to Artisan over Websocket
            JsonObject root = doc.to<JsonObject>();
            JsonObject data = root.createNestedObject("data");
            if(command == "getBT")
            {
              root["id"] = ln_id;
              data["BT"] = BT_AvgTemp;
              //Serial_debug.printf("getBT created BT: %4.2f \n",cmd_M1.TC1);
            }
            else if(command == "getET")
            {
              root["id"] = ln_id;
              data["ET"] = ET_CurTemp;
             // Serial_debug.printf("getET created ET: %4.2f \n",cmd_M1.TC2);
            }
                                  
            else if(command == "getData")
            {
              root["id"] = ln_id;
              data["BT"] = BT_AvgTemp;
              data["ET"] = ET_CurTemp;

              Serial.println("getData");
            }

            char buffer[200]; // create temp buffer 200
            size_t len = serializeJson(doc, buffer);  // serialize to buffer

            webSocket.sendTXT(num, buffer);

            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            }
            break;
        case WStype_BIN:
           // Serial_debug.printf("[%u] get binary length: %u\n", num, length);
            //hexdump(payload, length);

            // send message to client
             webSocket.sendBIN(num, payload, length);
            break;
    }
}



void setup() {
  
    // Initialize serial communication at 115200 bits per second:
    Serial.begin(57600);
    while (!Serial) {
        ; // wait for serial port ready
    }

    Serial.printf("\nTC4-WB v1.0.4 STARTING...\n");
  
    // Initial Bluetooth Serial Port Profile (SPP)
    BTSerial.register_callback(Bluetooth_Callback);
	// Setup bluetooth device name as
	if (!BTSerial.begin("TC4_THERMO")) {
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

//set up eeprom data 
EEPROM.begin(sizeof(struct settings) );
EEPROM.get( 0, user_wifi );
 
 btemp_fix_in = user_wifi.btemp_fix ;
 etemp_fix_in = user_wifi.etemp_fix ;

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
    ,   2048            // Stack size
    ,   NULL
    ,   3               // Priority
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

//初始化网络服务
  WiFi.mode(WIFI_STA);
  WiFi.begin(user_wifi.ssid, user_wifi.password);


  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {

   delay(1000);

    if (tries++> 5) {
    
      //Serial_debug.println("WiFi.mode(AP):");  
      WiFi.mode(WIFI_AP);
      WiFi.softAP("TC4_THRMO", "TC4_THRMO"); // defualt IP address :192.168.4.1 password min 8 digis
      break;
    }
     //show AP's IP
  }

Serial.print("TC4 THREMO 's IP:");

 if  (WiFi.getMode() == 2 ) //1:STA mode 2:AP mode
 {
    Serial.println(IpAddressToString(WiFi.softAPIP())); 
    local_IP = IpAddressToString(WiFi.softAPIP());
 }
 else {
    Serial.println(IpAddressToString(WiFi.localIP())); 
    local_IP = IpAddressToString(WiFi.localIP());
 }



//init websocket 
  webSocket.begin();
  Serial.println("WebSocket started!");

// event  websocket handler
  webSocket.onEvent(webSocketEvent);


// for index.html
  server_OTA.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

//get the value from index.html 
  server_OTA.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
     

    strncpy(user_wifi.ssid,request->getParam("ssid")->value().c_str(), sizeof(user_wifi.ssid) );
    strncpy(user_wifi.password,request->getParam("password")->value().c_str(), sizeof(user_wifi.password) );
    user_wifi.ssid[request->getParam("ssid")->value().length()] = user_wifi.password[request->getParam("password")->value().length()] = '\0';
   
    user_wifi.btemp_fix = request->getParam("Btemp_fix")->value().toFloat();
    user_wifi.etemp_fix = request->getParam("Etemp_fix")->value().toFloat();

//debug output 
 //   Serial.println("");
 //   Serial.printf("Btemp is %f and Etemp is %f ",user_wifi.btemp_fix , user_wifi.etemp_fix );
 //   Serial.println("");
//Svae EEPROM 
    EEPROM.put(0, user_wifi);
    EEPROM.commit();

 btemp_fix_in = user_wifi.btemp_fix ;
 etemp_fix_in = user_wifi.etemp_fix ;

//output wifi_sussce html;
    request->send_P(200, "text/html", wifi_sussce_html);
  });



  server_OTA.onNotFound(notFound); //404 page seems not necessary...

  AsyncElegantOTA.begin(&server_OTA);    // Start ElegantOTA

  server_OTA.begin();

  Serial.println("HTTP server started");

}

void loop()
{

    webSocket.loop();  //处理websocketmie
   
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