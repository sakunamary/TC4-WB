#ifndef __TC4_ROR_H__
#define __TC4_ROR_H__


#include "TC4.h"

float BT_ROR_TempArray[TEMPERATURE_ROR_LENGTH] = {0.0}; // temperature array
float ET_ROR_TempArray[TEMPERATURE_ROR_LENGTH] = {0.0}; // temperature array
int j=1;
float ROR( float y_signal[TEMPERATURE_ROR_LENGTH],const int num );

void TaskROR(void *pvParameters)
{

     /* Variable Definition */
    (void)pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = ( 1* 1000) / portTICK_PERIOD_MS; 

        // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

        for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xIntervel);

   
        //读取数据，并移位温度数组
         while (j <= TEMPERATURE_ROR_LENGTH){
            if (j == TEMPERATURE_ROR_LENGTH){  //如果是数组第一位就读取新的数据
                if (xSemaphoreTake(xThermoDataMutex, xIntervel) == pdPASS) 
                 {
                   BT_ROR_TempArray[j] = temperature_data.BT_AvgTemp ;
                   ET_ROR_TempArray[j] = temperature_data.ET_AvgTemp ;
                   xSemaphoreGive(xThermoDataMutex);  //end of lock mutex
                    }
                    Serial.print("ET array 15:");
                    Serial.println(ET_ROR_TempArray[j] );
                    j=1;
                    break;
                }else { //如果不是数据第一位就移动位置
                    BT_ROR_TempArray[j] = BT_ROR_TempArray[j-1];
                    ET_ROR_TempArray[j] = ET_ROR_TempArray[j-1];
                     Serial.printf("BT array %d:",j);
                    Serial.println(BT_ROR_TempArray[j] );
                     Serial.printf("ET array %d:",j);
                    Serial.println(ET_ROR_TempArray[j] );
                }
                j++;
         }

        if (xSemaphoreTake(xThermoDataMutex, xIntervel) == pdPASS) 
            {
                temperature_data.BT_ROR = ROR(BT_ROR_TempArray  ,TEMPERATURE_ROR_LENGTH) * 2;
                
                temperature_data.ET_ROR = ROR(ET_ROR_TempArray  ,TEMPERATURE_ROR_LENGTH) * 2;
                xSemaphoreGive(xThermoDataMutex);  //end of lock mutex
            }
    }
}


float ROR( float y_signal[],const int num )
{

    // x 轴 为固定时间区间点，跟采集时间保持一致。 Y轴为温度输入点。
    float sumx = 0.0;
    float sumy = 0.0;
    float sumx2 = 0.0;
    float sumxy = 0.0;
    float ROR =0.0;
     for (int i = 1 ;i <= num ;i++)
     {
        sumx += i;
        sumy += y_signal[i];
        sumx2 +=  i * i;
        sumxy +=  i *  y_signal[i];
     }


     ROR =(num * sumxy - sumy * sumx) / (num * sumx2 - sumx * sumx);

     return ROR;
}



#endif