#ifndef ADC_INCLUDED
#define ADC_INCLUDED

#include "includes.h"

#define Ain1_channel  ADC_CHANNEL_4
#define Ain1 hadc1
#define Ain2_channel  ADC_CHANNEL_5
#define Ain2 hadc1
#define Ain3_channel  ADC_CHANNEL_15
#define Ain3 hadc3
#define Ain4_channel  ADC_CHANNEL_9
#define Ain4 hadc3
#define Ain5_channel  ADC_CHANNEL_14
#define Ain5 hadc3
#define Ain6_channel  ADC_CHANNEL_3
#define Ain6 hadc1


#define BatteryVoltageADCChannel  Ain1_channel
#define BatteryVoltageADC Ain1
#define BatteryVoltageADCScale ADC1Scale

#define SolarVoltageADCChannel  Ain2_channel
#define SolarVoltageADC Ain2
#define SolarVoltageADCScale ADC2Scale


float READ_ADC(ADC_HandleTypeDef adc,uint32_t channel,float scale);
#endif
