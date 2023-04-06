/*
    TC4-WB for Artisan Coffee Roaster Application

    Released under BSD-NC-3.0 License

    Created by Sakunamary on 2022

    TC4-WB main.cpp

    TC4-WB is a esp32 base thermo module for Artisan .
    1)it can get two channels  temperture data by MX6675 .
    For BT channel, data updates every 750ms and ET channel data updates every 3s
    2)The temperture datas can transmit with Artisan by wifi-websocket and/or bluetooth-TC4,
      base on the version you chioced .
       fullversion : Both wifi-websocket and bluetooth ,so you can monitor the data on PC and cellphone both
       wifiversion : Only wifi-websocket,so you can monitor data on PC on tablet by wifi
       bluetoothversion : Only bluetooth ,you can monitor data by bluetooth
    3) ALL version can use OTA to update firmware by wifi
    4)Thermo compensate funciton also include

*/



#include <Arduino.h>
#include "TC4.h"

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "BluetoothSerial.h"
#include <EEPROM.h>
//#include "WebSerial.h"

// Thermo lib for MX6675
#include "max6675.h"
// Websockets Lib by links2004
#include <WebSocketsServer.h>
// JSON for Artisan Websocket implementation
#include "ArduinoJson.h"

#include "TC4_ThermalMeter.h"
#include "TC4_Indicator.h"
#include "TC4_RoR.h"


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Define three tasks
extern void TaskIndicator(void *pvParameters);
extern void TaskThermalMeter(void *pvParameters);
extern void TaskBatCheck(void *pvParameters);
extern void TaskROR(void *pvParameters);

// define other functions
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
String IpAddressToString(const IPAddress &ipAddress);                         //转换IP地址格式
void Bluetooth_Callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param); // bluetooth callback handler
void notFound(AsyncWebServerRequest *request);                                // webpage function
String processor(const String &var); // webpage function

char ap_name[30] ;


String BT_EVENT;
String local_IP;
uint32_t lastTimestamp = millis();
float last_BT_temp = -273.0;
bool take_temp = true;
uint8_t macAddr[6];


TaskHandle_t xHandle_indicator;

/*
     // Create a task, storing the handle.
     xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );

     // ...

     // Use the handle to suspend the created task.
     vTaskSuspend( xHandle );

*/
user_wifi_t user_wifi = {" ", " ", 0.0, 0.0, 0.75, 300,true};
temperature_data_t temperature_data= {0.0,0.0,0.0,0.0};

// object declare
AsyncWebServer  server_OTA(80);


#if defined(FULL_VERSION) || defined(WIFI_VERSION)
// WebSocketsServer declare
WebSocketsServer webSocket = WebSocketsServer(8080); //构建websockets类
#endif

#if defined(FULL_VERSION) || defined(BLUETOOTH_VERSION)
// bluetooth declare
BluetoothSerial BTSerial;

void Bluetooth_Callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{

    switch (event)
    {
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
#endif

String IpAddressToString(const IPAddress &ipAddress)
{
    return String(ipAddress[0]) + String(".") +
           String(ipAddress[1]) + String(".") +
           String(ipAddress[2]) + String(".") +
           String(ipAddress[3]);
}

#if defined(FULL_VERSION) || defined(WIFI_VERSION)

// Define Artisan Websocket events to exchange data
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{

    // Artisan schickt Anfrage als TXT
    // TXT zu JSON lt. https://forum.arduino.cc/t/assistance-parsing-and-reading-json-array-payload-websockets-solved/667917

    const size_t capacity = JSON_OBJECT_SIZE(3) + 60; // Memory pool
    DynamicJsonDocument doc(capacity);
    String temp_cmd_out = ""; // from websockets recived drumer control command and send out ;
    switch (type)
    {
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
        // DEBUG WEBSOCKET
        Serial.printf("[%u] get Text: %s\n", num, payload);

        // Extract Values lt. https://arduinojson.org/v6/example/http-client/
        // Artisan Anleitung: https://artisan-scope.org/devices/websockets/

        deserializeJson(doc, (char *)payload);

        // char* entspricht String
        String command = doc["command"].as< const char *>();


        // Serial_debug.printf("Command received: %s \n",command);

        long ln_id = doc["id"].as<long>();

        // Send Values to Artisan over Websocket
        JsonObject root = doc.to<JsonObject>();
        JsonObject data = root.createNestedObject("data");
        if (command == "getBT")
        {
            root["id"] = ln_id;
            data["BT"] = temperature_data.BT_AvgTemp;
            // Serial_debug.printf("getBT created BT: %4.2f \n",cmd_M1.TC1);
        }
        else if (command == "getET")
        {
            root["id"] = ln_id;
            data["ET"] = temperature_data.ET_AvgTemp;
            // Serial_debug.printf("getET created ET: %4.2f \n",cmd_M1.TC2);
        }

        else if (command == "getData")
        {
            root["id"] = ln_id;
            data["BT"] = temperature_data.BT_AvgTemp;
            data["ET"] = temperature_data.ET_AvgTemp;

            Serial.println("getData");
        }

        char buffer[200];                        // create temp buffer 200
        size_t len = serializeJson(doc, buffer); // serialize to buffer

   
        // send message to client
        // webSocket.sendTXT(num, "message here");

        // send data to all connected clients
        // webSocket.broadcastTXT("message here");
    }
    break;
    case WStype_BIN:
        // Serial_debug.printf("[%u] get binary length: %u\n", num, length);
        // hexdump(payload, length);

        // send message to client
        webSocket.sendBIN(num, payload, length);
        break;
    }
}

#endif

String processor(const String &var)
{
    Serial.println(var);
    if (var == "bt_compens")
    {
        return String(user_wifi.btemp_fix);
    }
    else if (var == "et_compens")
    {
        return String(user_wifi.etemp_fix);
    }
    else if (var == "version")
    {
        return VERSION;
    }
    else if (var == "sampling_time")
    { //
        return String(user_wifi.sampling_time);
    }
    else if (var == "sleeping_time")
    {
        return String(user_wifi.sleeping_time/60);
    }
    return String();
}

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Opps....Not found");
}

