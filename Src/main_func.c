
#include "includes.h"
#include "fatfs.h"

#include "defines.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global_var.h"
#include "my_delay.h"
#include "usart.h"
#include "rtc.h"
#include "ir.h"
#include "eeprom_ext.h"
#include "lcdlib.h"
#include "temperature.h"
#include "main_func.h"
#include "gsm.h"
#include "sound.h"
#include "adc.h"


void read_sensours(unsigned char*mem){
  
  float ad_value=0;
  mem[0]=0;    
  mem[0] |= valid_door_flag<<5;
  //-----read bat voltage-----------
    
  ad_value=READ_ADC(BatteryVoltageADC,BatteryVoltageADCChannel,BatteryVoltageADCScale);
  mem[1]=(uint8_t)ad_value;
  mem[2]=(uint8_t)((ad_value-mem[1])*100);
  //read solar voltage---------
  
	ad_value=READ_ADC(SolarVoltageADC,SolarVoltageADCChannel,SolarVoltageADCScale);
	mem[3]=(uint8_t)ad_value;
	mem[4]=(uint8_t)((ad_value-mem[3])*100);
  
  temp_lm75(&temp_in);
  
  mem[5] = temp_in;
  if(ext_device & temp_sensor){
    ReadExtTemp(&temp_out);
    mem[6] = temp_out; 
  }
  
  else
    mem[6]=0;
  
  if(ext_device & water_sensor){
    read_water_level();
    mem[7]=(unsigned char)water_level;
    mem[8]=(unsigned char)(water_level>>8);
  }
  else{ mem[7]=0;mem[8]=0;}
	
  if(ext_device & rain_sensor){
		mem[9]=(unsigned char)rain_level;
		mem[10]=(unsigned char)((rain_level-(float)mem[9])*100); 
  }
  
}
//----------------------------------------------------------
void ReadSensorToStr(char *str){
	float ad_value=0;
	uint16_t str_len=0,temp_int=0,temp_flt=0;
	
	sprintf(str,"%04d/%02d/%02d,%02d:%02d,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min);
  //-----read bat voltage-----------
  ad_value=READ_ADC(BatteryVoltageADC,BatteryVoltageADCChannel,BatteryVoltageADCScale);
  str_len=strlen(str);
	
	sprintf(str+str_len,"bat=%4.2f,\r\n",ad_value);
	
  //read solar voltage---------
  
	ad_value=READ_ADC(SolarVoltageADC,SolarVoltageADCChannel,SolarVoltageADCScale);
	str_len=strlen(str);
	sprintf(str+str_len,"sol=%4.2f,\r\n",ad_value);
  
  temp_lm75(&temp_in);
  str_len=strlen(str);
	sprintf(str+str_len,"Tin=%d,\r\n",temp_in);
  
  if(ext_device & temp_sensor){
    ReadExtTemp(&temp_out);
    str_len=strlen(str);
		sprintf(str+str_len,"Tout=%d,Humidity=%d,\r\n",temp_out,humidity);
  }
    
  if(ext_device & water_sensor){
    read_water_level();
		str_len=strlen(str);
		sprintf(str+str_len,"Wlvl=%d,\r\n",water_level);
    
  }
  
  if(ext_device & rain_sensor){
		str_len=strlen(str);
		sprintf(str+str_len,"Rn=%4.2f,\r\n",rain_level);
  }	
	
	if(flag.test_mode==1){
		strcat(str,"TestMode,\r\n");
	}
	
	str_len=strlen(str);
	*(str+str_len)=0;
	
//	str_len=strlen(str);
//	strcpy(str+str_len,"\r\n");
//	*(str+str_len)='\r';
//	*(str+str_len+1)='\n';
//	*(str+str_len+2)=0;

}

//------------------------------------------------------

unsigned char log_data(){
    
  UINT n;
	float ad_value=0;
	uint16_t str_len=0,temp_int=0,temp_flt=0;
  	  
//  if(mmc_detect==0){
	
	/*
	sprintf((char*)buf,"%d",clock.year);
	res=f_mkdir(buf);
	sprintf((char*)buf,"%d/%d",clock.year,clock.month);
	res=f_mkdir(buf);
	sprintf((char*)buf,"%d/%d/text",clock.year,clock.month);
	res=f_mkdir(buf);
	
	++++++++++++++++++++++++++++++++++++++++++++++
	sprintf((char*)buf,"%d/%d/text/%d.txt",clock.year,clock.month,clock.day);
	*/
	
	sprintf((char*)log_buffer,"%04d/%02d/%02d,%02d:%02d,",clock.year,clock.month,clock.day,clock.hour,clock.min);
	
	if(LogParameters & LogBatvolt){
		ad_value=READ_ADC(BatteryVoltageADC,BatteryVoltageADCChannel,BatteryVoltageADCScale);
		temp_int=(uint16_t)ad_value;
		temp_flt=(uint16_t)((ad_value-temp_int)*100);
		str_len=strlen((char*)log_buffer);
		sprintf((char*)log_buffer+str_len,"bat=%d.%d,",temp_int,temp_flt);

	}
	if(LogParameters & LogSolarvolt){
		ad_value=READ_ADC(SolarVoltageADC,SolarVoltageADCChannel,SolarVoltageADCScale);
		str_len=strlen((char*)log_buffer);
		temp_int=(uint16_t)ad_value;
		temp_flt=(uint16_t)((ad_value-temp_int)*100);
		sprintf((char*)log_buffer+str_len,"sol=%d.%d,",temp_int,temp_flt);

	}
	if(LogParameters & LogTemperature){
		temp_lm75(&temp_in);
		str_len=strlen((char*)log_buffer);
		sprintf((char*)log_buffer+str_len,"Tin=%d,",temp_in);
		
		if(ext_device & temp_sensor){
			ReadExtTemp(&temp_out);
			str_len=strlen((char*)log_buffer);
			sprintf((char*)log_buffer+str_len,"Tout=%d,Humidity=%d,",temp_out,humidity);
		}

	}
	if(LogParameters & LogWaterlevel){
		read_water_level();
		str_len=strlen((char*)log_buffer);
		sprintf((char*)log_buffer+str_len,"Wlvl=%d,",water_level);
		
	}
	if(LogParameters & LogRainlevel){
		temp_int=rain_level;
		temp_flt=(uint16_t)((rain_level-temp_int)*100);
		str_len=strlen((char*)log_buffer);
		sprintf((char*)log_buffer+str_len,"Rn=%d.%d,",temp_int,temp_flt);
		
//		if(rain_level>(last_rain_level+delta_rain_level)){
//			strcat((char*)log_buffer,"Flood,");
//			SendSMS((char*)ServerPhone,(char*)log_buffer);				
//		}
//		
//		last_rain_level=rain_level;
		
	}
	
	if(flag.test_mode==1){
		strcat((char*)log_buffer,"TestMode,");
	}
	
	strcat((char*)log_buffer,"\r\n");
	
	//Save to memory
	sprintf((char*)buf,"%04d_%02d.txt",clock.year,clock.month);
	
	enter_critical_section();
	
	f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
	
	res=f_open(&File[0],(char*)buf,FA_WRITE | FA_OPEN_ALWAYS);

	if(res==FR_OK){
					
		f_lseek(&File[0],f_size(&File[0]));
		f_write(&File[0],log_buffer,strlen((char*)log_buffer),&n);
		delayms(50);
		f_close(&File[0]);
		delayms(50);

	}
	else{
		sprintf(SMS_text,"SD Card Fail:%d",res);
		SendSMS((char*)ServerPhone,SMS_text);
		delayms(10000);
		HAL_NVIC_SystemReset();
	}
        
//  }
  
	exit_critical_section();
	
  return 1;	
}


unsigned char send_log_data(uint16_t f_year,unsigned char f_month,unsigned char f_day,unsigned char start_hour,unsigned char start_minute,unsigned char end_hour,unsigned char end_minute)
{
  
  static Rtc temp_clock;  
  static unsigned long end_adr;
  unsigned char read_buf[20];
  UINT n=0;
  unsigned char i=0,j=0,k=0,eof=0;
  
  data=fc301d_send_buffer+(base_address[0]*2)+5;
  
  fc301d_send_buffer[0]=fc_start;
  fc301d_send_buffer[1]=6+(base_address[0]*2);
  fc301d_send_buffer[2]=time_stamp.cmd;
  
  for(i=0;i<=(base_address[0]*2)+1;i++) fc301d_send_buffer[i+3]=base_address[i];
  i=0;
  j=76-fc301d_send_buffer[1];
  
  
  if(ext_device & sd_card){
    
    sprintf(buf,(char const*)"%d/%d/bin/%d.bin",f_year,f_month,f_day);
    mmc_pwr_on;
    delayms(50);
    
    f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
    
    if(f_open(&File[0],buf,FA_READ)==FR_OK){
      
      while(eof==0){
        i=0;
        fc301d_send_buffer[1]=6+(base_address[0]*2);
        tgl_wdt;
        do{
          tgl_wdt;
          f_read(&File[0],read_buf,12,&n);
          if(n==12){
            
            if((((uint16_t)read_buf[0]*60+read_buf[1])>=((uint16_t)start_hour*60+start_minute) ) && (((uint16_t)read_buf[0]*60+read_buf[1])<=((uint16_t)end_hour*60+end_minute))){
              
              k=0;
              while(k<12){
                *(data+i+k)=*(read_buf+k);
                k++;
                
              }
              i+=12; 
              if(i<j)fc301d_send_buffer[1]+=12;
            }
          }
          else{eof=1; break;}
          
        }while(i<j);
        
        
        if(i>0)	{
          
          resend=0;
          flag.ack=0;
          flag.aburt=0;
          send_count=0;        
          while(flag.ack==0 && resend<send_time && flag.aburt==0){
            tgl_wdt;
            if(send_count>(fc_delay+(fc301d_send_buffer[1]/30))){
              fc301d_send(fc301d_send_buffer)	;
              send_count=0;
              resend++;
              display();
            }
          }
        }
        
      }
      f_close(&File[0]);
      
    }
    
    fc301d_send_buffer[1]=6+(base_address[0]*2);
    fc301d_send_buffer[2] = end_pack.cmd;
    flag.ack=0;
    flag.aburt=0;
    resend=0;
    send_count=0;
    while(flag.ack==0 && resend<send_time && flag.aburt==0){
      tgl_wdt;
      if(send_count>fc_delay){
        fc301d_send(fc301d_send_buffer)	;
        send_count=0;
        resend++;
        display();
      }
    }	
    
  }
    	  
  resend=0;
  return 0;    
  
}


void check_door_state(){
	
	if((door_sense && flag.door==0)){delayms(200);
  if((door_sense && flag.door==0)){ delayms(200);
  if((door_sense && flag.door==0)){ delayms(200);
  if((door_sense && flag.door==0)){
		flag.door=1;
		lcd_pwr_on;
    flag.LcdOn=1;
    delayms(20);
    LCDinit();
    delayms(20);
		LCDclr();
		if(flag.alarm_disable==1){
			valid_door_flag=1;
			flag.main_menu=1;
			flag.alarm_disable=0;
			return;
		}
		LCDputs("Invalid User !");
		if(flag.rob==0){
			amp_pwr(1);
			alarm_count=0;
			valid_door_flag=2;
			flag.rob=1;
		}
		return;
		
	}
	}
	}
	}
	
	if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){
		flag.door=0;
		lcd_pwr_off;
    flag.LcdOn=0;
		
	}
	}
	}
	}
	
	if(alarm_count>30 && flag.rob==1){
		amp_pwr(0);
				
		
	}
		
	
