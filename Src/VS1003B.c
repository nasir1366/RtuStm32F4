/*******************************************************************/
/*          VS1003 diriver for  Mega8 MP3 Player                   */
/*                                                                 */
/* Platform   : AVRStudio4.13 b528 + WinAVR20070122                */
/*              optimize -0s                                       */
/* Author     : bozai(Zhang Qibo)                                  */
/* E-mail     : sudazqb@163.com                                    */
/* MSN        : zhangqibo_1985@hotmail.com                         */
/* Date       : 2006-05-09                                         */
/*******************************************************************/
/*2007-11-22: add bass tremble control                             */
/*2007-10-21: add new functions,                                   */
/*2007-10-19: use new delay function                               */
/*2007-05-04: add slow start up code, and add enough delay         */
/*2007-04-21:                                                      */



#include "VS1003B.h"
#include "my_delay.h"

#include "includes.h"
#include "stm32f4xx_hal_spi.h" 

extern SPI_HandleTypeDef hspi2;
void MX_SPI2_Init(void);
//HAL_StatusTypeDef SPI_WaitOnFlagUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Flag, FlagStatus Status, uint32_t Timeout);

#define uchar unsigned char
#define uint  unsigned int



void VS1003B_SPI_Low(void);//low spi clk
void VS1003B_SPI_High(void);//high spi clk
void VS1003B_WriteByte(unsigned char CH);//spi write a byte
void VS1003B_WriteCMD(unsigned char addr, unsigned int dat);//write register 
unsigned int VS1003B_ReadCMD(unsigned char addr);//read register 

const unsigned int sample_rate_values[] = {0, 44100, 48000, 32000, 22050, 24000, 16000, 11025, 12000, 8000};

const unsigned char sample_rate_values1[] = {11,12,8};
const unsigned char sample_rate_values2[] = {22,24,16};
const unsigned char sample_rate_values3[] = {44,48,32};
const unsigned int  bit_rate_values1[]    = {0,0,40,48,56,64,80,96,112,128,160,192,224,256,320};
const unsigned char bit_rate_values2[]    = {8,16,24,40,48,56,64,80,96,112,128,144,160};
const char mode_values[4][13]             = {   "Stereo","JointStereo","DualChannel","Mono"};



void sine(void) {
  
  VS1003B_XDCS_H();
  VS1003B_XCS_L();
  VS1003B_WriteByte(0x53);
  VS1003B_WriteByte(0xef);
  VS1003B_WriteByte(0x6e);
  //VS1003B_WriteByte(0x30);  
  VS1003B_WriteByte(0x7e);
  VS1003B_WriteByte(0);
  VS1003B_WriteByte(0);
  VS1003B_WriteByte(0);  
  VS1003B_WriteByte(0);
  VS1003B_XCS_H();
}



// Run at the loweset spi speed, clk should below 12.288M/6 
void VS1003B_SPI_Low(void)
{
  
}

// High speed, should not exceed 49.512MHz/6 
void VS1003B_SPI_High(void)
{
  
}

// write one byte to vs1003 
void VS1003B_WriteByte(unsigned char CH)
{   
  
  if(HAL_SPI_Transmit(&hspi2,&CH,1,100)!=HAL_OK){
    //MX_SPI2_Init();
    HAL_Delay(1);
  }
}

// read one byte from vs1003 
unsigned char VS1003B_ReadByte()
{
  unsigned char data=0;

  if(HAL_SPI_Receive(&hspi2,&data,1,100)!=HAL_OK){
    //MX_SPI2_Init();
    return 0;
  }
  return data;
}

//  config register 
void VS1003B_WriteCMD(unsigned char addr, unsigned int dat)
{
  VS1003B_XDCS_H();
  VS1003B_XCS_L();
  VS1003B_WriteByte(0x02);
  VS1003B_WriteByte(addr);
  VS1003B_WriteByte(dat>>8);
  VS1003B_WriteByte(dat);
  VS1003B_XCS_H();
}

//read register 
unsigned int VS1003B_ReadCMD(unsigned char addr)
{
  unsigned int temp;
  VS1003B_XDCS_H();
  VS1003B_XCS_L();
  VS1003B_WriteByte(0x03);
  VS1003B_WriteByte(addr);
  temp = VS1003B_ReadByte();
  temp <<= 8;
  temp += VS1003B_ReadByte();
  VS1003B_XCS_H();
  return temp;
}

// fill 2048 zero
void VS1003B_Fill2048Zero()
{
  unsigned char i,j;
  VS1003B_SPI_High();
  VS1003B_XCS_L();
  for(i=0;i<64;i++)
  {    
    VS1003B_XDCS_L();
    while(VS1003B_NeedData()==0);
    for(j=0;j<32;j++)
    {
      VS1003B_WriteByte(0x00);
    }
    VS1003B_XDCS_H();
  }
  VS1003B_XCS_H();
}

// Write 32bytes data 
void VS1003B_Write32B(unsigned char * buf)
{
  unsigned char n = 32;
  
  VS1003B_SPI_High();
  VS1003B_XDCS_L();
  //HAL_SPI_Transmit_DMA(&hspi2,buf,32);
  while(n--)
  {
    VS1003B_WriteByte(*buf++);
  }
  VS1003B_XDCS_H();
  
  
}

