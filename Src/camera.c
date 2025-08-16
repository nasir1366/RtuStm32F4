#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
#include "includes.h"
#include "gsm.h"
#include "global_var.h"
#include "my_delay.h"
#include "usart.h"
#include "rtc.h"
#include "ir.h"
#include "eeprom_ext.h"
#include "lcdlib.h"
#include "temperature.h"
#include "main_func.h"
#include "sound.h"
#include "usart.h"
#include "camera.h"

uint8_t read_camera(char pic_size){

	
	PictureSize=0;
  CamBufOvf=0;
   
  mmc_pwr_on;
  camera_on;
	
//  usart_init(USART2,115200);
  delayms(2000);
   
	
  RS485_send(camera_reset,sizeof(camera_reset));
  delayms(2000);
//  RS232_send(camera_changeboud,sizeof(camera_changeboud));
//  delayms(1000);
//  UsartInit(&USARTE0,&PORTE,12,0x40);
//  delayms(500);
  
  if(pic_size==1){
    RS485_send(camera_ChangePicSizeLarge,sizeof(camera_ChangePicSizeLarge)); 
    delayms(2000);
  }
  else if(pic_size==0){
    RS485_send(camera_ChangePicSizeSmall,sizeof(camera_ChangePicSizeSmall)); 
    delayms(2000);  
  }
  light_on;
	delayms(500);
  RS485_send(camera_TakePicture,sizeof(camera_TakePicture));
  delayms(2000);
  CamRecLen=0;
	CamBufOvf=0;
	
	HAL_UART_Receive_DMA(&huart2,CameraBuffer,9);
	light_off;
  RS485_send(camera_ReadFileSize,sizeof(camera_ReadFileSize));
//  delayms(1000);
	
	TakePictureCount=0;
  while(CamBufOvf==0 && TakePictureCount<6){tgl_wdt;}
  if(CamBufOvf==0){

    mmc_pwr_off;
    camera_off;
		light_off;
    return 2; 
		
	}
  
  camera_ReadFileContent[12]=CameraBuffer[7];
  camera_ReadFileContent[13]=CameraBuffer[8];
  
  PictureSize=((unsigned int)CameraBuffer[7]<<8) | (unsigned int)CameraBuffer[8];
  if(PictureSize>60000){
		mmc_pwr_off;
    camera_off;
		light_off;
		return 1;
	}
	
  PictureRecProcess=1;
    
  
	
//	BufPointer485=CameraBuffer;
	HAL_UART_Receive_DMA(&huart2,CameraBuffer,5);
  RS485_send(camera_ReadFileContent,sizeof(camera_ReadFileContent));
	CamBufOvf=0;
	TakePictureCount=0;
	while(CamBufOvf==0 && TakePictureCount<6){tgl_wdt;}
	
	CamRecLen=0;
//	RecSize485=CameraBufferSize;
//	write_size=CameraBufferSize;
//	RS485stat=0;
//	BufPointer485=CameraBuffer;
	HAL_UART_Receive_DMA(&huart2,MemBuf,PictureSize);
  CamBufOvf=0;
  TakePictureCount=0;
	while(CamBufOvf==0 && TakePictureCount<10){tgl_wdt;}
	if(TakePictureCount>=10){
		mmc_pwr_off;
    camera_off;
		light_off;
    return 3;
	}
	
	camera_off;
	light_off;
return 0;
}
//-----------------------------------------------------------
uint8_t get_picture(char pic_size,char* picture_name){
	
	if(flag.LcdOn==1){
    LCDclr();
		LCDputs("Taking Pic...");
  }

	res=read_camera(pic_size);
	if(res!=0)
		return 1;
	
	enter_critical_section();
	f_mount(&SDFatFs,(TCHAR const*)SDPath,1);
	f_unlink(picture_name);
  delayms(2000);
  if(f_open(&File[0],picture_name,FA_WRITE | FA_OPEN_ALWAYS) != FR_OK) {
    
    sprintf(SMS_text,"SD Card Fail:%d",res);
		SendSMS((char*)ServerPhone,SMS_text);
		delayms(10000);
		HAL_NVIC_SystemReset(); 
  }
	if(f_write(&File[0],MemBuf,PictureSize,&FatBytesWritten)!=FR_OK){
		sprintf(SMS_text,"SD Card Fail:%d",res);
		SendSMS((char*)ServerPhone,SMS_text);
		delayms(10000);
		HAL_NVIC_SystemReset(); 
	}
	f_close(&File[0]);
	exit_critical_section();
	return 0;
	
}
//---------------------------------------------------------
uint8_t log_picture(char pic_size){
  
	res=read_camera(pic_size);
	if(res!=0)
		return 1;
	
	enter_critical_section();
	f_mount(&SDFatFs,(TCHAR const*)SDPath,1);
  	
	res=f_mkdir("picture");
	sprintf(CameraBuffer,"picture/%04d_%02d",clock.year,clock.month);
	res=f_mkdir(CameraBuffer);
		
  sprintf(CameraBuffer,"picture/%04d_%02d/%02d_%02d_%02d.jpg",clock.year,clock.month,clock.day,clock.hour,clock.min);  
  f_unlink(CameraBuffer);
  delayms(2000);
  if(f_open(&File[0],CameraBuffer,FA_WRITE | FA_OPEN_ALWAYS) != FR_OK) {
    
    sprintf(SMS_text,"SD Card Fail:%d",res);
		SendSMS((char*)ServerPhone,SMS_text);
		delayms(10000);
		HAL_NVIC_SystemReset(); 
  }
	if(f_write(&File[0],MemBuf,PictureSize,&FatBytesWritten)!=FR_OK){
		sprintf(SMS_text,"SD Card Fail:%d",res);
		SendSMS((char*)ServerPhone,SMS_text);
		delayms(10000);
		HAL_NVIC_SystemReset(); 
	}
	f_close(&File[0]);
	exit_critical_section();
	return 0;
	
	 
   
}



