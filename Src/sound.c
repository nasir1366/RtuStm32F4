#include "includes.h"
#include "fatfs.h"
#include "defines.h"
#include "global_var.h"
#include "sound.h"
#include "VS1003B.h"
#include "main_func.h"
#include "my_delay.h"

void play_start(unsigned char num){
  /*
  uint16_t cou;
  
  amp_pwr(1);
  voice_pwr_on;
  delayms(100);
  VS1003B_SoftReset();  
  res=VS1003B_Init();
    
  res=f_mount(&SDFatFs,(TCHAR const*)SDPath,1); 
  sprintf(buf,"%d.mp3",num);
  res=f_open(&File[0],buf,FA_READ);
  
  VS1003B_SetVolume(0);
  
  while(flag.ply_alarm==1){
    flag.dmatranfercplt=0;
    BytesRead=0;
    res = f_read(&File[0],PlyBuffer,512,(void *)&BytesRead);
//    while(flag.dmatranfercplt==0){
//      tgl_wdt;
//    }
    if(BytesRead<512){
      f_close(&File[0]);
      
      break;
    }
    for(cou=0;cou<16;)
    {
      tgl_wdt;
      if(VS1003B_NeedData())
      {
        VS1003B_Write32B(&PlyBuffer[cou*32]);
        cou++;   
      }
    }
            
  }  
  */
    
}

void play_stop(){
  /*
  f_close(&File[0]);
  voice_pwr_off;
  flag.ply_alarm=0;
  */
    
}

void record_start(unsigned char num){
  
}

void record_stop(){
  
  
}