/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "usb_device.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>

#include "lcdlib.h"
#include "my_delay.h"
#include "defines.h" 
#include "eeprom_ext.h"
#include "rtc.h"   
#include "gsm.h"
#include "main_func.h"
#include "usart.h"
#include "temperature.h"
#include "timeout.h"
#include "ADXL345.h"
#include "main_func.h"
#include "wifi.h"
#include "ir.h"
#include "camera.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc3;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

IWDG_HandleTypeDef hiwdg;

SD_HandleTypeDef hsd;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
const uint32_t station_address=500;
char station_name[30]="MARV ELECTRONIC";
const char firmware_version[]="V4.01";

#ifdef marv
const char title_str[]="     MARV RTU";
#endif

#ifdef saramin
const char title_str[]="    SARAMIN RTU";
#endif

ADC_ChannelConfTypeDef adConfig;

volatile Rtc clock,cur_clock;

// server listen port for www

#define MYWWWPORT 80

// global packet buffer

#define BUFFER_SIZE 550


//static uint8_t tcp_buf[BUFFER_SIZE+1];

// Counter & Index Var ------------------------
volatile uint8_t play_count, alarm_count, resend,len,display_count,iic_count,rec_cou,rf_pwr_count,
time_out,time_out_pass,send_time,p_rec_cou,p_len,loop_timeout,ir_cou,iic_pwr_count,
fc_clr_time,next_msg,usart_rain_cou,ReadWaterTime,log_period,minute_changed,iic_busy_timeout;


volatile uint16_t send_count,count,rain_cou,wakeup_cou,i,j,wind_timeout,test_mode_timeout,temp_time;  

//Normal Var------------------------------------------------------------  
volatile uint8_t st_type,pwr_existence,
fc_on_time,fc_off_time,cur_minute,save_door_state,
valid_door_flag,snow_mtr_time=11,cur_resend,rain_sensor_type,fc_delay,temp_type,
sound_number,WindSensorType;


volatile uint16_t password,input_pass,alarm,log_time,next_log_time,sample_count,start_time,stop_time,
return_time=60,WaterSensorTime,DisplayTime,wind_dir,cur_wind_dir,wind_speed,cur_wind_speed,LogParameters,
time24;


volatile uint32_t write_adr,read_adr,data_ir,button[20],ext_device,AirPressure,cur_air_pressure;

volatile float wind_time,WindAlpha,WindBeta,AirAlpha,AirBeta,utp,ltp;
//-------------Rain Sensor Var--------------------------
volatile float rain_level,cur_rain_level,last_rain_level,delta_rain_level,rain_time,minute_rain;
volatile float rain_register[20],rain_sum;
volatile uint8_t rain_register_index;
volatile uint16_t RainRstTime,RainRstFlag;
volatile uint16_t RainEventTime=900,minute_rain_cou;

volatile uint8_t heater_control;
signed char heater_temp;
//----------------Analog input var--------------------
float ADC1Scale,ADC2Scale,ADC3Scale,ADC4Scale,ADC5Scale,currentScale;
//---------water sensor var--------------------------
unsigned char WaterSensorType;
volatile int16_t  water_level,last_water_level,delta_water_level,cur_water_level_display,max_water_level,
min_water_level,SensorLevel,RotaryResolution,RotaryDiameter,RotaryScale,cur_water_level;
float Imin,Imax;
uint16_t volatile SetLevelBar_count,SetLevelBar_timeout;
//-------------------wifi--------------------------------
uint8_t wifi_rec_buffer[250],wifi_send_buffer[250];
volatile uint8_t wifi_rec_index;
//-----------Camera Module Variables------------- 
volatile unsigned int PictureSize,PictureSizeCounter,TakePictureCount,CamRecLen,
camera_log_time;
unsigned char CameraBuffer[550],CameraBuffer1[550],MemBuf[60000]/*1310*/,*CameraBufPointer=CameraBuffer;
volatile unsigned char CamBufOvf,PictureRecProcess,rs485_select;

unsigned char gprs_first_connect,gprs_auto_upload;

unsigned char server_url[50],server_username[30],server_password[20],server_filename[50],server_path[100],apn_name[30];
unsigned char temp_filename[40];

