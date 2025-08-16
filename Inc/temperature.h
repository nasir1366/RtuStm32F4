#ifndef _TEMP_INCLUDED_
#define _TEMP_INCLUDED_


#define lm75_adr 0x90    //hardware address is 0 . A0 & A1 & A2 are connected to GND  : 1-0-0-1-A2-A1-A0-R/W        for  write on lm75 R/W=0

#define lm75_temp 0x00

void temp_lm75(signed char*);
void ReadExtTemp(signed char*);

 
 #endif
 