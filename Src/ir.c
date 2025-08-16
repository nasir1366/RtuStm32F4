#include "includes.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
#include "usart.h"
#include "ir.h"
#include "global_var.h"
#include "rtc.h"
#include "eeprom_ext.h"
#include "lcdlib.h"
#include "my_delay.h"
#include "temperature.h"
#include "gsm.h"
#include "main_func.h"
#include "ADC.h"

void setup_prog();
void menu_prog();
void select();
void pass_change();
char st_setting();
char add_device();
void WaterSetting();
void time_setting();
void GsmSetting();
void TemperatureSetting();
void ADCcalib_routin();
void MicrosonicSetting();
void RainSetting();
void log_setting();
void loadSettingSD();
void RotarySetting();
// Variable Definiton------------------------------
unsigned char command_ir;
float bat_v,sol_v;
char v_int,v_float;
unsigned char temp_val,temp,UTP_H,UTP_L,LTP_H,LTP_L;

uint16_t prim_password,prim_station_address,max_water_val,min_water_val,prim_water_level;

signed char key,start_key,end_key,prim_T_low,prim_T_hi;

Rtc prim_clock;

volatile float adc_temp;
char ServerPhone_temp[30];

static const char erase[]="               ";
static const char desel[]="Deselect";
static const char sel[]="Saved Ok";

//-------STRUCTURE----------------------
//---FLAGS-----------
#pragma anon_unions
struct {
  
  union{
    uint16_t blk1;
    struct{
			//general--------
      uint16_t setup:1;
      uint16_t setup_main:1;
      uint16_t menu:1;
      uint16_t first_menu:1;
			uint16_t station_sett:1;
      uint16_t st_set:1;
      uint16_t st_set_main:1;
			uint16_t display_adr:1;
      uint16_t display_sensor:1;
			uint16_t invalid_remote:1;
			//password ---------------
      uint16_t pas:1;
      uint16_t chang_pas:1;
      uint16_t change_pass_routin:1;
			uint16_t new_pas:1;
      uint16_t emph:1;
      uint16_t pas_ok:1;
			
    };
  };
  
  union{
    uint16_t blk2;	
    struct{	
			//clock rtc-----------
      uint16_t time_sett_main:1;
      uint16_t time_sett:1;
      uint16_t time_set_enter:1;
      uint16_t t:1;
      uint16_t d:1;
			//temperature------------
      uint16_t temp_setting:1;
      uint16_t temp_set_main:1;
			uint16_t temp_calib:1;
			//adc calibration----------
			uint16_t ADCcalibration:1;
      uint16_t adc_calib_main:1;
			uint16_t adc_calib_firstpage:1;
      uint16_t adc1_calib:1;      
      uint16_t adc2_calib:1;
      uint16_t adc3_calib:1;
    };
  };
  
  union{
    uint16_t blk3;			
    struct{
			//add device
			uint16_t select_sensor:1;
			uint16_t add_external_device:1; 
      uint16_t add_device_main:1;
			// snow motor---------
      uint16_t snow_test:1;
			//station -------------
			uint16_t read_st:1;
      uint16_t read_st_main:1;      
			//water sensor--------
			uint16_t water_set_main:1;
      uint16_t max_water:1;
			uint16_t min_water:1;
			uint16_t sensor_level:1;
      uint16_t water_sensor_setting:1;
      uint16_t WaterSensorMain:1;
      uint16_t MaxCurrent:1;
      uint16_t MinCurrent:1;
			uint16_t MicrosonicMain:1;
			uint16_t microsonic_setting:1;
    };	
  };	
  union{
    uint16_t blk4;
    struct{
			
      //gsm module------------
      uint16_t Gsm_Setting:1;
      uint16_t Gsm_Main:1;
      uint16_t server_phone:1;
			uint16_t adminphone2:1;
			uint16_t adminphone1:1;
			//general-----------
			uint16_t in_number:1;
			uint16_t type_sel:1;
			//rain sensor------      	
      uint16_t heater_temp_low:1;
      uint16_t heater_temp_hi:1;
      uint16_t RainSensorMain:1;
			uint16_t Rain_sensor_setting:1;
			
			//---adc calibration------
			uint16_t adc4_calib:1;      
      uint16_t adc5_calib:1;
      uint16_t current_calib:1;
			
						     
    };	
  };
	union{
    uint16_t blk5;
    struct{
			//-----log time-----------
			uint16_t log_prog:1;
      uint16_t log_first_page:1;
      uint16_t log_main:1;
      uint16_t log_interval:1;
			uint16_t sample_cou:1;
      uint16_t SetWaterLevelBarMain:1;
			uint16_t SetWaterLevelBar:1;
			//-----Water Setting-----------
			uint16_t RotaryMain:1;
			uint16_t Rotary_setting:1;
			uint16_t rotary_resolution:1;
			uint16_t rotary_diameter:1;
			uint16_t rotary_set_level:1;
			
    };	
  };
	
	
	 
  
} volatile flag_ir;

//-----------------keypad----------
uint8_t key_pressed;


char str[12][35]; //KEY buffer for display


void ir_rec(void){
  
  static unsigned char i=0;
  uint16_t time=0;
  
  if(i==0 || i==1)
    TIM6->CNT=0x00;
    
  if((1<i)&&(i<32)){
    time=TIM6->CNT;
    TIM6->CNT=0x00;
    
    if((200<time)&&(time<300))
      data_ir |=((unsigned long)1<<(i-2));
    else if((135<time)&&(time<170))
      data_ir &= ~((unsigned long)1<<(i-2));
    
    if(i==31){
                  
      flag.ir_flag=1;
      DisplayTime=0;
      if(flag.LcdOn==0 && flag.door==1){
        
        lcd_pwr_on;
        delayms(20);
        LCDinit();
        delayms(20);
        LCDclr();
        flag.LcdOn=1;
        flag.ir_flag=0; 
        display_count=4;
      }
      
      if(flag.ir_learn==0){
        flag_ir.invalid_remote=0;
        
        if(data_ir==button[0]) command_ir=MENU;
        else if(data_ir==button[1]) command_ir=up;
        else if(data_ir==button[2]) command_ir=down;
        else if(data_ir==button[3]) command_ir=ok;
        else if(data_ir==button[4]) command_ir=left;
        else if(data_ir==button[5]) command_ir=right;
        else if(data_ir==button[6]) command_ir=exit;
        
        else flag_ir.invalid_remote=1;
        
        data_ir=0;
        time_out=0;
        if(flag_ir.invalid_remote==0)
					flag.menu_setting=1;
          processing_ir_keypad();
      }
      
      i=0;      
      delayms(200);
      return;
    }
    
  }
    
  i++;
 
}


//-----------------scan_key--------------------------
void scan_key(){
	
	if(left_key==0){
			delayms(debounce_key);
			if(left_key==0){
				
				SetLevelBar_count++;
				SetLevelBar_timeout=0;
				if(SetLevelBar_count>20){
					flag.ir_flag=1;
					flag_ir.SetWaterLevelBarMain=1;
					flag_ir.water_sensor_setting=1;
					SetLevelBar_count=0;
				}
				
				key_pressed=1;
				command_ir=left;	
				
			}	
			
		}
		else if(menu_key==0){
			delayms(debounce_key);
			if(menu_key==0){
				flag.ir_flag=1;
				key_pressed=1;
				command_ir=MENU;
								
			}	
			
		}
		
		else if(down_key==0){
			delayms(debounce_key);
			if(down_key==0){
				
				key_pressed=1;
				command_ir=down;
							
			}	
			
		}
		
		else if(right_key==0){
			delayms(debounce_key);
			if(right_key==0){
				key_pressed=1;
			  command_ir=right;
				
			}	
			
		}
		else if(ok_key==0){
			delayms(debounce_key);
			if(ok_key==0){
				command_ir=ok;
				key_pressed=1;
			}	
			
		}
		
		else if(esc_key==0){
			delayms(debounce_key);
			if(esc_key==0){
								
				user_sms_count++;
				user_sms_timeout=0;
				if(user_sms_count>20){
					flag.UserCheck=1;
					user_sms_count=0;
				}
									
				key_pressed=1;
				command_ir=exit;
			}	
			
		}
		
		else if(up_key==0){
			delayms(debounce_key);
			if(up_key==0){
				command_ir=up;
				key_pressed=1;
			}	
			
		}
		
		if(key_pressed == 1){
			key_pressed=0;
			
			DisplayTime=0;
			time_out=0;
			flag.ir_flag=1;
			
			if(flag.LcdOn==0){
        
        lcd_pwr_on;
        delayms(20);
        LCDinit();
        delayms(20);
        LCDclr();
        flag.LcdOn=1;
        flag.ir_flag=0; 
        display_count=4;
				if(valid_door_flag==2){
					LCDputs("INVALID USER\n     [     ]    ");
					LCDcursorOn();
					LCDGotoXY(6,1);
					strcpy(input_number_str,"00000");
					LCDputs(input_number_str);
					LCDGotoXY(6,1);
					number_limit=5;
					number_limit_index=0;
				}
      }
			
			processing_ir_keypad();
			command_ir=0;
		}
		

}
//-----------------input number---------------
void input_number(){
	
	if(command_ir==up){
		input_number_str[number_limit_index]++;
		LCDsendChar(input_number_str[number_limit_index]);
		LCDcursorLeft(1);
	}
	else if(command_ir==down){
		input_number_str[number_limit_index]--;
		LCDsendChar(input_number_str[number_limit_index]);
		LCDcursorLeft(1);
	}
	else if(command_ir==left){
		if(number_limit_index>0){
						
			LCDcursorLeft(1);
			input_number_str[number_limit_index-1]=input_number_str[number_limit_index];
			number_limit_index--;
			LCDsendChar(input_number_str[number_limit_index]);
			LCDcursorLeft(1);
		}		
			
	}
	else if(command_ir==right){
		if(number_limit_index<(number_limit-1)){
						
			LCDcursorRight(1);
			input_number_str[number_limit_index+1]=input_number_str[number_limit_index];
			number_limit_index++;
			LCDsendChar(input_number_str[number_limit_index]);
			LCDcursorLeft(1);
		}
	}
	

}
//**************************************Processing IR recieved data************************************************
unsigned char processing_ir_keypad(){
  
  time_out=0;
	flag.menu_setting=1;
	
	if(flag.open_door==1 && command_ir==MENU){
		
		LCDclr();    
		LCDputsf("INPUT PASSWORD:\n     [     ]    ");
		LCDcursorOn();
		LCDGotoXY(6,1);
		strcpy(input_number_str,"00000");
		LCDputs(input_number_str);
		LCDGotoXY(6,1);
		number_limit=5;
		number_limit_index=0;
		
		flag.input_init_pass=1;
			
	}
	//------------------------input init password--------------------------------
  if(flag.input_init_pass==1){
		
		time_out_pass=0;
		input_number();
		if(command_ir==ok){
			input_pass=strtolong(input_number_str);
			if(input_pass==password){
				flag.input_init_pass=0;
				flag.open_door=0;
				command_ir=MENU;
			}
			else{
				LCDclr();    
				LCDputsf("INPUT PASSWORD:\n     [     ]    ");
				LCDcursorOn();
				LCDGotoXY(6,1);
				strcpy(input_number_str,"00000");
				LCDputs(input_number_str);
				LCDGotoXY(6,1);
				number_limit=5;
				number_limit_index=0;
			}
			
			if(valid_door_flag==2){
				if(password!=input_pass){
					flag.input_init_pass=1;
					number_limit_index=0;
					LCDGotoXY(6,1);
					strcpy(input_number_str,"00000");
					LCDputs(input_number_str);
					LCDGotoXY(6,1);
				}
			}
			
		}
		else if(command_ir==exit){
			flag.input_init_pass=0;
			flag.open_door=1;
			flag.main_menu=1;
			flag.ir_flag=0;
			flag.menu_setting=0;
		}
    
  }	
  //---------------------------------------------------------------------
  if(flag.open_door==0){
		
    if((command_ir==up || command_ir==down) && flag_ir.in_number==0){ 
      select();
     }
    
    else if(command_ir==MENU){
      flag_ir.first_menu=1;
      menu_prog();
    } 
    
       
    else if(flag_ir.menu==1 || flag_ir.display_adr==1 || flag_ir.display_sensor==1) menu_prog();
    
    else if(flag_ir.setup==1) setup_prog();
    
    else if (flag_ir.change_pass_routin==1) pass_change();
    
    else if(flag_ir.station_sett==1) st_setting();
    
    else if(flag_ir.add_external_device==1) add_device();
         
    else if(flag_ir.time_sett==1) time_setting();
            
    else if(flag_ir.water_sensor_setting==1) WaterSetting();
		
		else if(flag_ir.microsonic_setting==1) MicrosonicSetting();

    else if(flag_ir.Gsm_Setting==1) GsmSetting();
    
    else if(flag_ir.temp_setting==1) TemperatureSetting();
        
		else if(flag_ir.Rain_sensor_setting==1) RainSetting();
		
		else if(flag_ir.ADCcalibration==1)ADCcalib_routin();
		
		else if(flag_ir.log_prog==1) log_setting();
		
		else if(flag_ir.Rotary_setting==1) RotarySetting();
    
		else if(command_ir==exit){
			flag.ir_flag=0;
			clear_ir_flags();
		}
			
	}
	
  
  return 0;	
}