char camera_reset[4]={0x56,0,0x26,0};
char camera_TakePicture[5]={0x56,0,0x36,0x1,0};
char camera_ReadFileSize[5]={0x56,0,0x34,0x1,0};
char camera_ReadFileContent[16]={0x56,0,0x32,0xc,0,0xa,0,0,0,0,0,0,0,0,0,0xa};
char camera_ChangePicSizeLarge[5]={0x56,0,0x54,1,0};
char camera_ChangePicSizeSmall[5]={0x56,0,0x54,1,0x11};
char camera_changeboud[7]={0x56,0,0x24,3,1,0xAE,0xC8};

//--------bmp variabls-----------
volatile int16_t AC1,AC2,AC3,B1,B2,MB,MC,MD;
volatile uint16_t AC4,AC5,AC6;
volatile int32_t UP=0,UT=0,B3,B4=0,B5=0,B6=0,B7=0,X1=0,X2=0,X3=0,T=0;
volatile float P;

/////ADXL345 --------------------------------------------
uint16_t Accel_Xout_Val=0, Accel_Yout_Val=0, Accel_Zout_Val=0;
int16_t Accel_Xout_Normalized_Val=0, Accel_Yout_Normalized_Val=0, Accel_Zout_Normalized_Val=0;
uint8_t Offset_X_Val=0,Offset_Y_Val=0,Offset_Z_Val=0;
float Accel_XAngle=0, Accel_YAngle=0, Accel_ZAngle=0;

//***---------------temperature & humidity sensor ------------------
signed char temp_in,cur_temp_in,temp_out,cur_temp_out,temp_difference,T_low,T_hi;
uint8_t sht_rec_buffer[15],sht_send_buffer[5],temp_rec_len,humidity;
//------------thalimedes --------------
unsigned char thali_buf[20];
//buffer Definition ***********************

uint8_t p_rec_buf[15],p_send_bufer[15],fc301d_send_buffer[255],fc301d_rec_buffer[255],
log_buffer[100],base_address[30],log_header[20];

uint8_t usart2_rx[2],usart3_rx[2];
//uint16_t log_avg[40];
uint8_t buf[255];
//-------gsm module---------------
uint8_t SimSendBuffer[255],SimRecBuffer[255],ServerPhone[30],RemotePhone[30]
,SMS_text[255],ChargeReqStr[30],gprs_buffer[250];
uint8_t AdminMobile[30];
volatile uint8_t gsm_off_time,SmsSendFlag,CUSD_cnt;
volatile int8_t gsm_signal,SMS_number,ftp_path_separator;
volatile uint16_t SimCount,SmsSendTime,SimRecLen,user_sms_count,user_sms_timeout,gprs_upload_time;
volatile uint16_t upload_time_table[5];
//--------keypad string-----------
char input_number_str[20];
uint8_t number_limit,number_limit_index;
//pointer-------------------------
uint8_t *rec_data,*data,**endptr;
uint8_t *sms_pointer,*sms_pointer1,*str_point;
//FatFs variable definition---------------------

FATFS SDFatFs;  /* File system object for SD card logical drive */
uint8_t SDPath[4]; /* SD card logical drive path */

FRESULT res;                                          /* FatFs function common result code */

FATFS Fatfs[_VOLUMES];		/* File system object for each logical drive */
FIL File[2];				/* File object */
DIR dir[5];
UINT FatBytesWritten;
UINT FatBytesRead;

//Structures & Bit Variables -----------------------------------------------------------------
#pragma anon_unions
union {
  
  uint8_t msg;
  struct {
    
    uint8_t num:7;
    uint8_t device:1;
  }; 
}volatile play_buffer[20];



//Flag Defines-------------------
//#pragma anon_unions
struct{
  
  struct {
    uint16_t open_door:1;
    uint16_t main_menu:1;
    uint16_t ir_flag:1;
    uint16_t comm:1;
    uint16_t ack:1;
    uint16_t ply_alarm:1;
    uint16_t rob:1;
    uint16_t wakeup:1;
    uint16_t door:1;
    uint16_t temp_control:1;
    uint16_t fc_net_busy:1;
    uint16_t LcdOn:1;
    uint16_t bmp180_timeout:1;
    uint16_t pcf_timeout:1;
    uint16_t disp:1;
  };
  