// low power mode; checks every few seconds for an event
void checkLowPowerMode(float temp_in)
{

// for debug 
//  user_wifi.Init_mode = true ;
//   user_wifi.sampling_time = 0.25; 
//   user_wifi.sleeping_time = 100;
//   EEPROM.put(0, user_wifi);
//    EEPROM.commit();

//    Serial.println("reset function done!");
//

    if (take_temp)
    {
        last_BT_temp = temp_in;   //设置第一次温度戳
        lastTimestamp = millis(); //设置第一次时间戳
        take_temp = false;
        // Serial.printf("last_BT_temp is : %f ",BT_AvgTemp);
    }

    if ((millis() - lastTimestamp) > (user_wifi.sleeping_time * 1000))
    {
        if (abs(last_BT_temp - temp_in) < 5)
        { // 60s
            take_temp = true;
            vTaskSuspend(xHandle_indicator);
            // 满足条件1:时间够60s and 条件2: 温度变化不超过5度
            // display.dim(true); //set OLED DIM
            display.clear(); // disable OLED
            
            display.display();      // disable OLE
            delay(1000);
            // set sleep mode
            esp_deep_sleep_start();
        }
        else
        {
            lastTimestamp = millis(); // update timestamp
        }
    }
}



void setup()
{

    xThermoDataMutex = xSemaphoreCreateMutex();
    //xIndicatorDataMutex = xSemaphoreCreateMutex();

    // Initialize serial communication at 115200 bits per second:
    Serial.begin(BAUDRATE);
    while (!Serial)
    {
        ; // wait for serial port ready
    }

    Serial.printf("\nTC4-WB  STARTING...\n");

#if defined(FULL_VERSION) || defined(BLUETOOTH_VERSION)
    // Initial Bluetooth Serial Port Profile (SPP)
    BTSerial.register_callback(Bluetooth_Callback);
    // Setup bluetooth device name as
    if (!BTSerial.begin(ap_name))
    {
        Serial.println("An error occurred during initialize");
    }
    else
    {
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

#endif
    Serial.printf("\nRead data from EEPROM...\n");
    // set up eeprom data
    EEPROM.begin(sizeof(user_wifi));
    EEPROM.get(0, user_wifi);


   //user_wifi.Init_mode = true ;

if (user_wifi.Init_mode) 
{
    strcat(user_wifi.ssid,"TC4-WB");
    strcat(user_wifi.password,"12345678");
    user_wifi.Init_mode = false ;
    user_wifi.sampling_time = 0.75; 
    user_wifi.sleeping_time = 300;
    user_wifi.btemp_fix = 0;
    user_wifi.etemp_fix = 0;
    

    EEPROM.put(0, user_wifi);
    EEPROM.commit();
}
    Serial.printf("\nStart Task...\n");
    /*---------- Task Definition ---------------------*/
    // Setup tasks to run independently.
    xTaskCreatePinnedToCore(
        TaskBatCheck, "bat_check" // 测量电池电源数据，每分钟测量一次
        ,
        1024 // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        NULL, 1 // Priority, with 1 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        NULL,  1 // Running Core decided by FreeRTOS,let core0 run wifi and BT
    );
    Serial.printf("\nbat_check...\n");
    xTaskCreatePinnedToCore(
        TaskThermalMeter, "ThermalMeter" // MAX6675 thermal task to read Bean-Temperature (BT)
        ,
        1024 // Stack size
        ,
        NULL, 3 // Priority
        ,
        NULL, 
        1 // Running Core decided by FreeRTOS,let core0 run wifi and BT
    );
    Serial.printf("\nThermalMeter...\n");
    xTaskCreatePinnedToCore(
        TaskIndicator, "IndicatorTask" // 128*64 SSD1306 OLED 显示参数
        ,
        1024*6 // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        NULL, 2 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        &xHandle_indicator, 
        1 // Running Core decided by FreeRTOS , let core0 run wifi and BT
    );
    Serial.printf("\nOLED...\n");
    xTaskCreatePinnedToCore(
        TaskROR, "RORTask" // 计算ROR的任务
        ,
        1024 * 8 // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        NULL, 3 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        NULL, 
        1 // Running Core decided by FreeRTOS , let core0 run wifi and BT
    );   
     Serial.printf("\nROR ...\n");

     Serial.printf("\nStart Wifi ...\n");
     
  //初始化网络服务
    WiFi.mode(WIFI_STA);
    WiFi.begin(user_wifi.ssid, user_wifi.password);

    byte tries = 0;
    while (WiFi.status() != WL_CONNECTED)
    {

        delay(1000);
        Serial.println("wifi not ready");

        if (tries++ > 10)
        {
            WiFi.macAddress(macAddr); 
            // Serial_debug.println("WiFi.mode(AP):");
            WiFi.mode(WIFI_AP);
            sprintf( ap_name ,"TC4-WB_%02x%02x",macAddr[4] ,macAddr[5]);
            WiFi.softAP(ap_name, "12345678"); // defualt IP address :192.168.4.1 password min 8 digis
            break;
        }
        // show AP's IP
    }


    Serial.print("TC4-WB's IP:");

    if (WiFi.getMode() == 2) // 1:STA mode 2:AP mode
    {
        Serial.println(IpAddressToString(WiFi.softAPIP()));
        local_IP = IpAddressToString(WiFi.softAPIP());
    }
    else
    {
        Serial.println(IpAddressToString(WiFi.localIP()));
        local_IP = IpAddressToString(WiFi.localIP());
    }

#if defined(FULL_VERSION) || defined(WIFI_VERSION)
    // init websocket
    webSocket.begin();
    Serial.println("WebSocket started!");

    // event  websocket handler
    webSocket.onEvent(webSocketEvent);
#endif

    // for index.html
    server_OTA.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send_P(200, "text/html", index_html, processor); });

    // get the value from index.html
    server_OTA.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
