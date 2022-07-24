

#include <Arduino.h>
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


void setup(){



}


void loop(){
    
}