  struct{
    uint16_t read_state:1;
    uint16_t config_pwr:1;
    uint16_t pwr_config_read:1;
    uint16_t read_log:1;
    uint16_t config_rtc:1;
    uint16_t rtc_read:1;
    uint16_t config_log_time:1;
    uint16_t log_time_read:1;
    uint16_t config_fc_cycle:1;
    uint16_t fc_cycle_read:1;
    uint16_t config_send_time:1;
    uint16_t send_time_read:1;
    uint16_t rtcset:1;
    uint16_t dmatranfercplt:1;
    
    
  };
  struct{		
    uint16_t fc301d_reply_cmd:1;
    uint16_t display_level:1;
    uint16_t aburt:1;
    uint16_t spi_rec:1;
    uint16_t input_init_pass:1;
    uint16_t r:1;
    uint16_t detect_rain:1;
    uint16_t start_rain_event:1;
    uint16_t rec_rain_usart:1;
    uint16_t refresh:1;
    uint16_t rtc_update:1;
    uint16_t log_disable:1;
    uint16_t ir_learn:1;
    uint16_t WaterSensorOn:1; 
		uint16_t test_mode:1;
		uint16_t i2c_busy:1;
    
  };
	struct{		
    uint16_t UserCheck:1;
    uint16_t SetWaterLevelBar:1;
		uint16_t min_changed:1;
		uint16_t log_start:1;
		uint16_t stop_rain:1;
		uint16_t min_rain_check:1;
		uint16_t log_picture:1;
		uint16_t levelevent:1;
		uint16_t menu_setting:1;
		uint16_t alarm_disable:1;
		uint16_t upload_log:1;
		uint16_t upload_in_schedule:1;
		
  };
  
  
}volatile flag;



//commands : {length,cmd}

typedef struct  
{uint8_t length; uint8_t cmd;}command;


const command 
wakeup = {0,200},

play_alarm={0,21},

stop_alarm={0,22},

hozur_taki={0,42},

acknlg={0,43},

send_data={19,47},

read_mem={9,45},
time_stamp={0,49},

power_config={5,54},
read_power_config={0,55},
send_power_config={5,56},

rtc_config={6,57},
send_rtc={6,58},
read_rtc={0,59},


read_fc_time={0,64},
send_fc_time={2,65},
fc_time_config={2,66},

read_send_time={0,67},
send_time_config={1,68},
transmit_send_time={1,69},

log_time_config={2,70},
send_log_time={2,71},
read_log_time={0,72},

end_pack={0,73},
aburt_process={0,74},

change_server_phone={12,75},
server_phone_reply={12,76},
server_phone_read={0,77},

read_data_ascii={0,78},

snowmotor={0,79};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC3_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM6_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC3_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART3_UART_Init();
  MX_IWDG_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();

  /* USER CODE BEGIN 2 */
	
  delayms(1000);
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Base_Start(&htim6);
	
//	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	HAL_NVIC_DisableIRQ(TIM4_IRQn);//disable key input
		
	rtc_init();	
		
	rtc_gettime_shamsi(&clock);
	cur_clock=clock;
		
	lcd_pwr_on;
	LCDinit();
	LCDclr();
	LCDcursorOFF();
	LCDputs("Boot Device ");
	valid_door_flag=1;
	flag.main_menu=1;
	flag.open_door=1;
	flag.door=1;
	flag.LcdOn=1;
	delayms(1000);
	

//	if(mmc_detect==0){
		
		f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
		sprintf((char*)log_buffer,"%04d_%02d.txt",clock.year,clock.month);
		res=f_open(&File[0],log_buffer,FA_WRITE | FA_OPEN_ALWAYS);
		
		LCDclr();
		if(res==0){
			LCDputs("SD Card Mount OK");
			f_close(&File[0]);
			
		}
		else{
			sprintf(buf,"SD Card Fail %d",res);
			LCDputs(buf);
		}
	
		delayms(1000);
