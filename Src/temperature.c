#include "includes.h"
#include "defines.h"
#include "temperature.h"
#include "global_var.h"
#include "my_delay.h"
#include "lcdlib.h"
#include "usart.h"
#include "main_func.h"
#include <stdio.h>

extern HAL_StatusTypeDef stat;

void temp_lm75(signed char*tmp){
int16_t temp1=0,temp2=0;   

  i2c2_pwr_on;
  iic_pwr_count=0;
  delayms(50);
  
  stat=HAL_I2C_Mem_Read(&hi2c2,lm75_adr,(uint8_t)0,I2C_MEMADD_SIZE_8BIT,(uint8_t*)(&temp1),2,i2c_timeout);
     
  *tmp=temp1;
  
}

void ReadExtTemp(signed char*tmp){
  	
	if(temp_type & SHT20){
		sht_on;
		rs485_select=2;
		usart_init(USART2,9600);
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
		
		delayms(100);
		sht_send_buffer[0]=0x10;
		sht_send_buffer[1]=0x20;
		sht_send_buffer[2]=0x30;
		temp_rec_len=0;
		RS485_send(sht_send_buffer,3);
		loop_timeout=0;
		while(temp_rec_len<5 && loop_timeout<2){tgl_wdt;}
		if(temp_rec_len==5){
			temp_out=((uint16_t)sht_rec_buffer[1]|((uint16_t)sht_rec_buffer[2]<<8))/100;
			humidity=((uint16_t)sht_rec_buffer[3]|((uint16_t)sht_rec_buffer[4]<<8))/100;
		}
		else{
			temp_out=0;
			humidity=0;
		}
		sht_off;
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
	}
}