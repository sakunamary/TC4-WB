/*  TC4-WB for Artisan Coffee Roaster Application

    Released under BSD-NC-3.0 License

    Created by Sakunamary on 2022


    ThermalMeter Task
    The default sample rate of Artisan is 3 seconds, although the setting value can be modified by user.
    I think this value is generated from lots of experimental, so keeps polling-time as 3 seconds.
    Thus, In the ThermalMeter Task will (750ms x 4 = 3s)
    (1) read MAX6675 temperature evey 750ms, and
    (2) Average-Array length is 4.
    That is, the measured temperature is avaraged from 4 times of MAX6675 temperature reading.

*/
#ifndef __TC4_THERMALMETER_H__
#define __TC4_THERMALMETER_H__

#include <Arduino.h>
#include <SimpleKalmanFilter.h>
#include "TC4.h"
#include "max6675.h"


#define ABNORMAL_TEMPERATURE_DEGREE 100 // defin abnormal temperature value



float BT_TempArray[TEMPERATURE_ARRAY_LENGTH] = {0.0}; // temperature array
float ET_TempArray[TEMPERATURE_ARRAY_LENGTH] = {0.0}; // temperature array
int BT_ArrayIndex = 0;                                // A pointer of temperature array
int ET_ArrayIndex = 0;                                // A pointer of temperature array
float BT_CurTemp = 0.0;
float ET_CurTemp = 0.0;


bool bReady = false;         // flag to indicate temperature array whether is ready or not
bool bAbnormalValue = false; // indicate temperature value is unexpect or not

int thermoDO = 19;
int thermoCLK = 5;
int thermoCS_ET = 16;
int thermoCS_BT = 17;


SemaphoreHandle_t xThermoDataMutex = NULL;

// Create Variables and Object for Environment Temperature (ET) if exist
MAX6675 thermocouple_ET(thermoCLK, thermoCS_ET, thermoDO);

// Create object for Beam Temperature (BT)
MAX6675 thermocouple_BT(thermoCLK, thermoCS_BT, thermoDO);

SimpleKalmanFilter simpleKalmanFilter_BT(1, 1, 0.01);
SimpleKalmanFilter simpleKalmanFilter_ET(1, 1, 0.01);

float averageTemperature(float temp[TEMPERATURE_ARRAY_LENGTH],const int num );

int l = 0 ;

void TaskThermalMeter(void *pvParameters)
{ //function 

    /* Variable Definition */
    (void)pvParameters;
    TickType_t xLastWakeTime;

    const TickType_t xIntervel = (user_wifi.sampling_time * 1000) / portTICK_PERIOD_MS;
   //const TickType_t xIntervel = (2 * 1000) / portTICK_PERIOD_MS;
    /* Task Setup and Initialize */
    // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    for (;;) // A Task shall never return or exit.
    { //for loop
        // Wait for the next cycle (intervel 750ms).
        vTaskDelayUntil(&xLastWakeTime, xIntervel);
        l++;
        // Perform task actions from here
        // Read BT from MAX6675 thermal couple

           //读取max6675数据
              BT_CurTemp = simpleKalmanFilter_BT.updateEstimate(round(thermocouple_BT.readCelsius()*10)/10) + user_wifi.btemp_fix; //增加简单卡尔曼滤波
              ET_CurTemp = simpleKalmanFilter_ET.updateEstimate(round(thermocouple_ET.readCelsius()*10)/10) + user_wifi.etemp_fix; //增加简单卡尔曼滤波
              
            //BT_CurTemp = thermocouple_BT.readCelsius() + user_wifi.btemp_fix;  //get BT data
            //ET_CurTemp = thermocouple_ET.readCelsius() + user_wifi.etemp_fix;

     vTaskDelay(100);
         
                // Means, first round of temperature array is done,
                // The averaged temperyure is ready for reading

                if (xSemaphoreTake(xThermoDataMutex, xIntervel) == pdPASS)  //给温度数组的最后一个数值写入数据
                {//lock the  mutex       
                        BT_TempArray[TEMPERATURE_ARRAY_LENGTH]= BT_CurTemp;
                        ET_TempArray[TEMPERATURE_ARRAY_LENGTH]= ET_CurTemp;
                        xSemaphoreGive(xThermoDataMutex);  //end of lock mutex
                }   


        //读取数据，并移位温度数组
            if (l <TEMPERATURE_ARRAY_LENGTH){  //如果是数组第一位就读取新的数据
                 //Serial.printf("\n array counst ");     
                            BT_TempArray[l] = BT_TempArray[l+1];
                            ET_TempArray[l] = ET_TempArray[l+1];
 
                 }    
                 
                if (l ==  TEMPERATURE_ARRAY_LENGTH)  {

                    l=0;
                     }          

            if (xSemaphoreTake(xThermoDataMutex, xIntervel) == pdPASS) 
                {
                temperature_data.BT_AvgTemp  = averageTemperature(BT_TempArray  ,TEMPERATURE_ARRAY_LENGTH) ;
                temperature_data.ET_AvgTemp  = averageTemperature(ET_TempArray  ,TEMPERATURE_ARRAY_LENGTH) ;
                xSemaphoreGive(xThermoDataMutex);  //end of lock mutex
                    }   
    }
}//function 



// A function to average temperature array
float averageTemperature(float temp[TEMPERATURE_ARRAY_LENGTH],const int num )
{
    float avg ;
    int k;
    float sum=0.0;

    for(k=1;k<=num;k++)
    {
       sum=sum+temp[k];
    }
    avg = sum/num;
    return avg;
}




#endif