//	}
	
		 
//	__enable_irq();
//	__disable_irq();
  /*adc config chanel variable */
  
  adConfig.Rank = 1;
  adConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  //------------------station name-------------------------------
	if(read_exteeprom_byte(station_name_ee_adr)!=0xFF){
		read_exteeprom_ary((unsigned char*)station_name,30,station_name_ee_adr);
				
	}
			
  //--------------------if external device connected--------------
  ext_device=read_exteeprom_dword(ext_device_ee_adr);
  
  if(ext_device==0xFFFFFFFF){
    ext_device=0;
    write_exteeprom_dword(ext_device,ext_device_ee_adr);
    
  }
  else {
		ext_device |= gsm;//  gsm modem is fixed in device
				
    if((ext_device&temp_sensor)==temp_sensor){ 
      temp_type=read_exteeprom_byte(temp_type_ee_adr);
      if(temp_type & PT100){
        temp_difference=read_exteeprom_byte(temp_difference_ee_adr);
        analog_pwr_on;
      }
    }  
    
    if(ext_device & water_sensor){
      WaterSensorType=read_exteeprom_byte(water_type_ee_adr);
      if(WaterSensorType==microsonic){
        
        max_water_level=read_exteeprom_word(max_water_level_ee_adr);
        min_water_level=read_exteeprom_word(min_water_level_ee_adr);
        SensorLevel=read_exteeprom_word(sensor_level_ee_adr);
        //      Imax=read_exteeprom_float(Imax_ee_adr);
        //      Imin=read_exteeprom_float(Imin_ee_adr);
        
        
      }
			if(WaterSensorType == rotary_encoder){
				RotaryResolution=read_exteeprom_word(RotaryResolution_ee_adr);
				RotaryDiameter=read_exteeprom_word(RotaryDiameter_ee_adr);
				if(RotaryResolution==0xFFFF) RotaryResolution=200;//Pulse
				if(RotaryDiameter==0xFFFF) RotaryDiameter=20;//cm
				RotaryScale=RotaryResolution/RotaryDiameter;
				water_level=(int16_t)read_exteeprom_word(water_level_ee_adr);
				if(water_level>1000 || water_level<(0-1000))water_level=0;
				TIM1->CNT=water_level*RotaryScale;
				cur_water_level=water_level;
			}
			
			last_water_level=water_level;	
			cur_water_level_display=water_level;
			delta_water_level=read_exteeprom_word(DeltaWlvlEEadr);
			
    }
    
    if(ext_device & rain_sensor){
      T_hi=read_exteeprom_byte(T_hi_ee_adr);
      T_low=read_exteeprom_byte(T_low_ee_adr);
			
			rain_sensor_type=read_exteeprom_byte(rain_type_ee_adr);
     
      rain_level=read_exteeprom_float(rain_level_ee_adr);
      if(rain_level>100 || rain_level<0)rain_level=0;
      
      RainRstTime=read_exteeprom_word(RainRstTime_ee_adr);
            
      RainEventTime=read_exteeprom_word(RainEventTime_ee_adr);
      if(RainEventTime>30000)RainEventTime=900;
			
			if(read_exteeprom_dword(DeltaRainLevelEEadr)!=0xFFFFFFFF){
				delta_rain_level=read_exteeprom_float(DeltaRainLevelEEadr);
      }
			else
				delta_rain_level=1000;//unreachable value
						
			last_rain_level=rain_level;
      cur_rain_level=rain_level;      
			
			heater_control = read_exteeprom_byte(heater_control_ee_adr);
    }
    
    if(ext_device & gsm){
			LCDclr();
			LCDputs("GSM Init...");
			GsmInit();
      read_exteeprom_ary(ServerPhone,29,server_phone_ee_adr); 
			read_exteeprom_ary(ChargeReqStr,29,ChargeReqStr_ee_adr);
      gsm_off_time=clock.hour;
			SmsSendTime=read_exteeprom_word(SmsSendTime_ee_adr);
			
			gprs_upload_time=read_exteeprom_word(gprs_upload_time_ee_adr);
			gprs_auto_upload=read_exteeprom_word(gprs_auto_upload_ee_adr);
			
			upload_time_table[0]=read_exteeprom_word(upload_time_table0_ee_adr);
			upload_time_table[1]=read_exteeprom_word(upload_time_table1_ee_adr);
			upload_time_table[2]=read_exteeprom_word(upload_time_table2_ee_adr);
			upload_time_table[3]=read_exteeprom_word(upload_time_table3_ee_adr);
			upload_time_table[4]=read_exteeprom_word(upload_time_table4_ee_adr);
			
			ftp_path_separator=read_exteeprom_byte(ftp_path_separator_ee_adr);
			
//-------------------read ftp addrress------------------------
			 if(read_exteeprom_byte(server_changed1_ee_adr)==0){
				
				read_exteeprom_ary(server_url,29,server_url_ee_adr);
				read_exteeprom_ary(server_username,29,server_username_ee_adr);
				read_exteeprom_ary(server_password,19,server_password_ee_adr);
				
			}
			
			else{
				strcpy(server_url,"files.000webhost.com");
				strcpy(server_username,"merlin66");
				strcpy(server_password,"8724110316"); 
						
			}
			
			if(read_exteeprom_byte(server_changed2_ee_adr)==0){
						
				read_exteeprom_ary(server_filename,49,server_filename_ee_adr);
				read_exteeprom_ary(server_path,49,server_path_ee_adr);
				read_exteeprom_ary(apn_name,30,apn_name_ee_adr);
			}
			
			else{
				
				strcpy(server_filename,"MarvPicture.jpg");
				strcpy(server_path,"/");
				strcpy(apn_name,"MTNIRANCELL");
				
			}
			
			str_point=server_path;
			while((*str_point) != 0){
				if((*str_point) == '*')(*str_point)=ftp_path_separator;
				str_point++;
				
			}
			
			sprintf(buf,"%06d%c",station_address,ftp_path_separator);
			strcat(server_path,buf);
			
		}

    
		if(ext_device & wifi){
			delayms(1000);
			LCDclr();
			LCDputs("WIFI Init...");
			wifi_init();
		}
		
    if(ext_device & wind_sensor){
      WindSensorType=read_exteeprom_byte(wind_type_ee_adr);
      
      
      if(WindSensorType == wind_omc160){
        //init usart for omc
        
      }
      
    }
            
    
  }
  
  // station_address=read_exteeprom_word(station_ee_adr);
  if(read_exteeprom_byte(50)==0)
    read_exteeprom_ary(base_address,30,base_ee_adr);
  
  else{ 
    for(i=0;i<30;i++) base_address[i]=0;
    write_exteeprom_ary(base_address,30,base_ee_adr);
    write_exteeprom_byte(0,50);
  }
  
  if(base_address[0]!=0)  fc_delay = base_address[0];
  else fc_delay=2;
  
  st_type=read_exteeprom_byte(st_type_ee_adr);
  
  
  //if password changed from defult password address 64000 would be 0 else it would be 255 and password would be defult password--
  if(read_exteeprom_byte(32)==255){ 
    password=defult_password;
    write_exteeprom_word(password,password_ee_adr);
    write_exteeprom_byte(0,32);
    
  }
  else if(read_exteeprom_byte(32)==0)
    password = read_exteeprom_word(password_ee_adr);
  
  else {
    write_exteeprom_byte(0,32); 
    
    password=defult_password;
    write_exteeprom_word(password,password_ee_adr);
  }
  //-------------------------------------------------------------------------  
  pwr_existence=read_exteeprom_byte(pwr_existence_ee_adr);
  if(pwr_existence==0xFF){
    pwr_existence=0;
    write_exteeprom_byte(pwr_existence,pwr_existence_ee_adr);
  }
	
  //-----------------------------------------------log_time setting-------------------------------
  if(read_exteeprom_word(log_time_ee_adr)==0xFFFF){
    log_time=60;
    write_exteeprom_word(log_time,log_time_ee_adr);
    
  }
  else log_time = read_exteeprom_word(log_time_ee_adr);
	
	if(read_exteeprom_word(LogParametersEEadr)==0xFFFF){
    LogParameters=5;
    write_exteeprom_word(LogParameters,LogParametersEEadr);
    
  }
  else LogParameters = read_exteeprom_word(LogParametersEEadr);
	