/*	
  unsigned char *data,i;
  static const char str_f1[]="INPUT PASSWORD:\n     [     ]    ";
  static const char str_f2[]="PASSWORD OK";
  static const char str_f3[]="WRONG PASSWORD!\n     [     ]    ";
  static const char str_f4[]="INVALID USER\n     [     ]    ";
  
  
  data=fc301d_send_buffer+(base_address[0]*2)+5;
  
  fc301d_send_buffer[0]=fc_start;
  fc301d_send_buffer[1]=6+(base_address[0]*2);
  fc301d_send_buffer[1]+=send_data.length;
  fc301d_send_buffer[2]=send_data.cmd;
  
  for(i=0;i<=(base_address[0]*2)+1;i++) fc301d_send_buffer[i+3]=base_address[i];
  
  
  if((door_sense && flag.door==0)){ delayms(100);
  if((door_sense && flag.door==0)){ delayms(200);
  if((door_sense && flag.door==0)){ delayms(100);
  if((door_sense && flag.door==0)){ delayms(200);
  if((door_sense && flag.door==0)){ delayms(100);
  if((door_sense && flag.door==0)){ delayms(200);
  if((door_sense && flag.door==0)){ delayms(100);
  if((door_sense && flag.door==0)){ delayms(200);
  if((door_sense && flag.door==0)){ delayms(100);
  if((door_sense && flag.door==0)){
        
    flag.door=1;
    flag.open_door=1;
    
    lcd_pwr_on;
    flag.LcdOn=1;
    delayms(20);
    LCDinit();
    delayms(20);
    
		
    if(read_exteeprom_byte(IrIsLearned_ee_adr)==0xFF){
      if(IR_Learning()==1)write_exteeprom_byte(0,IrIsLearned_ee_adr);
      else{
        static const char str_f5[]="LEARN FAIL !";
        LCDclr();
        LCDputsf(str_f5);
        delayms(2000);
      }
    }  
    else{
      read_exteeprom_ary((unsigned char*)button,68,button_ir_ee_adr);    
      
    }
    flag.ir_learn=0;
    
    
    LCDclr();    
    LCDputsf(str_f1);
    flag.input_init_pass=1;
    time_out_pass=0;
				
		LCDcursorOn();
		LCDGotoXY(6,1);
		strcpy(input_number_str,"00000");
		LCDputs(input_number_str);
		LCDGotoXY(6,1);
		number_limit=5;
		number_limit_index=0;
    while(flag.input_init_pass==1) delayms(1);
    
    
    if((password==input_pass) || (input_pass==14702)){
      
      if(input_pass==14702) password=input_pass;
      
      input_pass=0;
      
      valid_door_flag=1;
      write_exteeprom_byte(valid_door_flag,door_state_ee_adr);
      save_door_state=valid_door_flag;
      LCDclr();
      LCDputsf(str_f2);
      
      voice_pwr_on;
      delayms(50);
     
      play_stop();
      voice_pwr_off;
      amp_pwr(0);
      for(i=0;i<20;i++) play_buffer[i].msg=0;
      next_msg=0;
      
      flag.rob=0;
      flag.open_door=0;
      flag.disp=0;
      display_count=5;
      flag.ir_flag=0;
      
    }
    
    else{
      
      LCDclr();
      LCDputsf(str_f3);
			
      LCDcursorOn();
			LCDGotoXY(6,1);
			strcpy(input_number_str,"00000");
			LCDputs(input_number_str);
			LCDGotoXY(6,1);
			number_limit=5;
			number_limit_index=0;
			
      flag.input_init_pass=1;
			
      time_out_pass=0;
      while(flag.input_init_pass==1) delayms(1);
      
      if((password==input_pass) || (input_pass==14702)){
        
        if(input_pass==14702) password=input_pass;
        input_pass=0;
        LCDclr();
        LCDputsf(str_f2);
        
        flag.rob=0;
        
        valid_door_flag=1;
        write_exteeprom_byte(valid_door_flag,door_state_ee_adr);
        save_door_state=valid_door_flag;
        
        voice_pwr_on;
        delayms(50);
        play_stop();
        
        voice_pwr_off;
        amp_pwr(0);
        for(i=0;i<20;i++) play_buffer[i].msg=0;
        next_msg=0;
        
        flag.open_door=0;
        
        flag.disp=0;
        display_count=5;
        flag.ir_flag=0;
        
      }
      
      else{
        flag.rob=1;
        
        ir_cou=0;
        LCDclr();
        LCDputsf(str_f4);
        LCDcursorOn();
				LCDGotoXY(6,1);
				strcpy(input_number_str,"00000");
				LCDputs(input_number_str);
				LCDGotoXY(6,1);
				number_limit=5;
				number_limit_index=0;
				valid_door_flag=2;
        write_exteeprom_byte(valid_door_flag,door_state_ee_adr);
				flag.input_init_pass=1;
      }
    }
    
    if(flag.rob==1){
      flag.rob=0;
      flag.input_init_pass=1;      
      if(save_door_state!=valid_door_flag){
        play_buffer[0].num=rob_alarm;
        play_buffer[0].device=0;
        next_msg=0;
        play_count=rob_alarm_cou;
        amp_pwr(1);
        delayms(50);
        voice_pwr_on;
        while(play_count--){
          play_start(play_buffer[next_msg].num);
        }
        
      }
      
      
    }
    
    read_sensours(data);
		if(ext_device & RadioModem){
			flag.comm=1;
			rf_pwr_on;
			flag.refresh=1;
			send_count=0;
			flag.ack=0;
			flag.aburt=0;
			resend=1;
			display();
			delayms(200);
			if(flag.fc_net_busy==0)  
				fc301d_send(fc301d_send_buffer);
		}
		else if(ext_device & gsm){
			ReadSensorToStr(SMS_text);
			SimChargeCat(SMS_text);
      SendSMS(ServerPhone,SMS_text);
			
		}
    
    
    
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }          
  
  if((!door_sense) && flag.door==1){ delayms(200); 
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){ delayms(200);
  if((!door_sense) && flag.door==1){
    
    valid_door_flag=0;
    write_exteeprom_byte(valid_door_flag,door_state_ee_adr); 
    save_door_state=valid_door_flag;
    
    
    lcd_pwr_off;
    flag.LcdOn=0;
    flag.door=0; flag.open_door=0;
        
    read_sensours(data);
		if(ext_device & RadioModem){
			flag.comm=1;
			rf_pwr_on;
			flag.refresh=1;
			send_count=0;
			flag.ack=0;
			flag.aburt=0;
			resend=1;
			display();
			delayms(200);
			if(flag.fc_net_busy==0)  
				fc301d_send(fc301d_send_buffer);
		}
		else if(ext_device & gsm){
			ReadSensorToStr(SMS_text);
      SendSMS(ServerPhone,SMS_text);
			
		}
    
    
    
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }            
  
  if(flag.open_door==1 && ((password==input_pass) || (input_pass==14702))){
    
    if(input_pass==14702) password=input_pass;
    flag.comm=1;
    flag.wakeup=1;
    wakeup_cou=0;
    
    LCDclr();
    LCDputsf(str_f2);
    
    flag.disp=0;
    display_count=5; 
    
    valid_door_flag=1;  
    write_exteeprom_byte(valid_door_flag,door_state_ee_adr);
    save_door_state=valid_door_flag;
    flag.rob=0; 
    flag.input_init_pass=0;
    flag.open_door=0; 
    input_pass=0;
    voice_pwr_on;
    delayms(50);
    
    play_stop();
    voice_pwr_off;
    amp_pwr(0);
    for(i=0;i<20;i++) play_buffer[i].msg=0;
    next_msg=0;
    
    
    read_sensours(data);
		if(ext_device & RadioModem){
			flag.comm=1;
			rf_pwr_on;
			flag.refresh=1;
			send_count=0;
			flag.ack=0;
			flag.aburt=0;
			resend=1;
			display();
			delayms(200);
			if(flag.fc_net_busy==0)  
				fc301d_send(fc301d_send_buffer);
		}
		else if(ext_device & gsm){
			ReadSensorToStr(SMS_text);
      SendSMS(ServerPhone,SMS_text);
			
		}
    
  }	
	
	*/
	
	
}


