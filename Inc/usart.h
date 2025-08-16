#ifndef USART_H
#define USART_H

#include "includes.h"

#define fc_start 100

unsigned char fc301d_rec(void);       // recieves RF data from FC301D/M module trough usart0 and put them to global ary prim_rec_buf[]

void fc301d_send(unsigned char *);    // sends RF data trough FC301D/M module with usart0 (global ary send_buf[])

void RS485_send(unsigned char *,unsigned int);

void RS232_send(unsigned char *,unsigned int);

void usart_init(USART_TypeDef* uart,uint32_t buad);

#endif
