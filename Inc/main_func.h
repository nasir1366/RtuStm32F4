#ifndef _MAIN_FUNC_INCLUDED_
#define _MAIN_FUNC_INCLUDED_

#include "includes.h"

void read_sensours(unsigned char*);

unsigned char log_data() ;  

unsigned char send_log_data(uint16_t f_year,unsigned char f_month,unsigned char f_day,unsigned char start_hour,unsigned char start_minute,unsigned char end_hour,unsigned char end_minute);

void check_door_state();

void display();

unsigned char analyse();

void fc301d_reply_command();

void correct_rain_level(volatile float,volatile float*);

unsigned char address_process(unsigned char*);

void event_process();

void read_pcf8574();

void read_bmp180sensor();

float read_water_level();

void amp_pwr(char stat);

void play_start(unsigned char num);

void play_stop();

void record_start(unsigned char num);

void record_stop();

void Error_Handle();

void wdt_refresh();

uint32_t strtolong(char *str);

uint8_t ReadDHT11(uint16_t *RH,uint16_t *Temperature);

void ReadSensorToStr(char *str);

uint8_t FindLog(uint8_t*,char*);

void log_event(char* log_str);

void enter_critical_section();

void exit_critical_section();

void SARAMIN(char *str);

#endif
