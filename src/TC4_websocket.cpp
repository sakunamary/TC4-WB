
#include <Arduino.h>
#include "TC4.h"
#include "WiFi.h"
#include "WebServer.h"

//Websockets Lib by links2004
#include <WebSocketsServer.h>
//JSON for Artisan Websocket implementation
#include "ArduinoJson.h"

#include <EEPROM.h>

extern float    BT_AvgTemp;
extern float    ET_CurTemp;

String local_IP;

//定义 网页wifi 内容
struct settings {
  char ssid[60]; //增加到30个字符
  char password[60]; //增加到30个字符
} user_wifi = {};

//WiFiServer    wifiserver(8090); //构建webserver类
WebServer     server(80);
WebSocketsServer webSocket = WebSocketsServer(8080); //构建websockets类


#define INDICATOR_INTERVEL      1000   // Task re-entry intervel (ms)

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
String IpAddressToString(const IPAddress& ipAddress);//转换IP地址格式
void handlePortal();//处理设置网页处理模块



void TaskwebSocket(void *pvParameters)  
{

 /***
    TaskwebSocket
     1)建立wifi端，
     2）建立web页面服务器，可以输入wifi的账号密码
     3）建立websocket服务器
    ***/

    /* Variable Definition */
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = INDICATOR_INTERVEL / portTICK_PERIOD_MS;


//set up eeprom data 
EEPROM.begin(sizeof(struct settings) );
EEPROM.get( 0, user_wifi );

//初始化网络服务
  WiFi.mode(WIFI_STA);
  WiFi.begin(user_wifi.ssid, user_wifi.password);


  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);
    if (tries++> 15) {
    
      //Serial_debug.println("WiFi.mode(AP):");  
      WiFi.mode(WIFI_AP);
      WiFi.softAP("TC4_THRMO", "TC4_THRMO"); // defualt IP address :192.168.4.1 password min 8 digis
      break;
    }
     //show AP's IP

  }

  server.on("/",  handlePortal);
  server.begin();

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
    // event handler
  webSocket.onEvent(webSocketEvent);
//websocket loop 

    xLastWakeTime = xTaskGetTickCount();
for(;;)
{   
    vTaskDelayUntil(&xLastWakeTime, xIntervel);
    webSocket.loop();  //处理websocketmie
    server.handleClient();//处理网页
}

}

String IpAddressToString(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}


void handlePortal() {

  if (server.method() == HTTP_POST) {

    strncpy(user_wifi.ssid,     server.arg("ssid").c_str(),     sizeof(user_wifi.ssid) );
    strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password) );
    user_wifi.ssid[server.arg("ssid").length()] = user_wifi.password[server.arg("password").length()] = '\0';

    EEPROM.put(0, user_wifi);
    EEPROM.commit();

    server.send(200,   "text/html",  "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>TC4 THREMO Wifi Setup</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>TC4 THRMO Wifi Setup</h1> <br/> <p>Your settings have been saved successfully!<br />Please restart the device.</p></main></body></html>" );
  } else {

    server.send(200,   "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>TC4 THREMO Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/' method='post'><h1 class=''>TC4 THRMO Wifi Setup</h1><br/><div class='form-floating'><label>SSID NAME</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button></form></main> </body></html>" );
  }
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
            //Serial_debug.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                //Serial_debug.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        
                // send message to client
                webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            { 
            //DEBUG WEBSOCKET
            //Serial.printf("[%u] get Text: %s\n", num, payload);

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