//get value form webpage      
    strncpy(user_wifi.ssid,request->getParam("ssid")->value().c_str(), sizeof(user_wifi.ssid) );
    strncpy(user_wifi.password,request->getParam("password")->value().c_str(), sizeof(user_wifi.password) );
    user_wifi.ssid[request->getParam("ssid")->value().length()] = user_wifi.password[request->getParam("password")->value().length()] = '\0';  
//Svae EEPROM 
    EEPROM.put(0, user_wifi);
    EEPROM.commit();
//output wifi_sussce html;
    request->send_P(200, "text/html", wifi_sussce_html); });

    server_OTA.on("/compens", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                      // get value form webpage
                      if (request->getParam("Btemp_fix")->value() != "")
                      {
                          user_wifi.btemp_fix = request->getParam("Btemp_fix")->value().toFloat();
                      }
                      if (request->getParam("Etemp_fix")->value() != "")
                      {
                          user_wifi.etemp_fix = request->getParam("Etemp_fix")->value().toFloat();
                      }
                      // Svae EEPROM
                      EEPROM.put(0, user_wifi);
                      EEPROM.commit();
                  });

    server_OTA.on("/other", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                      // get value form webpage
                      if (request->getParam("sampling_time")->value() != "")
                      {
                          user_wifi.sampling_time = request->getParam("sampling_time")->value().toFloat();
                      }
                      if (request->getParam("sleeping_time")->value() != "")
                      {
                          user_wifi.sleeping_time = request->getParam("sleeping_time")->value().toInt() * 60; // input in MINUTES covernet to seconds
                      }
                      // Svae EEPROM
                      EEPROM.put(0, user_wifi);
                      EEPROM.commit();
                  });

    server_OTA.onNotFound(notFound); // 404 page seems not necessary...

    AsyncElegantOTA.begin(&server_OTA); // Start ElegantOTA


    server_OTA.begin();

    Serial.println("HTTP server started");
}

void loop()

{
#if defined(FULL_VERSION) || defined(WIFI_VERSION)
    webSocket.loop(); //处理websocketmie
#endif

#if defined(FULL_VERSION) || defined(BLUETOOTH_VERSION)
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
        if (msg.indexOf("READ") == 0)
        {                               // READ command
            BTSerial.print(temperature_data.ET_AvgTemp); // channel 1 : Environment Temperature (ET);
            BTSerial.print(",");
            BTSerial.print(temperature_data.BT_AvgTemp);     // channel 2 : Bean Temperature (BT) with degree Celsius
            BTSerial.println(",0.00,0.00"); // channel 3,4 : A vaule of zero indicates the channel is inactive


        }

        /* CHAN command */
        else if (msg.indexOf("CHAN;") == 0)
        {
            BTSerial.print("#OK");
            Serial.println("Artisan \"CHAN\"");
        }
        /* FILT command */
        else if (msg.indexOf("FILT;") == 0)
        {
            BTSerial.print("#OK");
            Serial.println("Artisan \"FILT\"");
        }
        /* Unhandle command */
        else
        {
            Serial.println("Artisan Unhandle command");
            Serial.println(msg);
        }
    }
#endif


    checkLowPowerMode(temperature_data.BT_AvgTemp); //测量是否进入睡眠模式



}