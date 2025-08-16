#ifndef GLOBVAR_H
#define GLOBVAR_H

#include "includes.h"
#include "fatfs.h"
#include "rtc.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc3;

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

extern SD_HandleTypeDef hsd;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern IWDG_HandleTypeDef hiwdg;

/* USER CODE BEGIN PV */
extern const uint16_t station_address;
extern char station_name[30];
extern const char firmware_version[];
extern const char title_str[];

extern ADC_ChannelConfTypeDef adConfig;

extern volatile Rtc clock,cur_clock;

// Counter & Index Var ------------------------
extern volatile uint8_t play_count, alarm_count, resend,len,display_count,iic_count,rec_cou,rf_pwr_count,
time_out,time_out_pass,send_time,p_rec_cou,p_len,loop_timeout,ir_cou,iic_pwr_count,
fc_clr_time,next_msg,usart_rain_cou,ReadWaterTime,log_period,minute_changed,iic_busy_timeout;


extern volatile uint16_t send_count,count,rain_cou,wakeup_cou,i,j,wind_timeout,test_mode_timeout,temp_time;  

//Normal Var------------------------------------------------------------  
extern volatile uint8_t st_type,pwr_existence,
fc_on_time,fc_off_time,cur_minute,save_door_state,
valid_door_flag,snow_mtr_time,cur_resend,rain_sensor_type,fc_delay,temp_type,
sound_number,WindSensorType;

extern volatile uint16_t password,input_pass,alarm,log_time,next_log_time,sample_count,start_time,stop_time,
return_time,WaterSensorTime,DisplayTime,wind_dir,cur_wind_dir,wind_speed,cur_wind_speed,LogParameters,
time24;
//----------------Analog input var--------------------
extern float ADC1Scale,ADC2Scale,ADC3Scale,ADC4Scale,ADC5Scale,currentScale;
//---------water sensor var--------------------------
extern unsigned char WaterSensorType;
extern volatile int16_t water_level,last_water_level,delta_water_level,cur_water_level_display,
max_water_level,min_water_level,SensorLevel,RotaryResolution,RotaryDiameter,RotaryScale,cur_water_level;
extern float Imin,Imax;
extern volatile uint16_t SetLevelBar_count,SetLevelBar_timeout;
//-------------Rain Sensor Var--------------------------
extern volatile float rain_level,cur_rain_level,last_rain_level,delta_rain_level,rain_time,minute_rain;
extern volatile float rain_register[20],rain_sum;
extern volatile uint8_t rain_register_index;
extern volatile uint16_t RainRstTime,RainRstFlag;
extern volatile uint16_t RainEventTime,minute_rain_cou;

extern volatile uint8_t heater_control;
extern signed char heater_temp;
//***---------------temperature  & humidity sensor ------------------
extern signed char temp_in,cur_temp_in,temp_out,cur_temp_out,temp_difference,T_low,T_hi;
extern uint8_t sht_rec_buffer[15],sht_send_buffer[5],temp_rec_len,humidity;
//-----------------------------------------------
extern volatile uint32_t write_adr,read_adr,data_ir,button[20],ext_device,AirPressure,cur_air_pressure;

extern volatile float wind_time,WindAlpha,WindBeta,AirAlpha,AirBeta,utp,ltp;
//-------------------wifi--------------------------------
extern uint8_t wifi_rec_buffer[250],wifi_send_buffer[250];
extern volatile uint8_t wifi_rec_index;
//-----------Camera Module Variables------------- 
extern volatile unsigned int PictureSize,PictureSizeCounter,TakePictureCount,CamRecLen,
camera_log_time;
extern unsigned char CameraBuffer[550],CameraBuffer1[550],MemBuf[60000]/*1310*/,*CameraBufPointer;
extern volatile unsigned char CamBufOvf,PictureRecProcess,rs485_select;

extern unsigned char gprs_first_connect,gprs_auto_upload;

extern unsigned char server_url[50],server_username[30],server_password[20],server_filename[50],server_path[100],apn_name[30];