void display(){
	float bat_v=0,sol_v=0;
	if(valid_door_flag==1 && flag.LcdOn==1 && flag.menu_setting==0){
		LCDcursorOFF();
		if(flag.main_menu==1){
			flag.main_menu=0;
			sprintf(buf,"%s\n%02d:%02d O %02d/%02d/%02d",title_str,clock.hour,clock.min,clock.year%100,clock.month,clock.day);
			LCDclr();
			LCDputs(buf);		
			return;			
		}
		
		if(ext_device&water_sensor){
			
			if(cur_water_level_display!=water_level){
				cur_water_level_display=water_level;
//				sprintf(buf,"    GSM RTU\nWATER LEVEL:%dcm",water_level);
//				LCDclr();
				sprintf(buf,"WLVL:%dcm    ",water_level);
				LCDGotoXY(0,2);
				LCDputs(buf);
				
			}
		}					   
		if(ext_device&rain_sensor){
		 
			if(cur_rain_level!= rain_level){
				sprintf(buf,"%s\nRAIN LEVEL:%5.3fmm",title_str,rain_level);
				cur_rain_level=rain_level;
				LCDclr();
				LCDputs(buf);
			}
		}		
		
		if(clock.min != cur_clock.min){
			cur_clock=clock;
			sprintf(buf,"%s\n%02d:%02d O %02d/%02d/%02d",title_str,clock.hour,clock.min,clock.year%100,clock.month,clock.day);
			LCDclr();
			LCDputs(buf);
		}
				
	}
	
	else if(valid_door_flag==2 && flag.LcdOn==1 && flag.ir_flag==0){
		
		if(flag.main_menu==1){
			flag.main_menu=0;
			
			LCDclr();
			LCDputs("Invalid User !");		
			return;			
		}
	}
 
}	