//	next_log_time=(clock.hour+1)*60;
//  if(next_log_time>=1440) {next_log_time=0; flag.log_disable=1;}
	
//  next_log_time=(clock.hour*60)+clock.min+log_time;
//  if(next_log_time>=1440) {next_log_time=next_log_time%1440; flag.log_disable=1;}
  
  
  //-----------------camera---------------------------------
	if(ext_device & camera){
		if(read_exteeprom_word(camera_log_time_ee_adr)==0xFFFF){
			camera_log_time=60;
			write_exteeprom_word(camera_log_time,camera_log_time_ee_adr);
    
		}
		else camera_log_time = read_exteeprom_word(camera_log_time_ee_adr);
	}
	//------------------------------------------------------ FC ON & OFF TIME      High_nibble:OFF     Low_nibble:ON-----------------
  
	if(read_exteeprom_byte(fc_time_ee_adr)==0xFF)
    
    write_exteeprom_byte(0x23,fc_time_ee_adr);              //2 second ON & 2 second OFF	
  
  fc_on_time=read_exteeprom_byte(fc_time_ee_adr)&0x0F;
  fc_off_time=read_exteeprom_byte(fc_time_ee_adr)>>4;
  
  //------------------------------------------------------set default send_time--------------------------------------------------
  if(read_exteeprom_byte(send_time_ee_adr)==0xFF)
    write_exteeprom_byte(10,send_time_ee_adr);
  
  send_time=read_exteeprom_byte(send_time_ee_adr);
	//---------------------ADC Calib ---------------------
	
	if(read_exteeprom_byte(ADC1CalibOk)==0){
    ADC1Scale= read_exteeprom_float(ADC1Scale_ee_adr);
                                 
  }
  else {
    ADC1Scale=1;
  }
  
  if(read_exteeprom_byte(ADC2CalibOk)==0){
    ADC2Scale= read_exteeprom_float(ADC2Scale_ee_adr);
                                 
  }
  else {
    ADC2Scale=1;
  }
  
  if(read_exteeprom_byte(ADC3CalibOk)==0){
    ADC3Scale= read_exteeprom_float(ADC3Scale_ee_adr);
                                 
  }
  else {
    ADC3Scale=1;
  }
	
	if(read_exteeprom_byte(ADC4CalibOk)==0){
    ADC4Scale= read_exteeprom_float(ADC4Scale_ee_adr);
                                 
  }
  else {
    ADC4Scale=1;
  }
  
  if(read_exteeprom_byte(ADC5CalibOk)==0){
    ADC5Scale= read_exteeprom_float(ADC5Scale_ee_adr);
                                 
  }
  else {
    ADC5Scale=1;
  }
  
  if(read_exteeprom_byte(currentCalibOk)==0){
    currentScale= read_exteeprom_float(currentScale_ee_adr);
                                 
  }
  else {
    currentScale=1;
  }
  
  
  //read door state ------------------------------------------
  save_door_state = read_exteeprom_byte(door_state_ee_adr);
  	 	
