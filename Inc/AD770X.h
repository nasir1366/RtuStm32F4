#ifndef AD770X_H
#define AD770X_H

#include "stm32f4xx_hal.h"
/*
* AD7705/AD7706 Library
* Kerry D. Wong
* http://www.kerrywong.com
* Initial version 1.0 3/2011
* Updated 1.1 4/2012
*/


#define AD770X_CS_LOW GPIOF->ODR &= ~GPIO_PIN_7
#define AD770X_CS_HI GPIOF->ODR |= GPIO_PIN_7
#define AD770X_RST_LOW GPIOF->ODR &= ~GPIO_PIN_9
#define AD770X_RST_HI GPIOF->ODR |= GPIO_PIN_9

//register selection 
//RS2 RS1 RS0
static const uint8_t REG_CMM = 0x0; //communication register 8 bit
static const uint8_t REG_SETUP = 0x1; //setup register 8 bit
static const uint8_t REG_CLOCK = 0x2; //clock register 8 bit
static const uint8_t REG_DATA = 0x3; //data register 16 bit, contains conversion result
static const uint8_t REG_TEST = 0x4; //test register 8 bit, POR 0x0
static const uint8_t REG_NOP = 0x5; //no operation
static const uint8_t REG_OFFSET = 0x6; //offset register 24 bit
static const uint8_t REG_GAIN = 0x7; // gain register 24 bit

//channel selection for AD7706 (for AD7705 use the first two channel definitions)
//CH1 CH0
static const uint8_t CHN_AIN1 = 0x0; //AIN1; calibration register pair 0
static const uint8_t CHN_AIN2 = 0x1; //AIN2; calibration register pair 1
static const uint8_t CHN_COMM = 0x2; //common; calibration register pair 0
static const uint8_t CHN_AIN3 = 0x3; //AIN3; calibration register pair 2

//output update rate
//CLK FS1 FS0
static const uint8_t UPDATE_RATE_20 = 0x0; // 20 Hz
static const uint8_t UPDATE_RATE_25 = 0x1; // 25 Hz
static const uint8_t UPDATE_RATE_100 = 0x2; // 100 Hz
static const uint8_t UPDATE_RATE_200 = 0x3; // 200 Hz
static const uint8_t UPDATE_RATE_50 = 0x4; // 50 Hz
static const uint8_t UPDATE_RATE_60 = 0x5; // 60 Hz
static const uint8_t UPDATE_RATE_250 = 0x6; // 250 Hz
static const uint8_t UPDATE_RATE_500 = 0x7; // 500 Hz

//operating mode options
//MD1 MD0
static const uint8_t MODE_NORMAL = 0x0; //normal mode
static const uint8_t MODE_SELF_CAL = 0x1; //self-calibration
static const uint8_t MODE_ZERO_SCALE_CAL = 0x2; //zero-scale system calibration, POR 0x1F4000, set FSYNC high before calibration, FSYNC low after calibration
static const uint8_t MODE_FULL_SCALE_CAL = 0x3; //full-scale system calibration, POR 0x5761AB, set FSYNC high before calibration, FSYNC low after calibration

//gain setting
static const uint8_t GAIN_1 = 0x0;
static const uint8_t GAIN_2 = 0x1;
static const uint8_t GAIN_4 = 0x2;
static const uint8_t GAIN_8 = 0x3;
static const uint8_t GAIN_16 = 0x4;
static const uint8_t GAIN_32 = 0x5;
static const uint8_t GAIN_64 = 0x6;
static const uint8_t GAIN_128 = 0x7;

static const uint8_t UNIPOLAR = 0x0;
static const uint8_t BIPOLAR = 0x1;

static const uint8_t CLK_DIV_1 = 0x1;
static const uint8_t CLK_DIV_2 = 0x2;


uint8_t spiTransfer(uint8_t data);
void AD770X_setNextOperation(uint8_t reg, uint8_t channel, uint8_t readWrite);
void AD770X_writeClockRegister(uint8_t CLKDIS, uint8_t CLKDIV, uint8_t outputUpdateRate);
void AD770X_writeSetupRegister(uint8_t operationMode, uint8_t gain, uint8_t unipolar, uint8_t buffered, uint8_t fsync);
double AD770X_readADvoltage(uint8_t channel, float refOffset);
uint16_t AD770X_readADResult();
void AD770X_reset();
uint8_t AD770X_dataReady(uint8_t channel);
void init_AD770X(uint8_t channel);
void init_AD770X_1(uint8_t channel, uint8_t clkDivider, uint8_t polarity, uint8_t gain, uint8_t updRate);

uint16_t readADResult();

#endif