//***********************************************************************
void setup_prog (){
  
  if(flag_ir.setup_main==1){
    
    clear_ir_flags();
    end_key=8;
    start_key=4;
    key=4;
    flag_ir.setup_main=0;
    flag_ir.setup=1;
    
    static const char str_f1[]="Change Pass";
    static const char str_f2[]="Station Sett";  
    static const char str_f3[]="Time Sett";
    static const char str_f4[]="Clear Mem" ;  
		static const char str_f5[]="Reboot" ;
    
    strcpy(str[4],str_f1);
    strcpy(str[5],str_f2);
    strcpy(str[6],str_f3);
    strcpy(str[7],str_f4);
		strcpy(str[8],str_f5);
        
    LCDclr();
    LCDputs(str[4]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
    
  }
  
  else if(flag_ir.setup==1){
    
    
    if(command_ir==ok)
      
    {
      
      switch(key){
      case 4:
        
        flag_ir.chang_pas=1;
        flag_ir.setup=0;
        pass_change();
        break;
        
      case 5:
        
        flag_ir.st_set_main=1;
        flag_ir.setup=0;
        st_setting();									
        break;
        
              
      case 6:
        flag_ir.time_sett_main=1;
        flag_ir.setup=0;	
        time_setting();
        break;
        
      case 7:
        clear_eeprom();
        LCDclr();
        LCDputs("Clear Ok!");
        delayms(1000);
        LCDclr();
        LCDputs("Device Reset");
        HAL_NVIC_SystemReset();
			case 8:
        LCDclr();
        LCDputs("Device Reset");
        HAL_NVIC_SystemReset();
      }	
    }
    
    else if(command_ir==exit){
      flag_ir.first_menu=1;
      menu_prog();
    }		
  }
  
}

void pass_change(){
  
  if(flag_ir.chang_pas==1){
    
    flag_ir.pas=1;
    flag_ir.chang_pas=0;
    flag_ir.change_pass_routin=1;
    key=0;
    ir_cou=0;
    input_pass=0;
    LCDclr();
    static const char str_f6[]="Enter Old Pass:\n     [     ]    ";
    LCDputsf(str_f6);
		
		LCDcursorOn();
		LCDGotoXY(6,1);
		
		strcpy(input_number_str,"00000");
		LCDputs(input_number_str);
		LCDGotoXY(6,1);
		number_limit=5;
		number_limit_index=0;
		flag_ir.in_number=1;
  }	
  else if(flag_ir.pas==1){
    
		input_number();
		if(command_ir==ok){
			input_pass=strtolong(input_number_str);
			if(input_pass==password){
        delayms(200);
        LCDclr();
        static const char str_f7[]="Enter New Pass:\n     [     ]    ";
        LCDputsf(str_f7);
        flag_ir.new_pas=1;
        flag_ir.pas=0;
        input_pass=0;
        LCDcursorOn();
				LCDGotoXY(6,1);
				
				strcpy(input_number_str,"00000");
				LCDputs(input_number_str);
				LCDGotoXY(6,1);
				number_limit=5;
				number_limit_index=0;
				flag_ir.in_number=1;
      }
      else{
        LCDclr();
        static const char str_f8[]="WRONG PASS!";
        LCDputsf(str_f8);
				flag_ir.chang_pas=1;
        delayms(1000);
        pass_change();
        
      }
			
		}
		else if(command_ir==exit){
			flag_ir.setup_main=1;
			setup_prog();
		}    
   
  }
  //-----------------------------------------------------
  else if(flag_ir.new_pas==1){  
    
    input_number();
    if(command_ir==ok){
			input_pass=strtolong(input_number_str);
			LCDclr();
      static const char str_f9[]="REWRITE PASS:\n     [     ]    ";
      LCDputsf(str_f9);
      LCDcursorOn();
			LCDGotoXY(6,1);
			
			strcpy(input_number_str,"00000");
			LCDputs(input_number_str);
			LCDGotoXY(6,1);
			number_limit=5;
			number_limit_index=0;
			flag_ir.in_number=1;
      prim_password=input_pass;
			input_pass=0;
      flag_ir.emph=1;
      flag_ir.new_pas=0;
      delayms(200);
									
		}
		else if(command_ir==exit){
			flag_ir.setup_main=1;
			setup_prog();
		}
		
    
  }
  
  //---------------------------------------------------------------------
  
  else if(flag_ir.emph==1){
    
    input_number();
    if(command_ir==ok){
			input_pass=strtolong(input_number_str);
      if(input_pass==prim_password){
        flag_ir.emph=0;
        flag_ir.pas_ok=1;
        input_pass=0;
        delayms(200);
        LCDclr();
				LCDputs("SAVE OK");
				password=prim_password;
				flag_ir.pas_ok=0;
				prim_password=0;
				write_exteeprom_word(password,password_ee_adr);
				write_exteeprom_byte(0,32);
				flag_ir.change_pass_routin=0;
				flag_ir.setup_main=1;
				delayms(1000);
				setup_prog();        
        
      }
      else{
        flag_ir.emph=0;
        flag_ir.pas=0;
        ir_cou=0;
        input_pass=0;
        LCDclr();
        static const char str_f11[]="False Input";
        LCDputsf(str_f11);
        flag_ir.chang_pas=1;
        delayms(1000);
        pass_change();
        
      }
    }
		
		else if(command_ir==exit){
			flag_ir.setup_main=1;
			setup_prog();
		}
  }	
  //------------------------------------------
  
  else if(command_ir==exit){
    flag_ir.setup_main=1;
    setup_prog();
  }
  
}
//---------------------------------------------
char st_setting(){
  
  if(flag_ir.st_set_main==1){
    
    ir_cou=0;
    key=5;
    start_key=5;
    flag_ir.st_set=1;
    flag_ir.station_sett=1;
    flag_ir.st_set_main=0;
    end_key=10;
    static const char str_f12[]="St Type";
    static const char str_f13[]="Add Device"; 
    static const char str_f14[]="ADC Calib" ; 
		static const char str_f15[]="Log Time" ; 
		static const char str_f16[]="Test Mode" ;
		static const char str_f17[]="LoadSetting" ;	
		
    strcpy(str[5],str_f12);
    strcpy(str[6],str_f13);
		strcpy(str[7],str_f14);
		strcpy(str[8],str_f15);
		strcpy(str[9],str_f16);
		strcpy(str[10],str_f17);
		
    LCDclr();
    LCDputs(str[5]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
  }
  //----------------------------		 
  else if(command_ir==ok){
    
    if(flag_ir.st_set==1){
      ir_cou=0;
      LCDclr();			
      switch(key){
              
      case 5:
        flag_ir.type_sel=1;
        flag_ir.st_set=0;
        key=4;
        start_key=4;
        end_key=5;
        static const char str_f15[]="Repeater";
        static const char str_f16[]="End ST";  
        strcpy(str[4],str_f15);
        strcpy(str[5],str_f16);
        
        LCDputs(str[4]);
        LCDGotoXY(15,0);
        LCDsendChar(up_arrow);
        LCDGotoXY(15,1);
        LCDsendChar(down_arrow);				
        
        break;
        
        
      case 6:
				flag_ir.st_set=0;
        flag_ir.add_device_main=1;
        flag_ir.add_external_device=1;
        flag_ir.station_sett=0;
        add_device();
        
        break;	
        
			case 7:
				key=0;
				flag_ir.st_set=0;
        flag_ir.adc_calib_main=1;
				flag_ir.ADCcalibration=1;
        flag_ir.station_sett=0;
        ADCcalib_routin(); 
			
				break;
			case 8:
				key=0;
				flag_ir.st_set=0;
        flag_ir.log_first_page=1;
				flag_ir.station_sett=0;
				log_setting();
			
				break;
			case 9:
					if(flag.test_mode==0){
						flag.test_mode=1;
						test_mode_timeout=0;
						LCDGotoXY(0,1);
						LCDputs("Test Mode Active");
					}
					else{
						flag.test_mode=0;
						LCDGotoXY(0,1);
						LCDputs("Test Mode Deactive");
					}
			break;
			case 10:
					loadSettingSD();
					LCDGotoXY(0,1);
					LCDputs("Setting Loaded");
					delayms(1000);
					HAL_NVIC_SystemReset();
			break;
			
      }
    }
    
    else if(flag_ir.type_sel==1){
      
      switch(key){
      case 4:
        st_type=rep;
        rf_pwr_on;
        write_exteeprom_byte(st_type,st_type_ee_adr);
        
        LCDGotoXY(0,1);
        static const char str_f17[]="SAVE OK";
        LCDputsf(str_f17);
        delayms(1000);
        LCDGotoXY(0,1);
        LCDputs("               ");
        break;
        
      case 5:
        st_type=0;
        write_exteeprom_byte(st_type,st_type_ee_adr);
        
        LCDGotoXY(0,1);
        LCDputsf(str_f17);
        delayms(1000);
        LCDGotoXY(0,1);
        LCDputs("               ");					
        break;
        
        
        
      }
      
    }
    
    
       
  }
  
  else if(command_ir==exit){
    if(flag_ir.st_set==1) {
      clear_ir_flags();
      flag_ir.setup_main=1;
      setup_prog();
    }
    else if(flag_ir.type_sel==1){
      clear_ir_flags();
      flag_ir.st_set_main=1;
      st_setting();
    }
    
    
    
  }
  
  return 0;
}
char add_device(){
  
  
  if(flag_ir.add_device_main==1){
    clear_ir_flags();
    flag_ir.add_device_main=0;
    flag_ir.select_sensor=1;
		flag_ir.add_external_device=1;
    key=4;
    start_key=4;
    end_key=11;
    static const char str_f1[]="Temp Sensor";
    static const char str_f2[]="Motor";
    static const char str_f3[]="Rain Sensor";
    static const char str_f4[]="Water Sensor";
    static const char str_f5[]="GSM Module";
		static const char str_f6[]="RADIO MODEM";
		static const char str_f7[]="WIFI MODULE";
		static const char str_f8[]="CAMERA MODULE"; 
    
    strcpy(str[4],str_f1);
    strcpy(str[5],str_f2);
    strcpy(str[6],str_f3);
    strcpy(str[7],str_f4);
    strcpy(str[8],str_f5);
		strcpy(str[9],str_f6);
		strcpy(str[10],str_f7);
		strcpy(str[11],str_f8);
		    
    LCDclr();
    LCDputs(str[4]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
  }
  else if(command_ir==ok){
    if(flag_ir.select_sensor==1){
      switch(key){
      case 4:
        if((ext_device & temp_sensor)==0){
          clear_ir_flags();
          key=0;
          flag_ir.select_sensor=0;
          flag_ir.temp_setting=1;
          flag_ir.temp_set_main=1;
          TemperatureSetting();
        }
        else{
          
          ext_device &= ~temp_sensor;
          write_exteeprom_word(ext_device,ext_device_ee_adr);
          LCDGotoXY(0,1);
          LCDputsf(erase);
          LCDGotoXY(0,1);
          LCDputsf(desel);
          delayms(1000);
          LCDGotoXY(0,1);
          LCDputsf(erase);
          
        }
        
        
        break;
      case 5:
        if((ext_device & snow_motor)==0){
          
          ext_device|=snow_motor;
          write_exteeprom_word(ext_device,ext_device_ee_adr);
          LCDGotoXY(0,1);
          LCDputsf(erase);
          LCDGotoXY(0,1);
          
          LCDputsf(sel);
          delayms(1000);
          LCDGotoXY(0,1);
          LCDputsf(erase);
        }
        else{
          ext_device &= ~snow_motor;
          write_exteeprom_word(ext_device,ext_device_ee_adr);
          LCDGotoXY(0,1);
          LCDputsf(erase);
          LCDGotoXY(0,1);
          LCDputsf(desel);
          delayms(1000);
          LCDGotoXY(0,1);
          LCDputsf(erase);
        }
        break;
      case 6:
				clear_ir_flags();
				flag_ir.RainSensorMain=1;
				flag_ir.Rain_sensor_setting=1;
        RainSetting();
        break;
      case 7:
        clear_ir_flags();
				flag_ir.WaterSensorMain=1;
				flag_ir.water_sensor_setting=1;
				WaterSetting();
        break;
      case 8:
        clear_ir_flags();
        flag_ir.Gsm_Main=1;
        flag_ir.Gsm_Setting=1;
        GsmSetting();
        break;
			case 9:
				 if((ext_device & RadioModem)==0){
          
          ext_device|=RadioModem;
          write_exteeprom_word(ext_device,ext_device_ee_adr);
          LCDGotoXY(0,1);
          LCDputsf(erase);
          LCDGotoXY(0,1);
          
          LCDputsf(sel);
          delayms(1000);
          LCDGotoXY(0,1);
          LCDputsf(erase);
        }
        else{
          ext_device &= ~RadioModem;
          write_exteeprom_word(ext_device,ext_device_ee_adr);
          LCDGotoXY(0,1);
          LCDputsf(erase);
          LCDGotoXY(0,1);
          LCDputsf(desel);
          delayms(1000);
          LCDGotoXY(0,1);
          LCDputsf(erase);
        }
				break;
				case 10:
				 if((ext_device & wifi)==0){
          
          ext_device|=wifi;
          write_exteeprom_word(ext_device,ext_device_ee_adr);
          LCDGotoXY(0,1);
          LCDputsf(erase);
          LCDGotoXY(0,1);
          
          LCDputsf(sel);
          delayms(1000);
          LCDGotoXY(0,1);
          LCDputsf(erase);
        }
        else{
          ext_device &= ~wifi;
          write_exteeprom_word(ext_device,ext_device_ee_adr);
          LCDGotoXY(0,1);
          LCDputsf(erase);
          LCDGotoXY(0,1);
          LCDputsf(desel);
          delayms(1000);
          LCDGotoXY(0,1);
          LCDputsf(erase);
        }
				break;
				case 11:
				 if((ext_device & camera)==0){
          
          ext_device|=camera;
          write_exteeprom_word(ext_device,ext_device_ee_adr);
          LCDGotoXY(0,1);
          LCDputsf(erase);
          LCDGotoXY(0,1);
          
          LCDputsf(sel);
          delayms(1000);
          LCDGotoXY(0,1);
          LCDputsf(erase);
        }
        else{
          ext_device &= ~camera;
          write_exteeprom_word(ext_device,ext_device_ee_adr);
          LCDGotoXY(0,1);
          LCDputsf(erase);
          LCDGotoXY(0,1);
          LCDputsf(desel);
          delayms(1000);
          LCDGotoXY(0,1);
          LCDputsf(erase);
        }
				break;
        
      }
      
    }
        
  }
  
  else if(command_ir==exit){
    
		clear_ir_flags();
		flag_ir.station_sett=1;
		flag_ir.st_set_main=1;
		
		st_setting();
    
  }
  
  return 0;	
}

//--------------------------------------------------
void time_setting(){
char *str_point=NULL;
	
  if(flag_ir.time_sett_main==1){
    
    key=4;
    start_key=4;
    end_key=5;
    flag_ir.time_sett_main=0;
    flag_ir.time_sett=1;
    flag_ir.time_set_enter=1;
    static const char str_f1[]="Date Setting";
    static const char str_f2[]="Clock Setting";
    strcpy(str[4],str_f1);
    strcpy(str[5],str_f2);
    LCDclr();
    LCDputs(str[4]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
    
    
  }
  else if(flag_ir.time_set_enter==1 && command_ir==ok){
    flag_ir.time_set_enter=0;
    switch(key)	{
    case 4:
      LCDclr();
      static const char str_f3[]="Input Date:";
      LCDputsf(str_f3);
      flag_ir.d=1;
		
			LCDcursorOn();
			LCDGotoXY(0,1);
			strcpy(input_number_str,"00000000");
			LCDputs(input_number_str);
			LCDGotoXY(0,1);
			number_limit=8;
			number_limit_index=0;
			flag_ir.in_number=1;
      break;	
      
    case 5:
      LCDclr();
      static const char str_f4[]="Input Time:";
      LCDputsf(str_f4);
      flag_ir.t=1;
			
			LCDcursorOn();
			LCDGotoXY(0,1);
			strcpy(input_number_str,"0000");
			LCDputs(input_number_str);
			LCDcursorOn();
			LCDGotoXY(0,1);
			number_limit=4;
			number_limit_index=0;
			flag_ir.in_number=1;
      break;	
    }
    
    
  }
  
  else if(flag_ir.d==1){
    input_number();
    
    if(command_ir==ok){
			flag.rtcset=1;
			buf[4]=0;
			memcpy(buf,input_number_str,4);
			clock.year=strtolong(buf);
      buf[2]=0;
			memcpy(buf,input_number_str+4,2);
			clock.month=strtolong(buf);
			
			memcpy(buf,input_number_str+6,2);
			clock.day=strtolong(buf);
      
      rtc_settime_shamsi(&clock);          
      LCDclr();
      LCDputsf(sel);
      delayms(1000);
      			
      flag_ir.time_sett_main=1;
      flag_ir.d=0;
      time_setting();
      
    }
    else if(command_ir==exit){
      flag_ir.time_sett_main=1;
      flag_ir.d=0;
      time_setting();
    }
    
  }
  else if(flag_ir.t==1){
		input_number();
    if(command_ir==ok){
			buf[2]=0;
			memcpy(buf,input_number_str,2);
			clock.hour=strtolong(buf);
      
			memcpy(buf,input_number_str+2,2);
			clock.min=strtolong(buf);
			
			rtc_settime_shamsi(&clock);
      
//      next_log_time=(clock.hour*60)+clock.min+log_time;
//      if(next_log_time>1440){next_log_time=next_log_time%1440; flag.log_disable=1;}
      
      LCDclr();
      LCDputsf(sel);
      flag_ir.time_sett_main=1;
      flag_ir.t=0;
      delayms(1000);
      
      time_setting();
      
    }
    else if(command_ir==exit){
      flag_ir.time_sett_main=1;
      flag_ir.t=0;
      time_setting();	
    }
    
  }
  
  else if(command_ir==exit) {
    flag_ir.setup_main=1;
    setup_prog();
  }
  
}

void menu_prog(){
  char *point;  
  
static const char str_f1[]="Setup";
static const char str_f2[]="Display Spec";
static const char str_f3[]="Display Sensor";
static const char str_f4[]="Relay 2";

  
  if(flag_ir.first_menu==1){
    
    clear_ir_flags();
    end_key=7;
    start_key=4;
    key=4;
    flag_ir.menu=1;
    flag_ir.first_menu=0;
    strcpy(str[4],str_f1);
    strcpy(str[5],str_f2);
    strcpy(str[6],str_f3);
    strcpy(str[7],str_f4);
    
    
    LCDclr();
    LCDputs(str[4]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
        
  }
  
  else if(flag_ir.menu==1){
    if(command_ir==ok){
      switch (key){
      case 4:
        flag_ir.menu=0;
        flag_ir.setup_main=1;
        setup_prog();
        break;
      case 5:
        for(i=4;i<10;i++)strcpy(str[i],"");
        flag_ir.display_adr=1;
        flag_ir.menu=0;
        end_key=6;
        start_key=4;
        key=4;
        static const char str_f6[]="Device ID:\n";
        strcpy(str[4],str_f6);
        sprintf(str[4]+11,"%d",station_address);
        LCDclr();
        LCDputs(str[4]);
        LCDGotoXY(15,0);
        LCDsendChar(up_arrow);
        LCDGotoXY(15,1);
        LCDsendChar(down_arrow);
			
				sprintf(str[5],"Device Name\n%s",station_name);
				sprintf(str[6],"VERSION\n%s",firmware_version);
							
			
			/*
        static const char str_f7[]="St Type:Rep";
        static const char str_f8[]="St Type:End_St";
        
        if(st_type==rep)
          
          strcpy(str[5],str_f7);
        else
          
          strcpy(str[5],str_f8);
        
          static const char str_f9[]="Base Addr:";
          strcpy(str[6],str_f9);
          sprintf(str[6]+10,"\n%d %d %d %d",*((uint16_t*)(base_address+1)),*((uint16_t*)(base_address+3)),*((uint16_t*)(base_address+5)),*((uint16_t*)(base_address+7)));
        
          
        static const char str_f10[]="POWER TYPE =\n line";
        static const char str_f11[]="POWER TYPE =\n Solar";
        static const char str_f12[]="POWER TYPE =\n Solar & Line";
        static const char str_f13[]="POWER TYPE =\n No Power";
                
        switch(pwr_existence){
        case 1:
          strcpy(str[7],str_f10);
          
          break;
        case 2:
          strcpy(str[7],str_f11);
          
          break;
        case 3:
          strcpy(str[7],str_f12);
          break;
        default :
          strcpy(str[7],str_f13);
          
        }
        */
        break;
        
      case 6:
         
        for(i=4;i<10;i++)strcpy(str[i],"");
        end_key=3;
        start_key=4;
        key=4;
        flag_ir.display_sensor=1;
        flag_ir.menu=0;
        
        static const char str_f14[]="Battery Volt=";
        static const char str_f15[]="Solar Volt=";
                
				bat_v=READ_ADC(BatteryVoltageADC,BatteryVoltageADCChannel,BatteryVoltageADCScale);
				
				sol_v=READ_ADC(SolarVoltageADC,SolarVoltageADCChannel,SolarVoltageADCScale);
        
				end_key++;
				strcpy(str[end_key],str_f14);
				sprintf(str[end_key]+12,"\n%4.2f",bat_v);
				
				end_key++;
				strcpy(str[end_key],str_f15);
				sprintf(str[end_key]+10,"\n%4.2f",sol_v);
				
        LCDclr();
        LCDputs(str[4]);
        LCDGotoXY(15,0);
        LCDsendChar(up_arrow);
        LCDGotoXY(15,1);
        LCDsendChar(down_arrow);
        
				end_key++;
        temp_lm75(&temp_in);
				sprintf(str[end_key],"Temp in=%d%cC",temp_in,up_point);
				
				
        if(ext_device & temp_sensor){
					end_key++;
					ReadExtTemp(&temp_out);
					sprintf(str[end_key],"Out Temp=%d%c",temp_out,up_point);
					
          if(temp_type & SHT20){
						end_key++;
						sprintf(str[end_key],"Humidity=%d",humidity);
						
					}
					  
        }
                
				if(ext_device & water_sensor){
					end_key++;
					read_water_level();
					sprintf(str[end_key],"Water_Lvl= %d",water_level);
					
				}
				if(ext_device & rain_sensor){
					end_key++;
					sprintf(str[end_key],"Rain_Lvl=\n%5.3f",rain_level);
					
        }
				
//				if(ext_device & gsm){
//          end_key++;
//          flush_buf(SimRecBuffer,250);
//          SimRecLen=0;
//          sprintf(SimSendBuffer,"AT+CSQ%c%c",CRchar,LF);
//          GsmSend(SimSendBuffer);
//          delayms(1000);
//          point=CharSearch(SimRecBuffer,LF,250);
//          if(point!=NULL){
//            *(point+11)=0;
//            gsm_signal=(unsigned char)strtol(point+8,endptr,10);
//            gsm_signal=(gsm_signal*2)-114;
//            sprintf(str[end_key],"GSM SIGNAL:\n%d dbm",gsm_signal);
//          }                  
//        }
        
        break;
            
      case 7:
        
        if(flag_ir.snow_test==0){
          flag_ir.snow_test=1;
          snow_motor_on;
          LCDGotoXY(0,1);
          LCDputs("ON ");
                    
        }
        else{
          flag_ir.snow_test=0;
          snow_motor_off;
          LCDGotoXY(0,1);
          LCDputs("OFF");
        }
       break; 
        
        
      }
      
      
    }
    else if(command_ir==exit){
      
      flag.disp = 0;
      display_count=4;
			flag.main_menu=1;
      flag.ir_flag=0;
      flag_ir.menu=0;
			flag.open_door=1;
      clear_ir_flags();
			flag.menu_setting=0;
      
    }
    
    
  }
  else if(command_ir==exit) {
    if(flag_ir.display_adr==1 || flag_ir.display_sensor==1){
      flag_ir.first_menu=1;
      menu_prog();
    }
    
  }
  
}


//********************************************************
void GsmSetting(){
  if(flag_ir.Gsm_Main==1){
    
    flag_ir.Gsm_Main=0;
    key=4;
    start_key=4;
    end_key=7;
    static const char str_f1[]="GSM Select";
    static const char str_f2[]="Server Phone";
		static const char str_f3[]="AdminMobile1";
		static const char str_f4[]="AdminMobile2";
    strcpy(str[4],str_f1);
    strcpy(str[5],str_f2);    
    strcpy(str[6],str_f3);
    strcpy(str[7],str_f4);
		
    LCDclr();
    LCDputs(str[4]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
    
  }
  
  else if(command_ir==ok){
    switch(key){
    case 4:
      if((ext_device & gsm)==0){
        
       // GsmInit();
				ext_device |= gsm;
        write_exteeprom_dword(ext_device,ext_device_ee_adr);
				
        LCDGotoXY(0,1);
        LCDputsf(sel);
        delayms(1000);
        LCDGotoXY(0,1);
        LCDputsf(erase);
          
        }
        else{
          
          ext_device &= ~gsm;
          write_exteeprom_word(ext_device,ext_device_ee_adr);
          LCDGotoXY(0,1);
          LCDputsf(desel);
          delayms(1000);
          LCDGotoXY(0,1);
          LCDputsf(erase);
          
        }
      return;
            
      
    case 5:
      key=0;
      ir_cou=0;
      flag_ir.server_phone=1;
      
      LCDclr();
      LCDputs("Server Phone:");
      LCDGotoXY(0,1);
      LCDputsf(erase);
      LCDGotoXY(0,1);
      
			strcpy(input_number_str,"000000000000");
			LCDputs(input_number_str);
			LCDcursorOn();
			LCDGotoXY(0,1);
			number_limit=12;
			number_limit_index=0;
			flag_ir.in_number=1;
      return;
		
		case 6:
      key=0;
      ir_cou=0;
      flag_ir.adminphone1=1;
      LCDclr();
      LCDputsf("Admin Mobile1:");
      LCDGotoXY(0,1);
      LCDputsf(erase);
      LCDGotoXY(0,1);
      
			read_exteeprom_ary(AdminMobile,29,AdminMobile1_ee_adr);
			if(AdminMobile[0]!=0xFF){
				strcpy(input_number_str,AdminMobile);
			}
			else
				strcpy(input_number_str,"000000000000");
			
			LCDputs(input_number_str);
			LCDcursorOn();
			LCDGotoXY(0,1);
			number_limit=12;
			number_limit_index=0;
			flag_ir.in_number=1;
      return;
		
		case 7:
      key=0;
      ir_cou=0;
      flag_ir.adminphone2=1;
      LCDclr();
      LCDputsf("Admin Mobile2:");
      LCDGotoXY(0,1);
      LCDputsf(erase);
      LCDGotoXY(0,1);
      
			read_exteeprom_ary(AdminMobile,29,AdminMobile2_ee_adr);
			if(AdminMobile[0]!=0xFF){
				strcpy(input_number_str,AdminMobile);
			}
			else
				strcpy(input_number_str,"000000000000");
			
			LCDputs(input_number_str);
			LCDcursorOn();
			LCDGotoXY(0,1);
			number_limit=12;
			number_limit_index=0;
			flag_ir.in_number=1;
      return;
		
    }
    //------Save Value---------------------------
    if(flag_ir.server_phone==1){
      flag_ir.server_phone=0;
      strcpy(ServerPhone,input_number_str);
      write_exteeprom_ary(ServerPhone,29,server_phone_ee_adr);
      
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.Gsm_Main=1;
      flag_ir.Gsm_Setting=1;
      GsmSetting();
    
    }
		
		if(flag_ir.adminphone1==1){
      flag_ir.adminphone1=0;
      strcpy(ServerPhone,input_number_str);
      write_exteeprom_ary(input_number_str,29,AdminMobile1_ee_adr);
      
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.Gsm_Main=1;
      flag_ir.Gsm_Setting=1;
      GsmSetting();
    
    }
		
		if(flag_ir.adminphone2==1){
      flag_ir.adminphone2=0;
      
      write_exteeprom_ary(input_number_str,29,AdminMobile2_ee_adr);
      
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.Gsm_Main=1;
      flag_ir.Gsm_Setting=1;
      GsmSetting();
    
    }
    
    
  }
  // exit process------------
  else if(command_ir==exit){
    if(flag_ir.server_phone==1 || flag_ir.adminphone1==1 || flag_ir.adminphone2==1){
      clear_ir_flags();
      flag_ir.Gsm_Setting=1;
      flag_ir.Gsm_Main=1;
      GsmSetting();
    }
    else{
      clear_ir_flags();
      flag_ir.add_device_main=1;
      flag_ir.add_external_device=1;
      add_device();
    }
   }
		
  //---------Input Value---------
  else if(flag_ir.server_phone==1 || flag_ir.adminphone1==1 || flag_ir.adminphone2==1){
    input_number();
    
  }
  
}

//*****************************************************************************
void TemperatureSetting(){
  if(flag_ir.temp_set_main==1){
    flag_ir.temp_set_main=0;
    key=4;
    start_key=4;
    end_key=6;
    static const char str_f1[]="PT100";
    static const char str_f2[]="NTC";
    static const char str_f3[]="SHT20";
		
    strcpy(str[4],str_f1);
    strcpy(str[5],str_f2);
    strcpy(str[6],str_f3);
    LCDclr();
    LCDputs(str[4]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
      
  }
  else if(command_ir==ok){
    
    switch(key){
    case 4:
      if((temp_type & PT100)==0){
        key=0;
        flag_ir.temp_calib=1;
        
        LCDclr();
        static const char str_f3[]="Enter Calib Val:";
        LCDputsf(str_f3);
				LCDGotoXY(0,1);
				LCDputs("000");
				strcpy(input_number_str,"000");
				LCDcursorOn();
				LCDGotoXY(0,1);
							
				number_limit=3;
				number_limit_index=0;
				flag_ir.in_number=1;
      }
      else{
        
        temp_type &= ~PT100;
        write_exteeprom_byte(temp_type,temp_type_ee_adr);
        temp_difference=0;
        write_exteeprom_byte(temp_difference,temp_difference_ee_adr);
        ext_device &= ~temp_sensor;
        write_exteeprom_word(ext_device,ext_device_ee_adr);
        LCDGotoXY(0,1);
        
        LCDputsf(erase);
        LCDGotoXY(0,1);
        
        LCDputsf(desel);
        delayms(1000);
        LCDGotoXY(0,1);
        LCDputsf(erase);
      }
      return;
    case 5:
      if((temp_type & NTC)==0){
        
        temp_type = NTC;
        write_exteeprom_byte(temp_type,temp_type_ee_adr);
        
        ext_device|=temp_sensor;
        write_exteeprom_word(ext_device,ext_device_ee_adr);
        LCDGotoXY(0,1);
        LCDputsf(erase);
        LCDGotoXY(0,1);
        
        LCDputsf(sel);
        delayms(1000);
        LCDGotoXY(0,1);
        LCDputsf(erase);
      }
      else{
        
        temp_type &= ~NTC;
        write_exteeprom_byte(temp_type,temp_type_ee_adr);
        ext_device &= ~temp_sensor;
        write_exteeprom_word(ext_device,ext_device_ee_adr);
        
        LCDGotoXY(0,1);
        
        LCDputsf(erase);
        LCDGotoXY(0,1);
        
        LCDputsf(desel);
        delayms(1000);
        LCDGotoXY(0,1);
        LCDputsf(erase);
      }
			case 6:
      if((temp_type & SHT20)==0){
        
        temp_type = SHT20;
        write_exteeprom_byte(temp_type,temp_type_ee_adr);
        
        ext_device|=temp_sensor;
        write_exteeprom_word(ext_device,ext_device_ee_adr);
        LCDGotoXY(0,1);
        LCDputsf(erase);
        LCDGotoXY(0,1);
        
        LCDputsf(sel);
        delayms(1000);
        LCDGotoXY(0,1);
        LCDputsf(erase);
      }
      else{
        
        temp_type &= ~SHT20;
        write_exteeprom_byte(temp_type,temp_type_ee_adr);
        ext_device &= ~temp_sensor;
        write_exteeprom_word(ext_device,ext_device_ee_adr);
        
        LCDGotoXY(0,1);
        
        LCDputsf(erase);
        LCDGotoXY(0,1);
        
        LCDputsf(desel);
        delayms(1000);
        LCDGotoXY(0,1);
        LCDputsf(erase);
      }

      return;
    
    }
    
    if(flag_ir.temp_calib==1){
      
      flag_ir.temp_calib=0;
			temp_val=strtolong(input_number_str);
      ReadExtTemp(&temp_out);
      temp_difference=temp_val-temp_out;
      write_exteeprom_byte(temp_difference,temp_difference_ee_adr);
      ext_device|=temp_sensor;
      write_exteeprom_word(ext_device,ext_device_ee_adr);
      temp_type = PT100;
      write_exteeprom_byte(temp_type,temp_type_ee_adr);
                  
      static const char str_f4[]="Calib Value:\n%d";
      sprintf(buf,str_f4,temp_difference);
      LCDclr();
      LCDputs(buf);
      
      ir_cou=0;
      delayms(2000);
      flag_ir.temp_set_main=1;
      TemperatureSetting();
            
    }
  
  }
  
	else if(command_ir==exit){
    if(flag_ir.temp_calib==1){
      clear_ir_flags();
      flag_ir.temp_setting=1;
      flag_ir.temp_set_main=1;
      TemperatureSetting();
    }
    else{
      clear_ir_flags();
      flag_ir.add_device_main=1;
      flag_ir.add_external_device=1;
      add_device();
    }
  }
	
  else if(flag_ir.temp_calib==1){
    input_number();
    
  }
  
  
  
}



//-----------------------------------------------------
void select(){
  
  if(key!=0){
    
    
    if(command_ir==down){
      
      key++;
      if(key>end_key) key=start_key;
      
    }
    
    if(command_ir==up){
      
      key--;
      if(key<start_key) key=end_key;
      
    }
    LCDclr();
    LCDputs(str[key]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
    
  }
  
}

void clear_ir_flags(){
  
  flag_ir.blk1=0;
  flag_ir.blk2=0;
  flag_ir.blk3=0;
  flag_ir.blk4=0;
	flag_ir.blk5=0;
  key=0;
  start_key=0;
  end_key=0;
	flag_ir.in_number=0;
  
}
//*********************************************************************
char IR_Learning(){
  
char index=0;  
  
  flag.ir_learn=1;
  LCDclr();
  static const char str_f1[]="IR LEARNING";   
  LCDputsf(str_f1);
  
  for(index=0;index<7;index++){
   
	switch (index){
		case 0:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f3[]="Press \"MENU\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f3);  
			
			break;
		case 1:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f4[]="Press \"UP\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f4); 
			break;
		case 2:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f5[]="Press \"DOWN\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f5); 
			break;
		case 3:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f6[]="Press \"OK\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f6); 
			break;  
		case 4:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f7[]="Press \"LEFT\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f7); 
			break;
		case 5:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f8[]="Press \"RIGHT\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f8); 
			break;
		case 6:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f9[]="Press \"ECS\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f9); 
			break;
        
        
    }
      
      count=0;
      while((flag.ir_flag==0) && count<10)delayms(1);
      if(flag.ir_flag==1){
        button[index]=data_ir;
        data_ir=0;
        flag.ir_flag=0;
      }
      else return 0;
   
    
  }
  
  write_exteeprom_ary((unsigned char*)button,30,button_ir_ee_adr);
  
  
  LCDclr();
  static const char str_f10[]="LEARN OK";
  LCDputsf(str_f10); 
  delayms(2000);
  return 1;
}

//------------------------------------------------------------------
void WaterSetting(){
  if(flag_ir.SetWaterLevelBarMain==1){
		flag_ir.SetWaterLevelBarMain=0;
		flag_ir.SetWaterLevelBar=1;
		LCDclr();
		LCDputs("INPUT LEVEL BAR");
		LCDGotoXY(0,1);
		strcpy(input_number_str,"0000");
		LCDputs(input_number_str);
		LCDcursorOn();
		LCDGotoXY(0,1);
		
		number_limit=4;
		number_limit_index=0;
		flag_ir.in_number=1;
		
	}
  else if(flag_ir.WaterSensorMain==1){
    flag_ir.WaterSensorMain=0;
    
    key=4;
    start_key=4;
    end_key=7;
    strcpy(str[4],"Microsonic");
    strcpy(str[5],"Thalimedes");
		strcpy(str[6],"RotaryEncoder");
    strcpy(str[7],"SensorSelection");
		LCDclr();
    LCDputs(str[4]);   
  }
  else if(command_ir==ok){
          
		switch(key){
			
		case 4:
			
			clear_ir_flags();
			flag_ir.MicrosonicMain=1;
			flag_ir.microsonic_setting=1;
			MicrosonicSetting();
			
			
			break;
		
		case 5:
			if(WaterSensorType != thalimedes){
				WaterSensorType = thalimedes;
				write_exteeprom_byte(WaterSensorType,water_type_ee_adr);
				LCDGotoXY(0,1);
				LCDputsf(sel);
				
			}
			else{
				WaterSensorType = 0;
				write_exteeprom_byte(WaterSensorType,water_type_ee_adr);
				LCDGotoXY(0,1);
				LCDputsf(desel);
			
			}
			break;
		case 6:
			
			clear_ir_flags();
			flag_ir.RotaryMain=1;
			flag_ir.Rotary_setting=1;
			RotarySetting();
			
			
			break;	
		case 7:
			if((ext_device & water_sensor)==0){
				
				ext_device |= water_sensor;
				write_exteeprom_dword(ext_device,ext_device_ee_adr);
				LCDGotoXY(0,1);
				LCDputsf(sel);
				
			}
			else{
				
				ext_device &= ~water_sensor;
				write_exteeprom_dword(ext_device,ext_device_ee_adr);
				LCDGotoXY(0,1);
				LCDputsf(desel);
			}
			
			break;
		}
		
		if(flag_ir.SetWaterLevelBar==1){
			sprintf((char*)SMS_text,"Level Bar =%s,  %d/%d/%d %d:%d\r\n",input_number_str,clock.year,clock.month,clock.day,clock.hour,clock.min);
			if(mmc_detect==0){
				f_mount(&SDFatFs,(TCHAR const*)SDPath,1);
				res=f_open(&File[0],"levelbar.txt",FA_WRITE | FA_OPEN_ALWAYS);
				if(res==FR_OK){
					f_lseek(&File[0],f_size(&File[0]));
					f_write(&File[0],SMS_text,strlen((char*)SMS_text),&FatBytesWritten);
					delayms(50);
					f_close(&File[0]);
					delayms(50);
	 
				}
				else{
					LCDclr();
					sprintf(buf,"log fail=%d",res);
					LCDputs(buf);
					delayms(1000);
				}
			}
			if(ext_device & gsm){
				
				if(SendSMS(ServerPhone,SMS_text)==1){
					delayms(1000);
					LCDclr();
					LCDputs("WATER LEVEL SENT OK!");
					delayms(2000);
				}
			}
			
		}
	}
    else if(command_ir==exit){
			if(flag_ir.SetWaterLevelBar==1){
				flag.disp = 0;
				display_count=4;
				flag.ir_flag=0;
				flag_ir.menu=0;
				clear_ir_flags();
			}
			else{
				clear_ir_flags();
				flag_ir.add_device_main=1;
				flag_ir.add_external_device=1;
				add_device();
      }
    }
  
  
	else if(flag_ir.SetWaterLevelBar==1){
		input_number();
	}
  
}
//-----------------------------------------------
void MicrosonicSetting(){
	
  if(flag_ir.MicrosonicMain==1){
    flag_ir.MicrosonicMain=0;
    key=4;
    start_key=4;
    end_key=9;
    strcpy(str[4],"Maximum Level");
    strcpy(str[5],"Minimum Level");
    strcpy(str[6],"Maximum Current");
    strcpy(str[7],"Minimum Current");
    strcpy(str[8],"Install Height");
    strcpy(str[9],"Sensor Select");
		LCDclr();
    LCDputs(str[4]);    
  }
  
  else if(command_ir==ok){
    
    switch(key){
    case 4:
      key=0;
      flag_ir.max_water=1;
      LCDGotoXY(0,1);
			sprintf(buf,"%05d",max_water_level);
      LCDputs(buf);
			strcpy(input_number_str,buf);
			LCDcursorOn();
			LCDGotoXY(0,1);
			
			number_limit=5;
			number_limit_index=0;
			flag_ir.in_number=1;
      return;
            
    case 5:
      key=0;
      flag_ir.min_water=1;
      LCDGotoXY(0,1);
			sprintf(buf,"%05d",min_water_level);
      LCDputs(buf);
			strcpy(input_number_str,buf);
			LCDcursorOn();
			LCDGotoXY(0,1);
			
			number_limit=5;
			number_limit_index=0;
			flag_ir.in_number=1;
      return;
      
      
    case 6:
      key=0;
      flag_ir.MaxCurrent=1;
      LCDGotoXY(0,1);
			sprintf(buf,"%f",read_microsonic_level());
      LCDputs(buf);
      
      return;
      
    case 7:
      key=0;
      flag_ir.MinCurrent=1;
      LCDGotoXY(0,1);
			sprintf(buf,"%f",read_microsonic_level());
      LCDputs(buf);
           
      return;
      
    case 8:
			key=0;
      flag_ir.sensor_level=1;
      LCDGotoXY(0,1);
			sprintf(buf,"%05d",SensorLevel);
      LCDputs(buf);
			strcpy(input_number_str,buf);
			LCDcursorOn();
			LCDGotoXY(0,1);
			
			number_limit=5;
			number_limit_index=0;
			flag_ir.in_number=1;
      return;
           
    case 9:
      if(WaterSensorType != microsonic){
        
        WaterSensorType = microsonic;
        write_exteeprom_byte(WaterSensorType,water_type_ee_adr);
        LCDGotoXY(0,1);
				LCDputsf(sel);
          
        }
        else{
          
          WaterSensorType = 0;
          write_exteeprom_byte(WaterSensorType,water_type_ee_adr);
          LCDGotoXY(0,1);
					LCDputsf(desel);
        }
      return;
      
    }
    
    //----------Save Value-----------
    
    if(flag_ir.max_water==1){
      flag_ir.max_water=0;
      max_water_level=strtolong(input_number_str);
      write_exteeprom_word(max_water_level,max_water_level_ee_adr);
      
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.MicrosonicMain=1;
      flag_ir.microsonic_setting=1;
      MicrosonicSetting();	
      
    }
    
    else if(flag_ir.min_water==1){
      flag_ir.min_water=0;
      min_water_level=strtolong(input_number_str);;
      write_exteeprom_word(min_water_level,min_water_level_ee_adr);
      
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.MicrosonicMain=1;
      flag_ir.microsonic_setting=1;
      MicrosonicSetting();		
      
    }
    else if(flag_ir.sensor_level==1){
      flag_ir.sensor_level=0;
      SensorLevel=strtolong(input_number_str);;
      write_exteeprom_word(SensorLevel,sensor_level_ee_adr);
      
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.MicrosonicMain=1;
      flag_ir.microsonic_setting=1;
      MicrosonicSetting();		
    }
    
    else if(flag_ir.MaxCurrent==1){
      Imax=read_microsonic_level();
      write_exteeprom_float(Imax,Imax_ee_adr);
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.MicrosonicMain=1;
      flag_ir.microsonic_setting=1;
      MicrosonicSetting();	
            
    }
    
    else if(flag_ir.MinCurrent==1){
      Imin=read_microsonic_level();
      write_exteeprom_float(Imin,Imin_ee_adr);
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.MicrosonicMain=1;
      flag_ir.microsonic_setting=1;
      MicrosonicSetting();
    
    }
  }
  
  //--------Input Value-----------
  else if(command_ir==exit){
    if(flag_ir.max_water==1 || flag_ir.min_water==1 || flag_ir.sensor_level==1 || flag_ir.MaxCurrent==1 || flag_ir.MinCurrent==1){
      clear_ir_flags();
      flag_ir.MicrosonicMain=1;
      flag_ir.microsonic_setting=1;
      MicrosonicSetting();
      
    }
    else{
      clear_ir_flags();
      flag_ir.water_sensor_setting=1;
      flag_ir.WaterSensorMain=1;
      WaterSetting();
    }
  }
	
  else if(flag_ir.max_water==1){
    input_number();
		    
  }
  
   else if(flag_ir.min_water==1){
    
		 input_number();
    
  }
  
  else if(flag_ir.sensor_level==1){
    input_number();
    	
  }
  
}
//----------------------------------------
void RainSetting(){
	if(flag_ir.RainSensorMain==1){
    flag_ir.RainSensorMain=0;
    key=4;
    start_key=4;
    end_key=8;
    strcpy(str[4],"TEMP LOW");
    strcpy(str[5],"TEMP HI");
		strcpy(str[6],"Sensor Outdoor");
    strcpy(str[7],"Sensor Indoor");
    strcpy(str[8],"SENSOR SELECT");
		LCDclr();
    LCDputs(str[4]);   
  }
  
  else if(command_ir==ok){
    
    switch(key){
    case 4:
      key=0;
      flag_ir.heater_temp_low=1;
			flag_ir.heater_temp_hi=0;
      LCDGotoXY(0,1);
			sprintf(buf,"%03d",T_low);
      LCDputs(buf);
			strcpy(input_number_str,buf);
			LCDcursorOn();
			LCDGotoXY(0,1);
						
			number_limit=3;
			number_limit_index=0;
			flag_ir.in_number=1;
      return;
            
    case 5:
      key=0;
      flag_ir.heater_temp_hi=1;
			flag_ir.heater_temp_low=0;
      LCDGotoXY(0,1);
			sprintf(buf,"%03d",T_hi);
      LCDputs(buf);
			strcpy(input_number_str,buf);
			LCDcursorOn();
			LCDGotoXY(0,1);
			number_limit=3;
			number_limit_index=0;
			flag_ir.in_number=1;
      return;
      
    case 6:
			heater_control=1;
			write_exteeprom_byte(heater_control,heater_control_ee_adr);
			LCDGotoXY(0,1);
			LCDputsf(sel);	
			break;
		case 7:
			heater_control=2;
			write_exteeprom_byte(heater_control,heater_control_ee_adr);
			LCDGotoXY(0,1);
			LCDputsf(sel);
			break;
		
    case 8:
      if((ext_device & rain_sensor)==0){
          
          ext_device |= rain_sensor;
          write_exteeprom_dword(ext_device,ext_device_ee_adr);
					rain_level=0;
					write_exteeprom_float(0,rain_level_ee_adr);
          LCDGotoXY(0,1);
					LCDputsf(sel);
          
        }
        else{
          
          ext_device &= ~rain_sensor;
          write_exteeprom_dword(ext_device,ext_device_ee_adr);
          LCDGotoXY(0,1);
					LCDputsf(desel);
        }
      return;
      
    }
    
    //----------Save Value-----------
    
    if(flag_ir.heater_temp_low==1){
      flag_ir.heater_temp_low=0;
      T_low=strtolong(input_number_str);
      write_exteeprom_byte(T_low,T_low_ee_adr);
      
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.RainSensorMain=1;
			flag_ir.Rain_sensor_setting=1;
      RainSetting();
            
    }
    
    else if(flag_ir.heater_temp_hi==1){
      flag_ir.heater_temp_hi=0;
      T_hi=strtolong(input_number_str);;
      write_exteeprom_byte(T_hi,T_hi_ee_adr);
      
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.RainSensorMain=1;
			flag_ir.Rain_sensor_setting=1;
      RainSetting();		
      
    }
    
  }
  
  //--------Input Value-----------
  else if(command_ir==exit){
    if(flag_ir.heater_temp_low==1 || flag_ir.heater_temp_hi==1 ){
      clear_ir_flags();
      flag_ir.RainSensorMain=1;
			flag_ir.Rain_sensor_setting=1;
      RainSetting();
      
    }
    else{
      clear_ir_flags();
			flag_ir.station_sett=1;
			flag_ir.st_set_main=1;
			st_setting();
    }
  }
	
  else if(flag_ir.heater_temp_low==1){
    input_number();
		    
  }
  
   else if(flag_ir.heater_temp_hi==1){
    
		 input_number();
    
  }
  
}
//---------------ADC Calibration-------------
void ADCcalib_routin(){
	if(flag_ir.adc_calib_main==1){
    flag_ir.adc_calib_main=0;
		key=4;
    start_key=4;
    end_key=9;
    strcpy(str[4],"ADC 1");
    strcpy(str[5],"ADC 2");
    strcpy(str[6],"ADC 3");
		strcpy(str[7],"ADC 4");
    strcpy(str[8],"ADC 5");
    strcpy(str[9],"Current4-20");
    
    LCDclr();
		LCDputs(str[4]); 
    
  }
	else if(command_ir==ok){
		 
    switch(key){
        
      case 4:
        key=0;
        flag_ir.adc1_calib=1;
        LCDGotoXY(0,1);
				strcpy(input_number_str,"0000");
				LCDputs(input_number_str);
				LCDcursorOn();
				LCDGotoXY(0,1);
						
				number_limit=4;
				number_limit_index=0;
				flag_ir.in_number=1;
        return;
      case 5:
        key=0;
        flag_ir.adc2_calib=1;
				LCDGotoXY(0,1);
				strcpy(input_number_str,"0000");
				LCDputs(input_number_str);
				LCDcursorOn();
				LCDGotoXY(0,1);
						
				number_limit=4;
				number_limit_index=0;
				flag_ir.in_number=1;
        return;
      case 6:
        key=0;
        flag_ir.adc3_calib=1;
				LCDGotoXY(0,1);
				strcpy(input_number_str,"0000");
				LCDputs(input_number_str);
				LCDcursorOn();
				LCDGotoXY(0,1);
						
				number_limit=4;
				number_limit_index=0;
				flag_ir.in_number=1;
			case 7:
        key=0;
        flag_ir.adc4_calib=1;
				LCDGotoXY(0,1);
				strcpy(input_number_str,"0000");
				LCDputs(input_number_str);
				LCDcursorOn();
				LCDGotoXY(0,1);
						
				number_limit=4;
				number_limit_index=0;
				flag_ir.in_number=1;
			case 8:
        key=0;
        flag_ir.adc5_calib=1;
				LCDGotoXY(0,1);
				strcpy(input_number_str,"0000");
				LCDputs(input_number_str);
				LCDcursorOn();
				LCDGotoXY(0,1);
						
				number_limit=4;
				number_limit_index=0;
				flag_ir.in_number=1;
			case 9:
        key=0;
        flag_ir.current_calib=1;
				LCDGotoXY(0,1);
				strcpy(input_number_str,"0000");
				LCDputs(input_number_str);
				LCDcursorOn();
				LCDGotoXY(0,1);
						
				number_limit=4;
				number_limit_index=0;
				flag_ir.in_number=1;
               
        return;
	  }
		
		 if(flag_ir.adc1_calib==1){
			memcpy(buf,input_number_str,2);
			buf[2]=0;
			adc_temp=strtolong(buf);
			memcpy(buf,input_number_str+2,2);
			adc_temp=adc_temp+(float)strtolong(buf)/100;
			
			ADC1Scale=READ_ADC(Ain1,Ain1_channel,1);
			ADC1Scale=adc_temp/ADC1Scale;
			
			write_exteeprom_float(ADC1Scale,ADC1Scale_ee_adr);
      write_exteeprom_byte(0,ADC1CalibOk);
			
			LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.adc_calib_main=1;
			flag_ir.ADCcalibration=1;
      ADCcalib_routin();	
		}
		
		else if(flag_ir.adc2_calib==1){
			memcpy(buf,input_number_str,2);
			buf[2]=0;
			adc_temp=strtolong(buf);
			memcpy(buf,input_number_str+2,2);
			adc_temp=adc_temp+(float)strtolong(buf)/100;
			
			ADC2Scale=READ_ADC(Ain2,Ain2_channel,1);
			ADC2Scale=adc_temp/ADC2Scale;
			
			write_exteeprom_float(ADC2Scale,ADC2Scale_ee_adr);
      write_exteeprom_byte(0,ADC2CalibOk);
			
			LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.adc_calib_main=1;
			flag_ir.ADCcalibration=1;
      ADCcalib_routin();

		}
		else if(flag_ir.adc3_calib==1){
			memcpy(buf,input_number_str,2);
			buf[2]=0;
			adc_temp=strtolong(buf);
			memcpy(buf,input_number_str+2,2);
			adc_temp=adc_temp+(float)strtolong(buf)/100;
			
			ADC3Scale=READ_ADC(Ain3,Ain3_channel,1);
			ADC3Scale=adc_temp/ADC3Scale;
			
			write_exteeprom_float(ADC3Scale,ADC3Scale_ee_adr);
      write_exteeprom_byte(0,ADC3CalibOk);
			
			LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.adc_calib_main=1;
			flag_ir.ADCcalibration=1;
      ADCcalib_routin();

		}
		
		else if(flag_ir.adc4_calib==1){
			memcpy(buf,input_number_str,2);
			buf[2]=0;
			adc_temp=strtolong(buf);
			memcpy(buf,input_number_str+2,2);
			adc_temp=adc_temp+(float)strtolong(buf)/100;
			
			ADC4Scale=READ_ADC(Ain4,Ain4_channel,1);
			ADC4Scale=adc_temp/ADC4Scale;
			
			write_exteeprom_float(ADC4Scale,ADC4Scale_ee_adr);
      write_exteeprom_byte(0,ADC4CalibOk);
			
			ADC4Scale= read_exteeprom_float(ADC4Scale_ee_adr);
			
			LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.adc_calib_main=1;
			flag_ir.ADCcalibration=1;
      ADCcalib_routin();

		}
		
		else if(flag_ir.adc5_calib==1){
			memcpy(buf,input_number_str,2);
			buf[2]=0;
			adc_temp=strtolong(buf);
			memcpy(buf,input_number_str+2,2);
			adc_temp=adc_temp+(float)strtolong(buf)/100;
			
			ADC5Scale=READ_ADC(Ain5,Ain5_channel,1);
			ADC5Scale=adc_temp/ADC5Scale;
			
			write_exteeprom_float(ADC5Scale,ADC5Scale_ee_adr);
      write_exteeprom_byte(0,ADC5CalibOk);
			
			LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.adc_calib_main=1;
			flag_ir.ADCcalibration=1;
      ADCcalib_routin();

		}
		
		else if(flag_ir.current_calib==1){
			memcpy(buf,input_number_str,2);
			buf[2]=0;
			adc_temp=strtolong(buf);
			memcpy(buf,input_number_str+2,2);
			adc_temp=adc_temp+(float)strtolong(buf)/100;
			
			currentScale=READ_ADC(Ain6,Ain6_channel,1);
			currentScale=adc_temp/currentScale;
			
			write_exteeprom_float(currentScale,currentScale_ee_adr);
      write_exteeprom_byte(0,currentCalibOk);
			
			LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.adc_calib_main=1;
			flag_ir.ADCcalibration=1;
      ADCcalib_routin();

		}
		
	}
	
	
	//--------Input Value-----------
  else if(command_ir==exit){
    if(flag_ir.adc1_calib==1 || flag_ir.adc2_calib==1 || flag_ir.adc3_calib==1 || flag_ir.adc4_calib==1 || flag_ir.adc5_calib==1 || flag_ir.current_calib==1){
      clear_ir_flags();
      flag_ir.adc_calib_main=1;
			flag_ir.ADCcalibration=1;
      ADCcalib_routin();
      
    }
    else{
      clear_ir_flags();
			flag_ir.station_sett=1;
			flag_ir.st_set_main=1;
			st_setting();
    }
  }
	
  else if(flag_ir.adc1_calib==1 || flag_ir.adc2_calib==1 || flag_ir.adc3_calib==1 || flag_ir.adc4_calib==1 || flag_ir.adc5_calib==1 || flag_ir.current_calib==1){
    input_number();
		    
  }
  
   
}
//----------------log setting--------------------
void log_setting(){
 
  if(flag_ir.log_first_page==1){
    flag_ir.log_first_page=0;
    flag_ir.log_prog=1;
    key=0;
    strcpy(str[4],"Log Time");
    		
    LCDclr();
		LCDputs(str[4]);
		
		flag_ir.log_interval=1;
		LCDGotoXY(0,1);
		strcpy(input_number_str,"0000");
		LCDputs(input_number_str);
		LCDcursorOn();
		LCDGotoXY(0,1);
				
		number_limit=4;
		number_limit_index=0;
		flag_ir.in_number=1;
    
  }
   
  else if(flag_ir.log_interval==1){
    
    if(command_ir==ok){
      
      log_time=strtolong(input_number_str);
      write_exteeprom_word(log_time,log_time_ee_adr);
//      next_log_time=(clock.hour+1)*60;
//			if(next_log_time>=1440){next_log_time=0; flag.log_disable=1;}
      
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
			flag_ir.st_set_main=1;
      st_setting();	
    }
    else if(command_ir==exit){
      clear_ir_flags();
      flag_ir.log_first_page=1;
      flag_ir.log_prog=1;
      log_setting();
    }
		
		input_number();
		
  }
  
}
//----------------------------------------------------
void loadSettingSD(){
	unsigned char *str_point=NULL,*str_point1=NULL;
	uint32_t filesize=0;
	
	mmc_pwr_on;
	f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
	
	if(f_open(&File[0],"setting.txt",FA_READ)!=FR_OK) {
		LCDclr();
		LCDputs("File Open Fail");
    delayms(1000);
    mmc_pwr_off;
    return;
  }
	filesize=f_size(&File[0]);
	if(f_read(&File[0],MemBuf,filesize,&FatBytesRead) != FR_OK){
		LCDclr();
		LCDputs("File Read Fail");
    delayms(1000);
    mmc_pwr_off;
    return;
	}
	//-------------------------------------------------
	if((str_point=StrSearch(MemBuf,"extdevices=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		ext_device=strtolong(str_point+11);
		write_exteeprom_dword(ext_device,ext_device_ee_adr);
		ext_device=read_exteeprom_dword(ext_device_ee_adr);
		
	}
	//----------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"temptype=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		temp_type=strtolong(str_point+9);
		write_exteeprom_dword(temp_type,temp_type_ee_adr);
		temp_type=read_exteeprom_dword(temp_type_ee_adr);
		
	}
	
	//----------------------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"watersensortype=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		WaterSensorType=strtolong(str_point+16);
		write_exteeprom_byte(WaterSensorType,water_type_ee_adr);
		WaterSensorType=read_exteeprom_byte(water_type_ee_adr);
	}
	//-------------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"deltawlvl=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		delta_water_level=strtolong(str_point+10);
		write_exteeprom_word(delta_water_level,DeltaWlvlEEadr);
		delta_water_level=read_exteeprom_word(DeltaWlvlEEadr);
		
	}
	//----------------------------------------------------
	if((str_point=StrSearch(MemBuf,"Thi=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		T_hi=strtolong(str_point+4);
		write_exteeprom_byte(T_hi,T_hi_ee_adr);
		T_hi=read_exteeprom_byte(T_hi_ee_adr);
	}
	//------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"Tlow=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		T_low=strtolong(str_point+5);
		write_exteeprom_byte(T_low,T_low_ee_adr);
		T_low=read_exteeprom_byte(T_low_ee_adr);
	}
	
	//------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"raintype=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		rain_sensor_type=strtolong(str_point+9);
		write_exteeprom_byte(rain_sensor_type,rain_type_ee_adr);
		rain_sensor_type=read_exteeprom_byte(rain_type_ee_adr);
	}
	
	//------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"raineventtimeout=",filesize))!=NULL){
    str_point+=17;
    str_point1=CharSearch(str_point,',',filesize);
    *str_point1=0;
    RainEventTime=strtolong(str_point);
    
    write_exteeprom_word(RainEventTime,RainEventTime_ee_adr);
    
  }
  //---------------------------------------------------------------------
  if((str_point=StrSearch(MemBuf,"rainzerotime=",filesize))!=NULL){
    str_point+=13;
    str_point1=CharSearch(str_point,':',filesize);
    
    *str_point1=0;
    RainRstTime=(strtolong(str_point))*60;
    str_point=CharSearch(str_point1,',',filesize);
		*str_point=0;
		RainRstTime+=strtolong(str_point1+1);
		
    write_exteeprom_word(RainRstTime,RainRstTime_ee_adr);
    RainRstTime=read_exteeprom_word(RainRstTime_ee_adr);
		
  }
  //----------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"deltarain=",filesize))!=NULL){
		str_point1=CharSearch(str_point,'.',filesize);
		*str_point1=0;
		delta_rain_level=(float)strtolong(str_point+10);
		str_point=CharSearch(str_point1,',',filesize);
		*str_point=0;
		delta_rain_level+=(((float)strtolong(str_point1+1))/100);
		
		write_exteeprom_float(delta_rain_level,DeltaRainLevelEEadr);
		delta_rain_level=read_exteeprom_float(DeltaRainLevelEEadr);
		
	}
	//------------------------------------------------
	if((str_point=StrSearch(MemBuf,"rainlevel=",filesize))!=NULL){
		str_point1=CharSearch(str_point,'.',filesize);
		*str_point1=0;
		rain_level=(float)strtolong(str_point+10);
		str_point=CharSearch(str_point1,',',filesize);
		*str_point=0;
		rain_level+=(((float)strtolong(str_point1+1))/100);
		
		write_exteeprom_float(rain_level,rain_level_ee_adr);
		rain_level=read_exteeprom_float(rain_level_ee_adr);
		
	}
	
	//--------------------------------------------------------
	if((str_point1=StrSearch(MemBuf,"rtcsett=",filesize))!=NULL){
        
    str_point=StrSearch(str_point1,"year:",filesize);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      clock.year=strtolong(str_point+5);
      
    }
    
    str_point=StrSearch(str_point1,"mon:",filesize);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      clock.month=strtolong(str_point+4);
    }
    
    
    str_point=StrSearch(str_point1,"day:",filesize);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      clock.day=strtolong(str_point+4);
    }
    
    
    str_point=StrSearch(str_point1,"hour:",filesize);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      clock.hour=strtolong(str_point+5);
    }
    
    
    str_point=StrSearch(str_point1,"min:",filesize);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      clock.min=strtolong(str_point+4);
    }
        
    rtc_settime_shamsi(&clock);
    
    rtc_gettime_shamsi(&clock);
    
    
  }
  	
//--------------------------------------------------------------

	if((str_point=StrSearch(MemBuf,"chargereq=",filesize))!=NULL){
        
    str_point+=10;
    str_point1=CharSearch(str_point,',',filesize);
		if(str_point1 != NULL){
			*str_point1=0;
			strcpy(ChargeReqStr,str_point);
			write_exteeprom_ary(ChargeReqStr,strlen(ChargeReqStr)+1,ChargeReqStr_ee_adr);
			read_exteeprom_ary(ChargeReqStr,strlen(ChargeReqStr)+1,ChargeReqStr_ee_adr);
		}
						
  }	
//-------------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"centerphone=",filesize))!=NULL){
    str_point+=12;
    str_point1=CharSearch(str_point,',',filesize);
    
    *str_point1=0;
    strcpy(ServerPhone,str_point);
    
    write_exteeprom_ary(ServerPhone,strlen(ServerPhone)+1,server_phone_ee_adr);
    
    read_exteeprom_ary(ServerPhone,29,server_phone_ee_adr); 
        
  } 
  
	//-----------------------------------------------------
	if((str_point=StrSearch(MemBuf,"smssendtime=",filesize))!=NULL){
		str_point1=CharSearch(str_point,':',filesize);
		*str_point1=0;
		SmsSendTime=(strtolong(str_point+12))*60;
			
    str_point=CharSearch(str_point1,',',filesize);
		*str_point=0;
		SmsSendTime+=strtolong(str_point1+1);
		
		write_exteeprom_word(SmsSendTime,SmsSendTime_ee_adr);
		SmsSendTime=read_exteeprom_word(SmsSendTime_ee_adr);
				
	}
