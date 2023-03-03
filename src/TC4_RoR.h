#ifndef __TC4_ROR_H__
#define __TC4_ROR_H__


#include "TC4.h"

float BT_ROR_TempArray[TEMPERATURE_ROR_LENGTH] = {0.0}; // temperature array
float ET_ROR_TempArray[TEMPERATURE_ROR_LENGTH] = {0.0}; // temperature array
int j;
float ROR( float y_signal[TEMPERATURE_ROR_LENGTH],const int num );

void TaskROR(void *pvParameters)
{

     /* Variable Definition */
    (void)pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xIntervel = (user_wifi.sampling_time * 1000) / portTICK_PERIOD_MS; 

        // Initial the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

        for (;;) // A Task shall never return or exit.
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xIntervel);

        j= TEMPERATURE_ROR_LENGTH ; 
        //读取数据，并移位
         while (j >= 1){
        
            if (j==1){  //如果是数组第一位就读取新的数据
                if (xSemaphoreTake(xThermoDataMutex, xIntervel) == pdPASS) 
                 {
                   BT_ROR_TempArray[j] = temperature_data.BT_AvgTemp ;
                   ET_ROR_TempArray[j] = temperature_data.ET_CurTemp ;
                   xSemaphoreGive(xThermoDataMutex);  //end of lock mutex
                    }
                }else { //如果不是数据第一位就移动位置
                    BT_ROR_TempArray[j] = BT_ROR_TempArray[j-1];
                    ET_ROR_TempArray[j] = ET_ROR_TempArray[j-1];
                }
                j--;
         }

        if (xSemaphoreTake(xThermoDataMutex, xIntervel) == pdPASS) 
            {
                //temperature_data.BT_ROR =  BT_ROR_TempArray[1];
                //temperature_data.ET_ROR =  ET_ROR_TempArray[1];
                temperature_data.BT_ROR = ROR(BT_ROR_TempArray  ,TEMPERATURE_ROR_LENGTH);
                temperature_data.ET_ROR = ROR(ET_ROR_TempArray  ,TEMPERATURE_ROR_LENGTH);
                xSemaphoreGive(xThermoDataMutex);  //end of lock mutex
            }

    }
}


float ROR( float y_signal[],const int num )
{

    // x 轴 为固定时间区间点，跟采集时间保持一致。 Y轴为温度输入点。
    float sum_x = 0.0;
    float sum_y = 0.0;
    float sum_x2 = 0.0;
    float sum_xy = 0.0;
    float ROR =0.0;
     for (int i = 1 ;i <= num ;i++)
     {
        sum_x += i;
        sum_y += y_signal[i];
        sum_x2 +=  i * i;
        sum_xy +=  i *  y_signal[i];
     }
     ROR =(num * sum_xy - sum_x * sum_y ) / (num * sum_x2 - sum_x * sum_x) ;

     return ROR;
}



#endif