//	if(read_exteeprom_byte(IrIsLearned_ee_adr)==0xFF){
//		lcd_pwr_on;
//		flag.LcdOn=1;
//		delayms(20);
//		LCDinit();
//		delayms(20);
//		if(IR_Learning()==1)write_exteeprom_byte(0,IrIsLearned_ee_adr);
//		else{
//			static const char str_f5[]="LEARN FAIL !";
//			LCDclr();
//			LCDputsf(str_f5);
//			delayms(2000);
//		}
//	}  
//	else{
//		read_exteeprom_ary((unsigned char*)button,68,button_ir_ee_adr);    
//		
//	}
//	flag.ir_learn=0;
    
		
//		loadSettingSD();
//		clock.min==0;
//		sprintf(buf,"picture/%04d_%02d/%02d_%02d_%02d.jpg",clock.year,clock.month,clock.day,clock.hour,clock.min);
//		res=upload_picture(buf,"09154431093");	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
		
	HAL_NVIC_EnableIRQ(TIM4_IRQn);//enable input key
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
   
    tgl_wdt;

//		 //-------------------------rain level reset in time---------------------
//    if(((clock.hour*60)+clock.min) != RainRstTime)RainRstFlag=0;
//    
//    if((((clock.hour*60)+clock.min )== RainRstTime) && (RainRstFlag==0)){
//      RainRstFlag=1;
//      rain_level=0;
//      write_exteeprom_float(rain_level,rain_level_ee_adr);
//      
//    }
    //--------------------------clock update after 6 month---------------		
    if(iic_busy_timeout>60){
			flag.i2c_busy=0;
		}
		
    if(clock.hour==0 && clock.min==0){
      
      flag.log_disable=0; //enable clock compare for data logging
          
      if(clock.day==1 && clock.month==1){
        clock.hour++;
        rtc_settime_shamsi(&clock);
      }
      
      else if(clock.day==1 && clock.month==7 && flag.rtc_update==0){
        flag.rtc_update=1;
        clock.hour=23;
        clock.day=31;
        clock.month=6;
        rtc_settime_shamsi(&clock);
        
      }
      
    }
    
    if(clock.hour==1 && flag.rtc_update==1) flag.rtc_update=0;
    
    //------------------------------------ in time processing--------------------------------
    	
		
		if(flag.log_start==1){
			flag.log_start=0;
      		
			log_data(log_buffer);
			flag.log_disable=1;
			
    }
		if(ext_device & camera){
			if(flag.log_picture==1){
				flag.log_picture=0;
				
				log_picture(1);
				
			}
		}
		if(ext_device & gsm){
			if(gprs_auto_upload & gprs_enable){
				if(gprs_auto_upload & gprs_upload_logfile){
					if(flag.upload_log==1){
						flag.upload_log=0;
						sprintf(buf,"%04d_%02d.txt",clock.year,clock.month);
						upload_file(buf,ServerPhone);
					}
					
				}
				
				if(gprs_auto_upload & gprs_upload_tabletime){
					if(flag.upload_in_schedule==1){
						flag.upload_in_schedule=0;
						ReadSensorToStr(gprs_buffer);
						upload_instant(gprs_buffer,ServerPhone);
						if(ext_device & camera){
							res=get_picture(1,server_filename);
							if(res!=0){
								strcpy(SMS_text,"Camera Response\r\nCamera Connection Fail");
								SendSMS(ServerPhone,SMS_text);
								
							}
							else{
								upload_file(server_filename,ServerPhone);
							}
						}	
					}
					
				}
			}
		}
