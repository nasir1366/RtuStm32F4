#include "includes.h"
#include "my_delay.h"
#include "global_var.h"


float READ_ADC(ADC_HandleTypeDef adc,uint32_t channel,float scale){
ADC_ChannelConfTypeDef adConfig;
float value=0;
	uint16_t avg=0; 
	
	adConfig.Channel = channel;
  adConfig.Rank = 1;
  adConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&adc, &adConfig) != HAL_OK)
  {
    return 0;
  }
	
  HAL_ADC_ConfigChannel(&adc, &adConfig);
	for(avg=0;avg<100;avg++){
		HAL_ADC_Start(&adc);
		HAL_ADC_PollForConversion(&adc,10);
		value+=(float)HAL_ADC_GetValue(&adc)/100;
	}
  value=((value+1)/4096)*2.5;
  value*=scale;
	return value;
	
}

