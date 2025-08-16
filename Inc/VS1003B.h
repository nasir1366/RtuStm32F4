

#ifndef __VS1003B_H__
#define __VS1003B_H__

#include "includes.h"   
                                       
                                         
#define _BV(pin)  (1<<(pin))
#define _delay_ms	delayms
                                             // tanzimateeee bass va equlazer va clock sample rate
#define TREMBLE_VALUE	8		/* 0~15, 8 means off  */
#define TREMBLE_LOW_FS	8		/* 0~15, 0Hz-15KHz, lower frequency of tremble enhancement */
#define BASS_VALUE	0		/* 0~15, 0 means off  */
#define BASS_HIGH_FS	8		/* 2~15, up limit frequency of bass enhancement */

#define VS1003_AUDATA			0x05
#define VS1003_HEDATA0			0x08
#define VS1003_HEDATA1			0x09
#define VS1003_VOLUME	        0x0B

#define DEFAULT_BASS_TREMBLE ((TREMBLE_VALUE<<12)|(TREMBLE_LOW_FS<<8)|(BASS_VALUE<<4)|(BASS_HIGH_FS))

#define DEFAULT_VOLUME 0x3333	/* default volumn */

#define CLOCK_REG         0xE000       //SCI CLOCKF = 0xFFD0  SC MULT = 2  SC ADD = 0  SC FREQ = 0x7D0 = 2000 XTALI = 2000*4000+8000000 = 16 MHz                                  
//    CLOCK_REG = 0x47D0 = 0b010 00 11111010000
//                           \\\ \\ \\\\\\\\\\\
//                           \\\ \\            ----> SC FREQ 00 ~ 10  Clock frequency                   2000 = 11111010000
//                           \\\   ----------------> SC ADD  11 ~ 12  Allowed multiplier addition       0    = 00
//                              -------------------> SC MULT 13 ~ 15  Clock multiplier                  2    = 010

typedef struct _AUDIO_DATA
        {
	unsigned int bitrate;
	unsigned int sample_rate;
	unsigned char is_stereo;
        }AUDIO_DATA;
        
typedef struct VS1003_Mp3Info
        {
	char sample_rate;
	char bit_rate; 
        char mode;
        }AUDIO_INFO;



#define VS1003B_XCS_PORT GPIOB->ODR
#define VS1003B_XCS_BIT  12


#define VS1003B_DREQ_PIN  GPIOF->IDR
#define VS1003B_DREQ_BIT  3


#define VS1003B_XDCS_PORT GPIOF->ODR
#define VS1003B_XDCS_BIT  2


#define VS1003B_XRESET_PORT GPIOF->ODR
#define VS1003B_XRESET_BIT  1





/********************************************************************************/

/* port initialization */
#define PORT_INI()
   

#define VS1003B_XCS_H()         VS1003B_XCS_PORT |=  _BV(VS1003B_XCS_BIT)
#define VS1003B_XCS_L()         VS1003B_XCS_PORT &= ~_BV(VS1003B_XCS_BIT)
#define VS1003B_XRESET_H()      VS1003B_XRESET_PORT |=  _BV(VS1003B_XRESET_BIT)
#define VS1003B_XRESET_L()      VS1003B_XRESET_PORT &= ~_BV(VS1003B_XRESET_BIT)
#define VS1003B_XDCS_H()        VS1003B_XDCS_PORT |=  _BV(VS1003B_XDCS_BIT)
#define VS1003B_XDCS_L()        VS1003B_XDCS_PORT &= ~_BV(VS1003B_XDCS_BIT)

/*****************************************************************************************/

unsigned char VS1003B_Init(void);        /* Initialization, include prot and chip config */
void VS1003B_SoftReset(void);                /* Software reset, clear decode time and jump out midi or wma playing*/
unsigned int VS1003B_ReadDecodeTime(void);        /* Get current decode time, for time display */
void VS1003B_Fill2048Zero(void);        /* fill 32bytes zero, for wma and midi zero filling */
void VS1003B_Write32B(unsigned char * buf);        /* write 32bytes data */
unsigned char VS1003B_NeedData(void);        /* Check if it need data */
void VS1003B_SetVolume(unsigned int vol); /* Set volume */

/****************************************************************************************/
#endif