//-------------------------------------------------------------------------
   
  if((str_point=StrSearch(MemBuf,"ftpurl=",filesize))!=NULL){
        
		str_point1=CharSearch(str_point1,',',filesize);
		*str_point1=0;
		strcpy(server_url,str_point+7);
		write_exteeprom_ary(server_url,strlen(server_url)+1,server_url_ee_adr);
		write_exteeprom_byte(0,server_changed1_ee_adr);
	}
//-----------------------------------------------------------    
    
  if((str_point=StrSearch(MemBuf,"ftpuname=",filesize))!=NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      strcpy(server_username,str_point+9);
			write_exteeprom_ary(server_username,strlen(server_username)+1,server_username_ee_adr);
			write_exteeprom_byte(0,server_changed1_ee_adr);
	}
//--------------------------------------------------------    
    
    if((str_point=StrSearch(MemBuf,"ftppass=",filesize))!=NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      strcpy(server_password,str_point+8);
			write_exteeprom_ary(server_password,strlen(server_password)+1,server_password_ee_adr);
			write_exteeprom_byte(0,server_changed1_ee_adr);
		}
    
         
  //-------------------------------------------------------------------------------
 if((str_point=StrSearch(MemBuf,"ftpfilename=",filesize))!=NULL){
        
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		strcpy(server_filename,str_point+12);
		write_exteeprom_ary(server_filename,strlen(server_filename)+1,server_filename_ee_adr);
		write_exteeprom_byte(0,server_changed2_ee_adr);
  }
    
  //---------------------------------------------------
  if((str_point=StrSearch(MemBuf,"ftppath=",filesize))!=NULL){
        
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		strcpy(server_path,str_point+8);
				
		write_exteeprom_ary(server_path,strlen(server_path)+1,server_path_ee_adr);
		write_exteeprom_byte(0,server_changed2_ee_adr);
		
  }
	//---------------------------------------------------
  if((str_point=StrSearch(MemBuf,"apn=",filesize))!=NULL){
        
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		strcpy(apn_name,str_point+4);
		write_exteeprom_ary(apn_name,strlen(apn_name)+1,apn_name_ee_adr);
		write_exteeprom_byte(0,server_changed2_ee_adr);
		
  }
   
	
