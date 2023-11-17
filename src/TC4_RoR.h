#ifndef __TC4_ROR_H__
#define __TC4_ROR_H__


#include "TC4.h"

float BT_ROR_TempArray[TEMPERATURE_ROR_LENGTH] = {0.0}; // temperature array
float ET_ROR_TempArray[TEMPERATURE_ROR_LENGTH] = {0.0}; // temperature array
int j=0;
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
        j++;   
        //Serial.printf("\nJ is now : %d",j);

            if (xSemaphoreTake(xThermoDataMutex, xIntervel) == pdPASS)  //给温度数组的最后一个数值写入数据
                    {
                    BT_ROR_TempArray[TEMPERATURE_ROR_LENGTH] = temperature_data.BT_AvgTemp ;
                    ET_ROR_TempArray[TEMPERATURE_ROR_LENGTH] = temperature_data.ET_AvgTemp ;
                    xSemaphoreGive(xThermoDataMutex);  //end of lock mutex
                    }
   
        //读取数据，并移位温度数组
            if (j <TEMPERATURE_ROR_LENGTH){  //如果是数组第一位就读取新的数据
                 //Serial.printf("\n array counst ");     
                            BT_ROR_TempArray[j] = BT_ROR_TempArray[j+1];
                            ET_ROR_TempArray[j] = ET_ROR_TempArray[j+1];
                           // Serial.printf("ET array %d:",j);
                           // Serial.println(ET_ROR_TempArray[j] );
                 }    
                 
                if (j ==  TEMPERATURE_ROR_LENGTH)  {

                    j=0;
                     }          

            if (xSemaphoreTake(xThermoDataMutex, xIntervel) == pdPASS) 
                {
                temperature_data.BT_ROR = ROR(BT_ROR_TempArray  ,TEMPERATURE_ROR_LENGTH) * (60 / TEMPERATURE_ROR_LENGTH);
                temperature_data.ET_ROR = ROR(ET_ROR_TempArray  ,TEMPERATURE_ROR_LENGTH) * (60 / TEMPERATURE_ROR_LENGTH);
                xSemaphoreGive(xThermoDataMutex);  //end of lock mutex
                    }   
}
}

float ROR( float y_signal[TEMPERATURE_ROR_LENGTH],const int num )
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



/*
 code from TC4 

// T1, T2 = temperatures x 1000
// t1, t2 = time marks, milliseconds
// ---------------------------------------------------
float calcRise( int32_t T1, int32_t T2, int32_t t1, int32_t t2 ) {
  int32_t dt = t2 - t1;
  if( dt == 0 ) return 0.0;  // fixme -- throw an exception here?
  float dT = ( convertUnits( T2 ) - convertUnits( T1 ) ) * D_MULT;
  float dS = dt * 0.001; // convert from milli-seconds to seconds
  return ( dT / dS ) * 60.0; // rise per minute
}

*/


#endif