unsigned char analyse(){
  
  unsigned char i;  
  
  rec_data=fc301d_rec_buffer+(base_address[0]*2)+5;
  
  data=fc301d_send_buffer+(base_address[0]*2)+5;  
  
  if(fc301d_rec_buffer[2]==play_alarm.cmd){
    
    if(flag.ply_alarm==1) return 0;
    else{
      
      for(i=0;i<rec_data[1];i++){
        play_buffer[i].num=rec_data[i+2] & 0x7F;
        play_buffer[i].device=rec_data[i+2]>>7;
        
      }
      next_msg=0;
      play_count = rec_data[0];
      flag.ply_alarm=1;
      voice_pwr_on;
      delayms(100);
      amp_pwr(1);
      
            
    }
    return 0;
    
  }
  
  if(fc301d_rec_buffer[2]==stop_alarm.cmd){
    play_stop();
    
    voice_pwr_off;
    amp_pwr(0);
    for(i=0;i<20;i++) play_buffer[i].msg=0;
    next_msg=0;
    flag.ply_alarm=0;
    
  }
  
  
  else{	
    
    
    if(fc301d_rec_buffer[2]==acknlg.cmd){flag.ack=TRUE; return 0;}
    
    if(fc301d_rec_buffer[2]==aburt_process.cmd){flag.aburt=1;return 0;}
    
    
    
    if(flag.fc301d_reply_cmd==1){send_count=0; return 0;}
    
    flag.fc301d_reply_cmd=1;
    
    if(fc301d_rec_buffer[2]==hozur_taki.cmd){flag.read_state=1;}
    
    else if(fc301d_rec_buffer[2]==read_mem.cmd){flag.read_log=1;}	 
    
    else if(fc301d_rec_buffer[2]==rtc_config.cmd){flag.config_rtc=1;}
    
    else if(fc301d_rec_buffer[2]==read_rtc.cmd){flag.rtc_read=1;}		 
    
    else if(fc301d_rec_buffer[2]==log_time_config.cmd){flag.config_log_time=1;}	 
    
    else if(fc301d_rec_buffer[2]==read_log_time.cmd){flag.log_time_read=1;}	 
    
    else if(fc301d_rec_buffer[2]==power_config.cmd){flag.config_pwr=1;}	 
    
    else if(fc301d_rec_buffer[2]==read_power_config.cmd){flag.pwr_config_read=1;}
    
    else if(fc301d_rec_buffer[2]==fc_time_config.cmd){flag.config_fc_cycle=1;}
    
    else if(fc301d_rec_buffer[2]==read_fc_time.cmd){flag.fc_cycle_read=1;}
    
    else if(fc301d_rec_buffer[2]==send_time_config.cmd){flag.config_send_time=1;}		
    
    else if(fc301d_rec_buffer[2]==read_send_time.cmd){flag.send_time_read=1;}
    
    
  }		
  return 0;
  
}

