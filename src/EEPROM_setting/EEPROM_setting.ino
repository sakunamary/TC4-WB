/*
   EEPROM Write

   Stores random values into the EEPROM.
   These values will stay in the EEPROM when the board is
   turned off and may be retrieved later by another sketch.
*/

#include "EEPROM.h"

// 网页设置的参数
 typedef struct eeprom_settings 
{
  char ssid[60]; //增加到30个字符
  char password[60]; //增加到30个字符
  float  btemp_fix;
  float  etemp_fix;
  double sampling_time;//采样时间   单位：s
  int    sleeping_time ;//休眠时间  单位：s
  bool   Init_mode ; //是否初始化模式
} user_wifi_t;

extern user_wifi_t  user_wifi ;


void setup()
{
  Serial.begin(BAUDRATE);
  Serial.println("start...");
  if (!EEPROM.begin(sizeof(user_wifi)))
  {
    Serial.println("failed to initialise EEPROM"); 
    delay(1000000);
  } else {
    Serial.println("Initialed EEPROM,data will be writen after 3s..."); 
    delay(3000);
    EEPROM.get(0, user_wifi);
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

  Serial.println(" bytes read from Flash . Values are:");

  for (int i = 0; i < sizeof(user_wifi); i++)
  {
    Serial.print(byte(EEPROM.read(i))); Serial.print(" ");
  }

}


void loop()
{
  
}