/**********  Initialization, 1 means fail, 0 OK ***************/
unsigned char VS1003B_Init() {
  
  unsigned char retry;
  
  VS1003B_XCS_PORT |= 1<<VS1003B_XCS_BIT;
  VS1003B_XRESET_PORT |= 1<<VS1003B_XRESET_BIT;
  VS1003B_XDCS_PORT |= 1<<VS1003B_XDCS_BIT;
  
  VS1003B_XRESET_L();	/* A hardware reset */
  _delay_ms(20);
  VS1003B_XRESET_H();
  
  VS1003B_SPI_Low();	/* Low initialize spi clock */
  _delay_ms(20);
  
  retry=0;
  
  while(VS1003B_ReadCMD(0x03) != CLOCK_REG)	/* set PLL register */
  {
    VS1003B_WriteCMD(0x03,CLOCK_REG);
    if(retry++ >10 )return 1;
  }
  
  _delay_ms(20);
  
  VS1003B_WriteCMD(0x05,0x000a);
  
  retry=0;
  while(VS1003B_ReadCMD(0x0b) != 0xfefe)        /* set Volume to minimum */
  {
    VS1003B_WriteCMD(0x0b,0xfefe);
    if(retry++ >10 )return 1;
  }
  
  VS1003B_WriteCMD(0x05,0xac45);                        /* Soft start */
  
  retry=0;
  while(VS1003B_ReadCMD(0x0b) != DEFAULT_VOLUME)        /* Set volume to default value */
  {
    VS1003B_WriteCMD(0x0b,DEFAULT_VOLUME);
    if(retry++ >10 )return 1;
  }
  
  retry=0;
  while(VS1003B_ReadCMD(0x00) != 0x0800)        /* set mode register */
  {
    VS1003B_WriteCMD(0x00,0x0800);
    if(retry++ >10 )return 1;
  }
  
  _delay_ms(1);
  
  retry=0;
  while(VS1003B_ReadCMD(0x02) != DEFAULT_BASS_TREMBLE)        /* set bass/tremble register */
  {
    VS1003B_WriteCMD(0x02,DEFAULT_BASS_TREMBLE);
    if(retry++ >10 )return 1;
  }
  _delay_ms(20);
  
  VS1003B_SoftReset();        /* A soft reset */
  
  _delay_ms(20);
  
  VS1003B_SPI_High();                /* High SPI clock, for internal pll has been works now */
  return 0;
}

// VS1003B soft reset 
void VS1003B_SoftReset()
{
  VS1003B_SPI_High();
  VS1003B_WriteCMD(0x00,0x0804);        /* reset */
  _delay_ms(20);
}

// get total decode time form last reset 
unsigned int VS1003B_ReadDecodeTime()
{
  VS1003B_SPI_Low();
  return VS1003B_ReadCMD(0x04);
}

/* check if the chip need data, 1: need  0: don't */
unsigned char VS1003B_NeedData(void)
{
  if(VS1003B_DREQ_PIN & (1<<VS1003B_DREQ_BIT))return 1;
  else return 0;
}

/* Set volume */
void VS1003B_SetVolume(unsigned int vol)
{
  VS1003B_SPI_High();
  VS1003B_WriteCMD(VS1003_VOLUME,vol);        
}

// read the AUDATA register
//unsigned int VS1003_get_audio_data(AUDIO_DATA* audio)
//        { 
//	unsigned int audata; 
//        VS1003B_SPI_Low();
//        audata = VS1003B_ReadCMD(VS1003_AUDATA);
//	audio->sample_rate = pgm_read_word(sample_rate_values + ((audata&0x1E00)>>9));
//	audio->bitrate = audata&0x1FF;
//	audio->is_stereo = (audata&0x8000)>>15;
//        return audata;
//        }

void VS1003_get_audio_info(struct VS1003_Mp3Info* Mp3Info)   
{   
  unsigned int temp;
  unsigned char ID,SR,BR;
  VS1003B_SPI_Low();      
  ID = (unsigned char)((VS1003B_ReadCMD(VS1003_HEDATA1) & 0x0018) >> 3);      
  temp = VS1003B_ReadCMD(VS1003_HEDATA0);   
  SR = (char)((temp & 0x0C00) >> 10);   
  BR = (char)((temp & 0xF000) >> 12);
  Mp3Info->mode = (char)((temp & 0xC0) >> 6);   
  if (ID == 0 || ID == 1)
  {
    Mp3Info->sample_rate  =  sample_rate_values1[SR];
  }
  else if (ID == 2)
  {
    Mp3Info->sample_rate  =  sample_rate_values2[SR];
  }
  else if (ID == 3)
  {
    Mp3Info->sample_rate  =  sample_rate_values3[SR];
    Mp3Info->bit_rate = bit_rate_values1[BR];
  }
  else if (ID == 0 || ID == 1 || ID == 2)
  {
    Mp3Info->bit_rate = bit_rate_values2[BR];
  }
}   