extern char camera_reset[4];
extern char camera_TakePicture[5];
extern char camera_ReadFileSize[5];
extern char camera_ReadFileContent[16];
extern char camera_ChangePicSizeLarge[5];
extern char camera_ChangePicSizeSmall[5];
extern char camera_changeboud[7];
//--------bmp variabls-----------
extern volatile int16_t AC1,AC2,AC3,B1,B2,MB,MC,MD;
extern volatile uint16_t AC4,AC5,AC6;
extern volatile int32_t UP,UT,B3,B4,B5,B6,B7,X1,X2,X3,T;
extern volatile float P;

//------------thalimedes --------------
extern unsigned char thali_buf[20];
//buffer Definition ***********************

extern uint8_t p_rec_buf[15],p_send_bufer[15],fc301d_send_buffer[255],fc301d_rec_buffer[255],
log_buffer[100],base_address[30],log_header[20];

extern uint8_t usart2_rx[2],usart3_rx[2];
//uint16_t log_avg[40];
extern uint8_t buf[255];
//-------gsm module---------------
extern uint8_t SimSendBuffer[255],SimRecBuffer[255],ServerPhone[30],RemotePhone[30]
,SMS_text[255],ChargeReqStr[30],gprs_buffer[250];
extern uint8_t AdminMobile[30];
extern volatile uint8_t gsm_off_time,SmsSendFlag,CUSD_cnt;
extern volatile int8_t gsm_signal,SMS_number,ftp_path_separator;
extern volatile uint16_t SimCount,SmsSendTime,SimRecLen,user_sms_count,user_sms_timeout,gprs_upload_time;
extern volatile uint16_t upload_time_table[5];
//--------keypad string-----------
extern char input_number_str[20];
extern uint8_t number_limit,number_limit_index;
//pointer-------------------------
extern uint8_t *rec_data,*data,**endptr;

//FatFs variable definition---------------------

extern FATFS SDFatFs;  /* File system object for SD card logical drive */
extern uint8_t SDPath[4]; /* SD card logical drive path */

extern FRESULT res;                                          /* FatFs function common result code */

extern FATFS Fatfs[_VOLUMES];		/* File system object for each logical drive */
extern FIL File[2];				/* File object */
extern DIR dir[5];
extern UINT FatBytesWritten;
extern UINT FatBytesRead;

/////ADXL345 --------------------------------------------
extern uint16_t Accel_Xout_Val, Accel_Yout_Val, Accel_Zout_Val;
extern int16_t Accel_Xout_Normalized_Val, Accel_Yout_Normalized_Val, Accel_Zout_Normalized_Val;
extern uint8_t Offset_X_Val,Offset_Y_Val,Offset_Z_Val;
extern float Accel_XAngle, Accel_YAngle, Accel_ZAngle;
//Structures & Bit Variables -----------------------------------------------------------------
#pragma anon_unions
union {
  
  uint8_t msg;
  struct {
    
    uint8_t num:7;
    uint8_t device:1;
  }; 
}extern volatile play_buffer[20];



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
  
  
}extern volatile flag;


//-------keypad------------------
extern uint8_t upkey_press,downkey_press,rightkey_press,leftkey_press,okkey_press,menukey_press,esckey_press;

//commands : {length,cmd}

typedef struct  
{uint8_t length; uint8_t cmd;}command;


extern const command 
wakeup ,

play_alarm,
stop_alarm,   
hozur_taki,
acknlg,
send_data,

read_mem,
time_stamp,

power_config,
read_power_config,
send_power_config,

rtc_config,
send_rtc,
read_rtc,

read_fc_time,
send_fc_time,
fc_time_config,

read_send_time,
send_time_config,
transmit_send_time,

log_time_config,
send_log_time,
read_log_time,
end_pack,
aburt_process,

change_server_phone,
server_phone_reply,
server_phone_read,

read_data_ascii,

snowmotor;

/* USER CODE END PV */


#endif