//-----------------------------------------------------------------
   
	if((str_point=StrSearch(MemBuf,"logtime=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		log_time=strtolong(str_point+8);
		write_exteeprom_word(log_time,log_time_ee_adr);
		log_time=read_exteeprom_word(log_time_ee_adr);
		next_log_time=(clock.hour*60)+clock.min+log_time;
    if(next_log_time>=1440){next_log_time=next_log_time%1440; flag.log_disable=1;}
		
	}
	
	//---------------------------------------------
	if((str_point=StrSearch(MemBuf,"LogParameters=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		LogParameters=strtolong(str_point+14);
		write_exteeprom_word(LogParameters,LogParametersEEadr);
				
	}
	//---------------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"adminmobile1=",filesize))!=NULL){
		str_point+=13;
    str_point1=CharSearch(str_point,',',filesize);
    
    *str_point1=0;
    
    write_exteeprom_ary(str_point,strlen(str_point)+1,AdminMobile1_ee_adr);
        				
	}
	//---------------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"adminmobile2=",filesize))!=NULL){
		str_point+=13;
    str_point1=CharSearch(str_point,',',filesize);
    
    *str_point1=0;
    
    write_exteeprom_ary(str_point,strlen(str_point)+1,AdminMobile2_ee_adr);
        				
	}
	//---------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"fileuploadinterval=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		gprs_upload_time=strtolong(str_point+19);
		write_exteeprom_word(gprs_upload_time,gprs_upload_time_ee_adr);
				
	}
	//---------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"gprsautoupload=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		gprs_auto_upload=strtolong(str_point+15);
		write_exteeprom_word(gprs_auto_upload,gprs_auto_upload_ee_adr);
				
	}
	//---------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"datauploadtimes=",filesize))!=NULL){//uploadtimes=t0,t1,t2,t3,t4,
		
		str_point+=16;
		for(i=0;i<5;i++){
			str_point1=CharSearch(str_point,',',5);
			if(str_point1!=NULL){
				*str_point1=0;
				upload_time_table[i]=strtolong(str_point);
				write_exteeprom_word(upload_time_table[i],upload_time_table0_ee_adr+(i*2));
				str_point = str_point1+1;
			}
			
		}
				
	}
	
	//---------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"ftppathseparator=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		ftp_path_separator=*(str_point+17);
		write_exteeprom_byte(ftp_path_separator,ftp_path_separator_ee_adr);
				
	}
	//---------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"stname=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		strcpy(station_name,str_point+7);
		write_exteeprom_ary(station_name,strlen(station_name)+1,station_name_ee_adr);
				
	}
		
}
//----------------------------------------------------
void RotarySetting(){
	if(flag_ir.RotaryMain==1){
    flag_ir.RotaryMain=0;
    key=4;
    start_key=4;
    end_key=7;
    strcpy(str[4],"Resolusion");
    strcpy(str[5],"Circumference");
		strcpy(str[6],"Set Level");
		strcpy(str[7],"Sensor Select");
    LCDclr();
    LCDputs(str[4]);    
  }
	else if(command_ir==ok){
    
    switch(key){
    case 4:
      key=0;
      flag_ir.rotary_resolution=1;
      LCDGotoXY(0,1);
			sprintf(buf,"%05d",RotaryResolution/2);
      LCDputs(buf);
			strcpy(input_number_str,buf);
			LCDcursorOn();
			LCDGotoXY(0,1);
			
			number_limit=5;
			number_limit_index=0;
			flag_ir.in_number=1;
      return;
            
    case 5:
      key=0;
      flag_ir.rotary_diameter=1;
      LCDGotoXY(0,1);
			sprintf(buf,"%05d",RotaryDiameter);
      LCDputs(buf);
			strcpy(input_number_str,buf);
			LCDcursorOn();
			LCDGotoXY(0,1);
			
			number_limit=5;
			number_limit_index=0;
			flag_ir.in_number=1;
      return;
		case 6:
			key=0;
      flag_ir.rotary_set_level=1;
      LCDGotoXY(0,1);
			sprintf(buf,"%05d",water_level);
      LCDputs(buf);
			strcpy(input_number_str,buf);
			LCDcursorOn();
			LCDGotoXY(0,1);
			
			number_limit=5;
			number_limit_index=0;
			flag_ir.in_number=1;
			return;
		case 7:
			if(WaterSensorType != rotary_encoder){
        RotaryScale=RotaryResolution/RotaryDiameter;
        WaterSensorType = rotary_encoder;
        write_exteeprom_byte(WaterSensorType,water_type_ee_adr);
        LCDGotoXY(0,1);
				LCDputsf(sel);
          
        }
        else{
          
          WaterSensorType = 0;
          write_exteeprom_byte(WaterSensorType,water_type_ee_adr);
          LCDGotoXY(0,1);
					LCDputsf(desel);
        }
      return;
          
    }
    
    //----------Save Value-----------
    
    if(flag_ir.rotary_resolution==1){
      flag_ir.rotary_resolution=0;
      RotaryResolution=strtolong(input_number_str);
			RotaryResolution*=2;
      write_exteeprom_word(RotaryResolution,RotaryResolution_ee_adr);
      RotaryScale=RotaryResolution/RotaryDiameter;
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.RotaryMain=1;
      flag_ir.Rotary_setting=1;
      RotarySetting();	
      
    }
    
    else if(flag_ir.rotary_diameter==1){
      flag_ir.rotary_diameter=0;
      RotaryDiameter=strtolong(input_number_str);;
      write_exteeprom_word(RotaryDiameter,RotaryDiameter_ee_adr);
      RotaryScale=RotaryResolution/RotaryDiameter;
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.RotaryMain=1;
      flag_ir.Rotary_setting=1;
      RotarySetting();		
      
    }
		else if(flag_ir.rotary_set_level==1){
      flag_ir.rotary_set_level=0;
      water_level=strtolong(input_number_str);;
      write_exteeprom_word(water_level,water_level_ee_adr);
			
      TIM1->CNT=water_level*RotaryScale;
			
      LCDclr();
      LCDputsf(sel);
      delayms(500);
      clear_ir_flags();
      flag_ir.RotaryMain=1;
      flag_ir.Rotary_setting=1;
      RotarySetting();		
      
    }
    
  }
  
  //--------Input Value-----------
  else if(command_ir==exit){
    if(flag_ir.rotary_resolution==1 || flag_ir.rotary_diameter==1 || flag_ir.rotary_set_level==1){
      clear_ir_flags();
      flag_ir.RotaryMain=1;
      flag_ir.Rotary_setting=1;
      RotarySetting();
      
    }
    else{
      clear_ir_flags();
      flag_ir.water_sensor_setting=1;
      flag_ir.WaterSensorMain=1;
      WaterSetting();
    }
  }
	
  else if(flag_ir.rotary_resolution==1 || flag_ir.rotary_set_level==1 || flag_ir.rotary_diameter==1){
    input_number();
		    
  }
  
   
}