#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
#include "includes.h"
#include "global_var.h"
#include "my_delay.h"
#include "usart.h"
#include "rtc.h"
#include "ir.h"
#include "eeprom_ext.h"
#include "lcdlib.h"
#include "temperature.h"
#include "main_func.h"
#include "wifi.h"
#include "gsm.h"

void wifi_send_cmd(char* frame){
	
	HAL_UART_Transmit(&huart1,(uint8_t*)frame,strlen((char*)frame),100);

}

void wifi_init(){

	wifi_pwr_on;
	delayms(3000);
	wifi_rec_index=0;
	flush_buf(wifi_rec_buffer,250);
	wifi_send_cmd("AT\r\n");
	delayms(300);
	wifi_rec_index=0;
	flush_buf(wifi_rec_buffer,250);
	wifi_send_cmd("AT+CWMODE=2\r\n");	
	delayms(300);
	wifi_rec_index=0;
	flush_buf(wifi_rec_buffer,250);
	wifi_send_cmd("AT+CWSAP=\"NASIR\",\"1234567890\",5,3,1,0\r\n");
	delayms(500);
}