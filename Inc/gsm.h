#ifndef gsm_INCLUDED
#define gsm_INCLUDED

#include "stm32f4xx_hal.h"

#define CRchar 13
#define LF 10
#define CZ 26
#define ESC 27

char GsmInit();


void GsmSend(unsigned char*);

void GsmSendByte(unsigned char *,unsigned int);

char SendSMS(char *, char *);

char ReadSMS(char*,char*,unsigned char);

char SaveSMS(char*);

void BinToStr(unsigned char*,unsigned char,char*);

char StrToBin(unsigned char*,char*);

void SMS_ReplyCMD(char*,char*);

void flush_buf (unsigned char *,uint16_t);

unsigned char* StrSearch(unsigned char*,char*,uint16_t);

unsigned char* StrSearch_P(unsigned char*,char const*,uint16_t);

unsigned char* CharSearch(unsigned char *,char ,uint16_t );

unsigned char* CharSearch_str(unsigned char *buffer,char chr);

void strcopy(char *,char *);

void SimChargeCat(unsigned char*);

void sendfile(unsigned char*);

void lowercase(char *str,unsigned int size);

uint8_t upload_instant(char* data_str,char *PhoneNumber);

uint8_t upload_file(char* filename,char *PhoneNumber);
  
#endif