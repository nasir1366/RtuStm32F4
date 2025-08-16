#ifndef wifi_INCLUDED
#define wifi_INCLUDED

#include "includes.h"

#define wifi_pwr_on GPIOB->ODR |= GPIO_PIN_11
#define wifi_pwr_off GPIOB->ODR &= ~GPIO_PIN_11


void wifi_init();

void wifi_send_cmd(char* frame);

#endif