//------------------------------------------------------------
		if(ext_device & water_sensor){
			water_level=(int16_t)(TIM1->CNT)/(int16_t)RotaryScale;
			
			if(water_level>(cur_water_level+5)){
				cur_water_level=water_level;
				write_exteeprom_word(water_level,water_level_ee_adr);
			}
			if(water_level<(cur_water_level-5)){
				cur_water_level=water_level;
				write_exteeprom_word(water_level,water_level_ee_adr);
			}
			
			#ifdef marv		
			if((water_level>(last_water_level+delta_water_level))||(water_level<(last_water_level-delta_water_level))){
							
				last_water_level=water_level;
		  #endif
			#ifdef saramin		
			if((water_level>(delta_water_level)) && flag.levelevent==0){
				
				flag.levelevent=1;
				
		  #endif
				
				if(ext_device & gsm){
					sprintf(SMS_text,"%04d/%02d/%02d,\r\n%02d:%02d:%02d,\r\nLVL=%d,\r\nLVLevent,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,water_level);		
					SendSMS(ServerPhone,SMS_text);
					delayms(2000);
					read_exteeprom_ary(AdminMobile,29,AdminMobile1_ee_adr); 
					SendSMS(AdminMobile,SMS_text);
					delayms(2000);
					read_exteeprom_ary(AdminMobile,29,AdminMobile2_ee_adr); 
					SendSMS(AdminMobile,SMS_text);
					delayms(3000);
				}
				if(ext_device & camera){
					
					res=get_picture(1,"LVLEVENT.jpg");
					if(res!=0){
						strcpy(SMS_text,"Camera Connection Fail");
						SendSMS(ServerPhone,SMS_text);
					
					}
					else{
						
						upload_file("LVLEVENT.jpg",ServerPhone);
					}
				}
				
			}
//			#ifdef marv
//			if(water_level<last_water_level){
//				last_water_level=water_level;

//			}
//			#endif
			
			#ifdef saramin
			if((water_level<(delta_water_level))){
				flag.levelevent=0;
			}
			#endif
			
		}
		
    //----------------------rain sensor heater control---------
		if(ext_device & rain_sensor){
		 if(temp_time>600){
			 if(heater_control==1)
				ReadExtTemp(&heater_temp);
			 else if(heater_control==2)
				temp_lm75(&heater_temp);
			 
       temp_time=0;
     }
        
     if(heater_temp<T_low && flag.temp_control==0){
       flag.temp_control=1;
          
       heater_on;
     }
     if(heater_temp>T_hi && flag.temp_control==1){
       flag.temp_control=0;
          
       heater_off;
     }
	 }
    //---------------------------------------------------------------------------
            
    if((send_count>fc_delay) && flag.refresh==1 && (ext_device & RadioModem)){
      
      resend++;
      if(flag.ack || flag.aburt){
        flag.refresh=0;
        flag.aburt=0;
        flag.ack=0;
        resend=0;
        
        
      }
      
      else{
        if(resend<send_time){
          
          wakeup_cou=0;
          if(flag.fc_net_busy==0)
            fc301d_send(fc301d_send_buffer);
          send_count=0;
          
        }
        else {
          resend=0;flag.refresh=0;
          if(ext_device & gsm){
            
            ReadSensorToStr(SMS_text);
            SendSMS(ServerPhone,SMS_text);            
          }
          
        }
        
      }
    }
    
    
    //if(iic_pwr_count>10) i2c_pwr_off;  
    
    
    //*****************************************************check for wake up*************   
    //about 10 minutes
    if(flag.wakeup==1 && wakeup_cou>120){     
      flag.wakeup=0;
      flag.comm=0;
      wakeup_cou=0;
      
    } 
		
    
    //---------------Detect Events and send to Center---------
    event_process();
    
    
    //Reply Request from Monitoring Center-----------------------
    if(flag.fc301d_reply_cmd==1){
      fc301d_reply_command();
      flag.fc301d_reply_cmd=0;
      resend=0;
    }	
    
    if(ext_device & gsm){
      
			if((clock.hour)*60+clock.min != SmsSendTime)SmsSendFlag=0;
    
			if(((clock.hour)*60+clock.min == SmsSendTime)&&(SmsSendFlag==0)){
				SmsSendFlag=1;
				ReadSensorToStr(SMS_text);
				SimChargeCat(SMS_text);
        SendSMS(ServerPhone,SMS_text);
				
			}
					            
      if(gsm_off_time!=clock.hour){
        gsm_off_time=clock.hour;
        simpwr_off;
        delayms(5000);
        GsmInit();
        flush_buf(SMS_text,250);
        
      }
      
       if((sms_pointer=StrSearch_P(SimRecBuffer,"+CMTI:",250))!=NULL){  //+CMTI:
         delayms(2000);
				 sms_pointer=CharSearch_str(sms_pointer,',');
				 if(sms_pointer != NULL){
				 sms_pointer++;
				 sms_pointer1=CharSearch_str(sms_pointer,CRchar);
				 if(sms_pointer1 != NULL){
						 *sms_pointer1=0;
						 SMS_number=strtolong(sms_pointer);
		
							if(ReadSMS(RemotePhone,SMS_text,SMS_number)==1){
		
								if(flag.LcdOn==1 && flag.ir_flag==0){
									LCDclr();
									sprintf(buf,"SMS Receive:\n    %s",RemotePhone);
									LCDputs(buf);
									
								}
								SMS_ReplyCMD(RemotePhone,SMS_text);
								delayms(200);
						}
					}
				}
				for(i=1;i<5;i++){
					flush_buf(SimRecBuffer,250);
					sprintf(SimSendBuffer,"AT+CMGD=%d%c%c",i,CRchar,LF);
					GsmSend(SimSendBuffer);
					SimCount=0;
					sms_pointer=NULL;
					while(SimCount<3 && sms_pointer==NULL){
						sms_pointer=StrSearch(SimRecBuffer,"OK",250);
					}
					delayms(200);
				}
        flush_buf(SimRecBuffer,250);
        flush_buf(SMS_text,250);
        SimRecLen=0;
                
      }
      
    }
    
    if(remute_ir_clear==0){
      delayms(200);
      if(remute_ir_clear==0){
        delayms(200);
        if(remute_ir_clear==0){
          write_exteeprom_byte(0xFF,IrIsLearned_ee_adr);
          flush_buf((unsigned char*)button,80);
          write_exteeprom_ary((unsigned char*)button,68,button_ir_ee_adr);
          if( flag.LcdOn==1 && flag.ir_flag==0){
            LCDclr();
            LCDputs("CLEAR IR !");
            delayms(1000);
          }
          while(1);
        }
      }
      
    }
    
    //********************Display off in time*************************************************
    if(flag.LcdOn==1){
      display();
      if(DisplayTime>600){
        lcd_pwr_off;
        flag.LcdOn=0;
      }
    }
    
		    
  }
  

  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV8;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* ADC3 init function */
static void MX_ADC3_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.ScanConvMode = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* I2C2 init function */
static void MX_I2C2_Init(void)
{

  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* IWDG init function */
static void MX_IWDG_Init(void)
{

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SDIO init function */
static void MX_SDIO_SD_Init(void)
{

  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 10;

}

/* TIM1 init function */
static void MX_TIM1_Init(void)
{

  TIM_Encoder_InitTypeDef sConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 672;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 62500;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 20;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM4 init function */
static void MX_TIM4_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 2100;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 100;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM6 init function */
static void MX_TIM6_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 192;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_11 
                          |GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE3 PE4 PE5 
                           PE6 PE7 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PF2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB11 
                           PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_11 
                          |GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PE8 PE12 PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_12|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PE10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PE15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PD10 PD11 PD14 PD0 
                           PD1 PD3 PD4 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_0 
                          |GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PG3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PG9 */
//  GPIO_InitStruct.Pin = GPIO_PIN_9;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