void fc301d_reply_command(){
  
  
  rec_data=fc301d_rec_buffer+(base_address[0]*2)+5;
  
  data=fc301d_send_buffer+(base_address[0]*2)+5;
  
  fc301d_send_buffer[0]=fc_start;
  fc301d_send_buffer[1]=6+(base_address[0]*2);
  
  for(i=0;i<=(base_address[0]*2)+1;i++) fc301d_send_buffer[i+3]=base_address[i];	
  
  // ------------------hozur taki----------------
  if(flag.read_state==1){
    
    
    fc301d_send_buffer[1]+=send_data.length;
    fc301d_send_buffer[2]=send_data.cmd;
    
    read_sensours(data);
    
    flag.ack=0;
    resend=0;
    flag.comm=1;
    rf_pwr_on;
    flag.wakeup=1;
    wakeup_cou=0;
    send_count=0;
    flag.aburt=0;
    flag.read_state=0;
    while(flag.ack==0 && flag.aburt==0 && resend<send_time){
      tgl_wdt;
      if(send_count>fc_delay){ 
        fc301d_send(fc301d_send_buffer);
        send_count=0;
        resend++;
        display();
        
      }
      
    }
    
    resend=0;
    
  }
  //-----------------------send data in memory---------------
  else if(flag.read_log==1){
    
    flag.comm=1;
    rf_pwr_on;
    flag.wakeup=1;
    wakeup_cou=0;
    
    send_log_data(((uint16_t)rec_data[0]*100)+(uint16_t)rec_data[1],rec_data[2],rec_data[3],rec_data[4],rec_data[5],rec_data[6],rec_data[7]);
    flag.read_log=0;
  }
  
    
  
  else if(flag.config_rtc==1 || flag.rtc_read==1){
    
    if(flag.config_rtc==1){
      clock.year=rec_data[0]*100+rec_data[1];
      
      clock.month=rec_data[2];
      clock.day=rec_data[3];
      clock.hour=rec_data[4];
      clock.min=rec_data[5];
      clock.sec=0;
      flag.rtcset=1;
      //rtc_settime_shamsi(&clock);
      delayms(10);
      next_log_time=(clock.hour*60)+clock.min+log_time;
      if(next_log_time>1440) {next_log_time=next_log_time%1440; flag.log_disable=1;}
      
    }				
    //rtc_gettime_shamsi(&clock);
    
    
    fc301d_send_buffer[1]+=send_rtc.length;
    fc301d_send_buffer[2]=send_rtc.cmd;
    data[0]=(clock.year-clock.year%100)/100;
    data[1]=clock.year%100;
    data[2]=clock.month;
    data[3]=clock.day;
    data[4]=clock.hour;
    data[5]=clock.min;
    
    
    flag.comm=1;
    rf_pwr_on;
    flag.wakeup=1;
    wakeup_cou=0;
    flag.ack=0;
    resend=0;
    delayms(300);
    flag.aburt=0; 
    
    send_count=0;
    flag.config_rtc=0;
    flag.rtc_read=0;
    
    while(flag.ack==0 && flag.aburt==0 && resend<send_time){
      tgl_wdt;
      if(send_count>fc_delay){ 
        fc301d_send(fc301d_send_buffer);
        send_count=0;
        resend++;
        display();
        
      }
      
    }
    
    
  }
  
  
  else if(flag.config_log_time==1 || flag.log_time_read==1){
    
    if(flag.config_log_time){
      write_exteeprom_byte(rec_data[0],log_time_ee_adr);
      write_exteeprom_byte(rec_data[1],log_time_ee_adr+1);
      
      
      log_time=read_exteeprom_word(log_time_ee_adr);
      next_log_time=(clock.hour*60)+clock.min+log_time;
      if(next_log_time>1440) {next_log_time=next_log_time%1440; flag.log_disable=1;}
    }
    
    fc301d_send_buffer[1]+=send_log_time.length;
    fc301d_send_buffer[2]=send_log_time.cmd;
    data[0]=read_exteeprom_byte(log_time_ee_adr);
    data[1]=read_exteeprom_byte(log_time_ee_adr+1);
    
    
    flag.comm=1;
    rf_pwr_on;
    flag.wakeup=1;
    wakeup_cou=0;
    flag.ack=0;
    resend=0;
    flag.aburt=0; 
    send_count=0;
    
    
    flag.config_log_time=0;
    flag.log_time_read=0;
    
    while(flag.ack==0 && flag.aburt==0 && resend<send_time){
      tgl_wdt;
      if(send_count>fc_delay){ 
        fc301d_send(fc301d_send_buffer);
        send_count=0;
        resend++;
        display();
        
      }
      
    }
    
    resend=0;
  }
  
  else if(flag.config_fc_cycle==1 || flag.fc_cycle_read==1){
    
    if(flag.config_fc_cycle==1){
      write_exteeprom_byte( (rec_data[0] | rec_data[1]<<4) ,fc_time_ee_adr);
      fc_on_time= rec_data[0];
      fc_off_time= rec_data[1];
      rf_pwr_count=0;
    }				
    
    fc301d_send_buffer[1]+=send_fc_time.length;
    fc301d_send_buffer[2]=send_fc_time.cmd;
    
    data[0]=read_exteeprom_byte(fc_time_ee_adr) & 0x0F;
    data[1]=read_exteeprom_byte(fc_time_ee_adr)>>4;
    
    
    flag.comm=1;
    rf_pwr_on;
    flag.wakeup=1;
    wakeup_cou=0;
    flag.ack=0;
    resend=0;
    flag.aburt=0; 
    send_count=0;
    flag.config_fc_cycle=0;
    flag.fc_cycle_read=0;
    
    while(flag.ack==0 && flag.aburt==0 && resend<send_time){
      tgl_wdt;
      if(send_count>fc_delay){ 
        fc301d_send(fc301d_send_buffer);
        send_count=0;
        resend++;
        display();
        
      }
      
    }
    
    resend=0;
    
    
  }
  
  else if(flag.config_send_time==1 || flag.send_time_read==1){
    
    if(flag.config_send_time==1){
      write_exteeprom_byte(rec_data[0],send_time_ee_adr);
      
      send_time=rec_data[0];
    }
    
    fc301d_send_buffer[1]+=transmit_send_time.length;
    fc301d_send_buffer[2]=transmit_send_time.cmd;
    
    data[0]=read_exteeprom_byte(send_time_ee_adr);
    
    
    flag.comm=1;
    rf_pwr_on;
    flag.wakeup=1;
    wakeup_cou=0;
    flag.ack=0;
    resend=0;
    flag.aburt=0; 
    send_count=0;
    flag.config_send_time=0;
    flag.send_time_read=0;
    
    while(flag.ack==0 && flag.aburt==0 && resend<send_time){
      tgl_wdt;
      if(send_count>fc_delay){ 
        fc301d_send(fc301d_send_buffer);
        send_count=0;
        resend++;
        display();
        
      }
      
    }
    
    resend=0;
  }
  
  
  
}

