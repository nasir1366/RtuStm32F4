/*
 * AD7705/AD7706 Library
 * Kerry D. Wong
 * http://www.kerrywong.com
 * Initial version 1.0 3/2011
 * Updated 1.1 4/2012
 */

#include "AD770X.h"
#include "includes.h" 
#include "global_var.h"
#include "enc28j60.h"
#include "my_delay.h"  


uint8_t spiTransfer(uint8_t data) {

  uint8_t temp=0;
  
  HAL_SPI_TransmitReceive(&hspi3,&data,&temp,1,100);
  
  return temp;
}

//write communication register
//   7        6      5      4      3      2      1      0
//0/DRDY(0) RS2(0) RS1(0) RS0(0) R/W(0) STBY(0) CH1(0) CH0(0)

void AD770X_setNextOperation(uint8_t reg, uint8_t channel, uint8_t readWrite) {
    uint8_t r = 0;
    r = reg << 4 | readWrite << 3 | channel;

    AD770X_CS_LOW;
    HAL_SPI_Transmit(&hspi3,&r,1,100);
    AD770X_CS_HI;
}

//Clock Register
//   7      6       5        4        3        2      1      0
//ZERO(0) ZERO(0) ZERO(0) CLKDIS(0) CLKDIV(0) CLK(1) FS1(0) FS0(1)
//
//CLKDIS: master clock disable bit
//CLKDIV: clock divider bit

void AD770X_writeClockRegister(uint8_t CLKDIS, uint8_t CLKDIV, uint8_t outputUpdateRate) {
    uint8_t r = CLKDIS << 4 | CLKDIV << 3 | outputUpdateRate;

    r &= ~(1 << 2); // clear CLK

    AD770X_CS_LOW;
    HAL_SPI_Transmit(&hspi3,&r,1,100);
    AD770X_CS_HI;
}

//Setup Register
//  7     6     5     4     3      2      1      0
//MD10) MD0(0) G2(0) G1(0) G0(0) B/U(0) BUF(0) FSYNC(1)

void AD770X_writeSetupRegister(uint8_t operationMode, uint8_t gain, uint8_t unipolar, uint8_t buffered, uint8_t fsync) {
    uint8_t r = operationMode << 6 | gain << 3 | unipolar << 2 | buffered << 1 | fsync;

    AD770X_CS_LOW;
    HAL_SPI_Transmit(&hspi3,&r,1,100);
    AD770X_CS_HI;
}

uint16_t AD770X_readADResult() {
    
  uint8_t b1=0,b2=0;
  uint16_t r=0,b=0;
  
  AD770X_CS_LOW;
  //HAL_SPI_Receive(&hspi3,(uint8_t*)(&b),2,100);
  HAL_SPI_Receive(&hspi3,&b1,1,100);
  HAL_SPI_Receive(&hspi3,&b2,1,100);
  
  AD770X_CS_HI;
  
   r= b1 << 8 | b2;
  
  return r;
}

double AD770X_readADvoltage(uint8_t channel, float refOffset) {
    while (!AD770X_dataReady(channel)) {
    };
    AD770X_setNextOperation(REG_DATA, channel, 1);

    return AD770X_readADResult() * 1.0 / 65536.0 * 2.5 - refOffset;
}

uint8_t AD770X_dataReady(uint8_t channel) {
  uint8_t b1 ;
    AD770X_setNextOperation(REG_CMM, channel, 1);

    AD770X_CS_LOW;
    HAL_SPI_Receive(&hspi3,&b1,1,100);
    AD770X_CS_HI;

    return (b1 & 0x80) == 0x0;
}

void AD770X_reset() {
  uint8_t r=0xFF;
  
    AD770X_CS_HI;
    AD770X_RST_LOW;
    delayms(1);
    AD770X_RST_HI;
    delayms(1);
    AD770X_CS_LOW;
    for (int i = 0; i < 100; i++)
        HAL_SPI_Transmit(&hspi3,&r,1,100);
    AD770X_CS_HI;
}


void init_AD770X_1(uint8_t channel, uint8_t clkDivider, uint8_t polarity, uint8_t gain, uint8_t updRate) {
    AD770X_setNextOperation(REG_CLOCK, channel, 0);
    AD770X_writeClockRegister(0, clkDivider, updRate);

    AD770X_setNextOperation(REG_SETUP, channel, 0);
    AD770X_writeSetupRegister(MODE_SELF_CAL, gain, polarity, 0, 0);

    while (!AD770X_dataReady(channel)) {
    };
}

void init_AD770X(uint8_t channel) {
    
    enc_CSPASSIVE;
    
    init_AD770X_1(channel, CLK_DIV_1, BIPOLAR, GAIN_1, UPDATE_RATE_25);
}