void correct_rain_level(volatile float time,volatile float*level){
  
  if(time<=0.7) *level+=0.143;
  
  else if(time<=0.8 && time>0.7) *level+=0.1389;
  else if(time<=0.9 && time>0.8) *level+=0.135;
  else if(time<=1.1 && time>0.9) *level+=0.1316;
  else if(time<=1.3 && time>1.1) *level+=0.128;
  else if(time<=1.5 && time>1.3) *level+=0.125;
  else if(time<=1.8 && time>1.5) *level+=0.122;
  else if(time<=2.1 && time>1.8) *level+=0.119;
  else if(time<=2.7 && time>2.1) *level+=0.1163;
  else if(time<=3.1 && time>2.7) *level+=0.1136;
  else if(time<=4 && time>3.1)   *level+=0.111;
  else if(time<=6.5 && time>4.0) *level+=0.1087;
  else if(time<=7.5 && time>6.5) *level+=0.1075;
  else if(time<=8.5 && time>7.5) *level+=0.1064;
  else if(time<=10 && time>8.5)  *level+=0.1042;
  else if(time<=12 && time>10)   *level+=0.102;
  
  else if(time>12) *level+=0.1;
  
  
}

uint8_t address_process(uint8_t *frame){
  
  unsigned char last_adr=0,layer_num=0,i=0,j=0,set_base_adr=0;
  uint16_t *address,temp_adr=0;
  
  address=(uint16_t*)(frame+4);
  last_adr=frame[3]-1;
  layer_num=(frame[3]*2)+4;
  
  if(address[0]==station_address){
    
    if(address[1]==center_adr && frame[3]==(frame[layer_num]+1)){
      i=0;
      j=last_adr;
      frame[layer_num]=1;
      while(i<j){
        temp_adr=address[i];
        address[i]=address[j];
        address[j]=temp_adr;
        i++;
        j--;
      }
      for(i=0;i<=last_adr;i++)
        if(*(address+i)!=*((uint16_t*)(base_address+1)+i)){set_base_adr=1;break;}
      
      if(set_base_adr==1){
        for(i=3;i<=layer_num;i++)
          base_address[i-3]=frame[i];
        write_exteeprom_byte(0,50);
        write_exteeprom_ary(base_address,30,base_ee_adr);
        fc_delay = base_address[0];
        
      }
      
      return 1;
    }
    else {
      i=0;
      while (i<last_adr){
        address[i]=address[i+1];
        i++;
      }
      address[last_adr]=station_address;
      frame[layer_num]+=1;
      fc301d_send(frame);
      
      return 0;
      
    }
  }
  return 0;
}




void event_process(){
  
  unsigned char i,*data,*gsm_data;
  
  data=fc301d_send_buffer+(base_address[0]*2)+5;
  
  gsm_data=data-2;
  strcpy(SMS_text,(char const*)"MARVCOM+");
  
  fc301d_send_buffer[0]=fc_start;
  fc301d_send_buffer[1]=6+(base_address[0]*2);
  
  for(i=0;i<=(base_address[0]*2)+1;i++) fc301d_send_buffer[i+3]=base_address[i];
  
    
  //----------------------if door open -------------------
	check_door_state();
	
	//--------------log user check -------------------------
	if(flag.UserCheck==1){
		flag.rob=0;
		amp_pwr(0);
		valid_door_flag=1;
		flag.main_menu=1;
		
		
		flag.UserCheck=0;
		sprintf((char*)SMS_text,"User check at %d/%d/%d %d:%d\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min);
		
		f_mount(&SDFatFs,(TCHAR const*)SDPath,1);
		res=f_open(&File[0],"user.txt",FA_WRITE | FA_OPEN_ALWAYS);
		if(res==FR_OK){
			f_lseek(&File[0],f_size(&File[0]));
			f_write(&File[0],SMS_text,strlen((char*)SMS_text),&FatBytesWritten);
			delayms(50);
			f_close(&File[0]);
			delayms(50);

		}
		else{
			LCDclr();
			LCDputs("log fail...!");
			delayms(1000);
		}
		
		if(ext_device & gsm){
			
			if(SendSMS(ServerPhone,SMS_text)==1){
				delayms(1000);
				LCDclr();
				LCDputs("USER SMS SENT OK!");
				delayms(2000);
			}
		}
	}
	
  //----------------------if rain detected --------------------
  if(flag.detect_rain==1 && flag.start_rain_event==0){
    
		flag.start_rain_event=1;
    flag.detect_rain=0;
    rain_cou=0;
		
		
		sprintf(buf,"%04d/%02d/%02d,%02d:%02d:%02d,Rn=%4.2f,RainStart,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);				
		log_event(buf);
		
		if(ext_device & gsm){
			sprintf(SMS_text,"%04d/%02d/%02d,\r\n%02d:%02d:%02d,\r\nRn=%4.2f,\r\nRainStart,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);		
			SendSMS(ServerPhone,SMS_text);
			delayms(2000);
			read_exteeprom_ary(AdminMobile,29,AdminMobile1_ee_adr); 
			SendSMS(AdminMobile,SMS_text);
			delayms(2000);
			read_exteeprom_ary(AdminMobile,29,AdminMobile2_ee_adr); 
			SendSMS(AdminMobile,SMS_text);
			delayms(2000);
		}
			
   }
	
	if(flag.stop_rain==1){ 
		flag.stop_rain=0;
		
		sprintf(buf,"%04d/%02d/%02d,%02d:%02d:%02d,Rn=%4.2f,RainStop,\r\n\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);				
		log_event(buf);
		
		if(ext_device & gsm){
			sprintf(SMS_text,"%04d/%02d/%02d,\r\n%02d:%02d:%02d,\r\nRn=%4.2f,\r\nRainStop,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);		
			SendSMS(ServerPhone,SMS_text);
			delayms(2000);
			read_exteeprom_ary(AdminMobile,29,AdminMobile1_ee_adr); 
			SendSMS(AdminMobile,SMS_text);
			delayms(2000);
			read_exteeprom_ary(AdminMobile,29,AdminMobile2_ee_adr); 
			SendSMS(AdminMobile,SMS_text);
			delayms(2000);
			
		}
	}
	
	if(flag.detect_rain==1){
		flag.detect_rain=0;
		sprintf(buf,"%04d/%02d/%02d,%02d:%02d:%02d,Rn=%4.2f,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);				
		log_event(buf);
		
	}
	
	if(flag.min_rain_check==1){
		flag.min_rain_check=0;
		rain_sum=0;
		for(i=0;i<15;i++){
			rain_sum+=rain_register[i];
		}
		if(rain_sum > delta_rain_level){
			for(i=0;i<15;i++)
					rain_register[i]=0;
				
			rain_register_index=0;
			
			sprintf(buf,"%04d/%02d/%02d,%02d:%02d:%02d,Rn=%4.2f,RainAlarm,\r\n\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);				
			log_event(buf);
			
			if(ext_device & gsm){
				sprintf(SMS_text,"%04d/%02d/%02d,\r\n%02d:%02d:%02d,\r\nRn=%4.2f,\r\nRainAlarm,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);		
				SendSMS(ServerPhone,SMS_text);
				delayms(2000);
				read_exteeprom_ary(AdminMobile,29,AdminMobile1_ee_adr); 
				SendSMS(AdminMobile,SMS_text);
				delayms(2000);
				read_exteeprom_ary(AdminMobile,29,AdminMobile2_ee_adr); 
				SendSMS(AdminMobile,SMS_text);
				delayms(2000);
				
			}
						
		}
	}

		
}
  

//******************************************************************
float read_microsonic_level(){
  
  float vo=0,current=0,level=0;
  unsigned char n=0;
  long time,loopcounter;

  WaterSensorTime=0;
  
  if(flag.WaterSensorOn==0){
    flag.WaterSensorOn=1;
    
  }
  
  WaterSensorTime=0;
  adConfig.Channel = ADC_CHANNEL_3;
  HAL_ADC_ConfigChannel(&hadc1, &adConfig);
  
  
  delayms(30);
  for(n=0;n<100;n++){ 
    HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,10);
    vo+=(float)HAL_ADC_GetValue(&hadc1)/100;
  }
  
  vo=(vo*2.5/4096)-0.06;
  current=vo/300;
  Imin=0.004369;
  Imax=0.02057;
  if(vo<=2.5 && vo>=0.3){
    level= (max_water_level-min_water_level);
    level/=(Imax-Imin);
    level*=(current-Imin);
    level+=min_water_level;
    water_level=SensorLevel-(int)level;
    
    
  }
	return current;

  
}
/*******************************************************************/

uint32_t strtolong(char *str){
  uint8_t sign=0;
  uint32_t value=0;
	
  if((*str)=='-') {
		sign=1;
		str++;
	}
	
  while (*str != 0){
    if((*str)>='0' && (*str)<='9')
			value = (value*10) + (*str)-0x30 ;
    else 
			return 0;    
		
    str++;
  }
	if(sign==1)
		value=0-value;
	
  return value;
}

//******************************************************************
float read_water_level(){
  char *point,**endptr;
  
  if (WaterSensorType==microsonic){
    read_microsonic_level();
  }
  else if(WaterSensorType==thalimedes){
    flush_buf(thali_buf,20);
    strcpy(buf,"0M!");
    thali_sendcmd(buf);
    delayms(2000);
    strcpy(buf,"0D0!");
    thali_sendcmd(buf);
    
    if(thali_readresp()==0){
      point=CharSearch(thali_buf,CRchar,19);
      if(point!=NULL){
        *point=0;
        water_level=(unsigned int)strtolong(thali_buf+2);
        if(thali_buf[1]=='-'){
          water_level=0-water_level;          
        }
        
      } 
    }
    
  }
  
}

////////////////////////////////////////////////////////////
void avg_buf(uint16_t *bufd,unsigned char *bufs,int16_t buf_size,int16_t count,char div){
  int16_t i;
  if(div==0){
    for(i=0;i<buf_size;i++){
      tgl_wdt;
      bufd[i]+=bufs[i];
    }
  }
  else{
    for(i=0;i<buf_size;i++){
      tgl_wdt;
      bufd[i]/=count;
    }
    for(i=0;i<buf_size;i++){
      tgl_wdt;
      bufs[i]=(unsigned char)bufd[i];
    }
  }
}

///////////////////////////////////////////////////////////
void read_bmp180sensor(){
  uint8_t OSS=0;
  uint8_t temp_buf[20];  
  flag.bmp180_timeout=0;
  
  //read bmp180------ 
  if(HAL_I2C_Mem_Read(&hi2c1,0xEE,0xAA,I2C_MEMADD_SIZE_8BIT,(uint8_t*)temp_buf,20,100)==HAL_OK){
    AC1=(temp_buf[0]<<8)|temp_buf[1];
    AC2=(temp_buf[2]<<8)|temp_buf[3];
    AC3=(temp_buf[4]<<8)|temp_buf[5];
    AC4=(temp_buf[6]<<8)|temp_buf[7];
    AC5=(temp_buf[8]<<8)|temp_buf[9];
    B1=(temp_buf[10]<<8)|temp_buf[11];
    B2=(temp_buf[12]<<8)|temp_buf[13];
    MB=(temp_buf[14]<<8)|temp_buf[15];
    MC=(temp_buf[16]<<8)|temp_buf[17];
    MD=(temp_buf[18]<<8)|temp_buf[19];
    
    //READ TEMP --------------------------
    temp_buf[0]=0x2E;
    HAL_I2C_Mem_Write(&hi2c1,0xEE,0xF4,I2C_MEMADD_SIZE_8BIT,(uint8_t*)temp_buf,1,100);
    HAL_I2C_Mem_Read(&hi2c1,0xEE,0xF6,I2C_MEMADD_SIZE_8BIT,(uint8_t*)temp_buf,2,100);
    UT=(temp_buf[0]<<8)|temp_buf[1];
    //READ UNCOMP P VALUE-------
    temp_buf[0]=0x34;
    HAL_I2C_Mem_Write(&hi2c1,0xEE,0xF4,I2C_MEMADD_SIZE_8BIT,(uint8_t*)temp_buf,1,100);
    HAL_I2C_Mem_Read(&hi2c1,0xEE,0xF6,I2C_MEMADD_SIZE_8BIT,(uint8_t*)temp_buf,3,100);
    UP=(temp_buf[0]<<16)|(temp_buf[1]<<8)|temp_buf[2];
    UP=UP>>(8-OSS);
    
    //------CALCULATE TEMP------
    X1=((UT-(long)AC6)*(long)AC5)/32768;
    X2=((long)MC*2048)/(X1+(long)MD);
    B5=X1+X2;
    T=(B5+8)/16;
    //--------CALCULATE PRESSURE---------
    B6=B5-4000;
    X1=((long)B2*(B6*B6/4096))/2048;
    X2=(long)AC2*B6/2048;
    X3=X1+X2;
    B3=((((long)AC1*4+X3)<<OSS)+2)/4;
    X1=((long)AC3*B6)/8192;
    X2=((long)B1*(B6*B6/4096))/65536;
    X3=(X1+X2+2)/4;
    B4=((long)AC4*(unsigned long)(X3+32768))/32768;
    B7=((unsigned long)UP-B3)*(50000>>OSS);
    if(B7<0x80000000)P=((unsigned long)B7*2)/B4;
    else P=(B7/B4)*2;
    X1=(P/256)*(P/256);
    X1=(X1*3038)/65536;
    X2=(7357*P)/65536;
    X2=0-X2;
    P=P+(X1+X2+3791)/16;
    AirPressure=(unsigned long)(P*AirAlpha+(AirBeta*100));
    
  }
  else
    flag.bmp180_timeout=1; 
  
  
}

 

/********************* sound player rutines******************************/
void amp_pwr(char stat){
  if(stat==0)sht_off;
  else sht_on;
}  

/////////////////////////////////////////////////////////////////

void Error_Handle()
{
  if( flag.LcdOn==1){
    LCDclr();
    LCDputs("Error . . . !");
    delayms(1000);
  }
  while(1)
  {
    
    HAL_Delay(40);
  }
}
/***************************tglwdt********************/
void wdt_refresh(){
  
  GPIOE->ODR ^= GPIO_PIN_7;
  HAL_IWDG_Refresh(&hiwdg);

}
//---------------------Find Log Record---------
uint8_t FindLog(uint8_t* header,char* str){
unsigned char *str_point=NULL,*str_point1=NULL;
uint8_t header_copy[20],HeaderLength;
uint32_t file_pointer=0;

strcpy(header_copy,header);
str_point=header_copy;
HeaderLength=strlen(header);	
	
	str_point1=CharSearch(str_point,'/',250);
	if(str_point1==NULL){
		strcat(str,"Incorrect Format\r\n");
		return 0;
	}
	*str_point1=0;
	strcpy(buf,str_point);
	strcat(buf,"_");
	str_point=CharSearch(str_point1+1,'/',250);
	if(str_point==NULL){
		strcat(str,"Incorrect Format\r\n");
		return 0;
	}
	*str_point=0;
	strcat(buf,str_point1+1);
	strcat(buf,".txt");
//	f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
	delayms(100);
	res=f_open(&File[0],(char*)buf,FA_READ);
	if(res!=0){
		sprintf(buf,"Error:%d\r\n",res);
		strcat(str,buf);
		delayms(100);
		f_close(&File[0]);
		delayms(100);
		return 0;
	}
	
	str_point=NULL;
	loop_timeout=0;
	while(loop_timeout<60){
		tgl_wdt;
		if(str_point==NULL){
			f_lseek(&File[0],file_pointer);
			delayms(20);
			f_read(&File[0],MemBuf,1024,&FatBytesRead);
			str_point=MemBuf;
		}
		else
			str_point++;
		
		if(memcmp(str_point,header,HeaderLength)==0){
			str_point1=CharSearch(str_point,LF,((MemBuf+1024)-str_point));
			if(str_point1 != NULL){
				*(str_point1+1)=0;
				strcat(str,str_point);
				delayms(100);
				f_close(&File[0]);
				delayms(100);
				return 1;
			}
//			else{
//				strcat(str,"Record corrupted!\r\n");
//				return 0;
//			}
		}
		
		str_point=CharSearch(str_point,LF,((MemBuf+1024)-str_point));
		
		if(str_point==NULL){
			if(FatBytesRead<1024){
				strcat(str,"Record does not find!\r\n");
				delayms(100);
				f_close(&File[0]);
				delayms(100);
				return 0;
			}
			else
				file_pointer+=512;
			
		}
					
	} 
	delayms(100);
	f_close(&File[0]);
	delayms(100);
	return 0;
}


//********************************************
void log_event(char* log_str){
	
	//Save to memory
	enter_critical_section();
	
	res=f_open(&File[0],"RnLog.txt",FA_WRITE | FA_OPEN_ALWAYS);

	if(res==FR_OK){
					
		f_lseek(&File[0],f_size(&File[0]));
		f_write(&File[0],log_str,strlen(log_str),&FatBytesWritten);
		delayms(50);
		f_close(&File[0]);
		delayms(50);

	}
	else{
		sprintf(SMS_text,"SD Card Fail:%d",res);
		SendSMS((char*)ServerPhone,SMS_text);
		delayms(10000);
		HAL_NVIC_SystemReset();
	}
	
	exit_critical_section();

}
//-------------------------------------------------
void enter_critical_section(){
	HAL_NVIC_DisableIRQ(TIM2_IRQn);
	HAL_NVIC_DisableIRQ(TIM4_IRQn);
	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}

void exit_critical_section(){
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}
//-----------------------------------------------
void SARAMIN(char *str){
	float ad_value=0;
	uint16_t str_len=0,temp_int=0,temp_flt=0;
	
	sprintf(str,"%04d/%02d/%02d %02d:%02d:%02d\r\n%s\r\n%06d\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,station_name,station_address);
  
	if(ext_device & rain_sensor){
		
		str_len=strlen(str);
		sprintf(str+str_len,"TRAIN=%4.1f\r\nNRAIN=%3.1f\r\n",rain_level,minute_rain);
  }	
	
	if(ext_device & water_sensor){
		read_water_level();
		str_len=strlen(str);
		sprintf(str+str_len,"Wlvl=%d\r\n",water_level);
			
	}
	
	//-----read bat voltage-----------
  ad_value=READ_ADC(BatteryVoltageADC,BatteryVoltageADCChannel,BatteryVoltageADCScale);
  str_len=strlen(str);
	
	sprintf(str+str_len,"Bat=%4.2f\r\n",ad_value);
	  
  temp_lm75(&temp_in);
  str_len=strlen(str);
	sprintf(str+str_len,"BOARD Temp:%d'C\r\n",temp_in);

}