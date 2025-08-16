
#include "includes.h"
#include "defines.h"
#include "my_delay.h"
#include "global_var.h"
#include "ADXL345.h"

#include <math.h>



/*********************************************************************/

unsigned char read_i2c(unsigned char BusAddres , unsigned char Reg , unsigned char Ack )
{
    unsigned char Data;
    
    HAL_I2C_Mem_Read(&hi2c1,BusAddres,Reg,I2C_MEMADD_SIZE_8BIT,&Data,1,i2c_timeout);
        
    return Data;
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void write_i2c(unsigned char BusAddres , unsigned char Reg , unsigned char Data)
{
  HAL_I2C_Mem_Write(&hi2c1,BusAddres,Reg,I2C_MEMADD_SIZE_8BIT,&Data,1,i2c_timeout);
  
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
unsigned char GetDeviceID(void)
{
    return read_i2c(ADXL345_Addr,RA_DEVID,0);
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void Set_X_Offset(unsigned char X_Val)
{
    write_i2c(ADXL345_Addr,RA_OFSX,X_Val);
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void Set_Y_Offset(unsigned char Y_Val)
{
    write_i2c(ADXL345_Addr,RA_OFSY,Y_Val);
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void Set_Z_Offset(unsigned char Z_Val)
{
    write_i2c(ADXL345_Addr,RA_OFSZ,Z_Val);
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void Get_Offset_Values(void)
{
	Offset_X_Val = read_i2c(ADXL345_Addr,RA_OFSX,0);
    Offset_Y_Val = read_i2c(ADXL345_Addr,RA_OFSY,0);
    Offset_Z_Val = read_i2c(ADXL345_Addr,RA_OFSZ,0);
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void Get_Accel_Values(void)
{
    Get_Offset_Values();

    Accel_Xout_Val = ( (read_i2c(ADXL345_Addr,RA_DATAX1,0)<<8) | read_i2c(ADXL345_Addr,RA_DATAX0,0) )-Offset_X_Val;
    Accel_Yout_Val = ( (read_i2c(ADXL345_Addr,RA_DATAY1,0)<<8) | read_i2c(ADXL345_Addr,RA_DATAY0,0) )-Offset_Y_Val;
	Accel_Zout_Val = ( (read_i2c(ADXL345_Addr,RA_DATAZ1,0)<<8) | read_i2c(ADXL345_Addr,RA_DATAZ0,0) )-Offset_Z_Val;
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void Get_Averge_Accel_Values(void)
{
  #define    NumAve      20
  unsigned long int Ave=0;
  unsigned char i=NumAve;

  Get_Offset_Values();

  while(i--)
  {
	Accel_Xout_Val = ( (read_i2c(ADXL345_Addr,RA_DATAX1,0)<<8) | read_i2c(ADXL345_Addr,RA_DATAX0,0) )-Offset_X_Val;
    Ave+=Accel_Xout_Val;
  }
  Accel_Xout_Val=Ave/NumAve;
  Ave=0;
  i=NumAve;
  while(i--)
  {
	Accel_Yout_Val = ( (read_i2c(ADXL345_Addr,RA_DATAY1,0)<<8) | read_i2c(ADXL345_Addr,RA_DATAY0,0) )-Offset_Y_Val;
    Ave+=Accel_Yout_Val;
  }
  Accel_Yout_Val=Ave/NumAve;
  Ave=0;
  i=NumAve;
  while(i--)
  {
	Accel_Zout_Val = ( (read_i2c(ADXL345_Addr,RA_DATAZ1,0)<<8) | read_i2c(ADXL345_Addr,RA_DATAZ0,0) )-Offset_Z_Val;
    Ave+=Accel_Zout_Val;
  }
  Accel_Zout_Val=Ave/NumAve;
  Ave=0;

}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void Get_Accel_Angles(void)
{
    Get_Accel_Values();
//  Convert Unsigned Int To Int
    Accel_Xout_Normalized_Val = Accel_Xout_Val;
    Accel_Yout_Normalized_Val = Accel_Yout_Val;
    Accel_Zout_Normalized_Val = Accel_Zout_Val;
//  Calculate The Angle Of Each Axis
	Accel_XAngle = 57.295*atan((float) Accel_Xout_Normalized_Val / sqrt(pow((float)Accel_Zout_Normalized_Val,2)+pow((float)Accel_Yout_Normalized_Val,2)));
	Accel_YAngle = 57.295*atan((float) Accel_Yout_Normalized_Val / sqrt(pow((float)Accel_Zout_Normalized_Val,2)+pow((float)Accel_Xout_Normalized_Val,2)));
  	Accel_ZAngle = 57.295*atan((float) sqrt(pow((float)Accel_Xout_Normalized_Val,2)+pow((float)Accel_Yout_Normalized_Val,2))/ Accel_Zout_Normalized_Val );
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void ADXL345_SelfTest(void)
{
    //  VS=ON   VDDi/o=ON
    delayms(2);
    //  INITIAL COMMAND SEQUENCE
    write_i2c(ADXL345_Addr,RA_DATA_FORMAT,0x0B);   //  +/-16g , 13-BIT MODE
    write_i2c(ADXL345_Addr,RA_POWER_CTL,0x08);     //  START MEASUREMENT
    write_i2c(ADXL345_Addr,RA_INT_ENABLE,0x80);    //  ENABLE DATA_READY INTERRUPT

    delayms(2);

    Get_Accel_Values();
    Get_Accel_Values();
    Get_Accel_Values();

    //  ACTIVATE SELF-TEST
    write_i2c(ADXL345_Addr,RA_DATA_FORMAT,0x8B);   //  +/-16g , 13-BIT MODE , SELF TEST ON

    delayms(2000);

    Get_Accel_Values();
    Get_Accel_Values();
    Get_Accel_Values();
    //  CALCULATE SELF-TESTDELTA AND COMPARE IT TO DATASHEET LIMITS
    delayms(4000);

    //  INACTIVATE SELF-TEST
    write_i2c(ADXL345_Addr,RA_DATA_FORMAT,0x0B);   //  +/-16g , 13-BIT MODE , SELF TEST OFF
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void ADXL345_Offset_Calib(void)
{
  #ifndef NumAve
    #define    NumAve      10
  #endif
  unsigned long int Ave=0;
  unsigned char i=NumAve;

    //  PLACE SENSOR IN  X=0g, Y=0g, Z=+1g  ORIENTATIONVS
    //  VS=ON   VDDi/o=ON
    delayms(2);

    write_i2c(ADXL345_Addr,RA_DATA_FORMAT,0x0B);   //  +/-16g , 13-BIT MODE
    write_i2c(ADXL345_Addr,RA_POWER_CTL,0x08);     //  START MEASUREMENT
    write_i2c(ADXL345_Addr,RA_INT_ENABLE,0x80);    //  ENABLE DATA_READY INTERRUPT

    delayms(12);

  //  Get Average of Accel For Each Axis
  while(i--)
  {
	Accel_Xout_Val = ((read_i2c(ADXL345_Addr,RA_DATAX1,0)<<8)|
                       read_i2c(ADXL345_Addr,RA_DATAX0,0)    );
    Ave += Accel_Xout_Val;
  }
  Accel_Xout_Val = Ave / NumAve;
  Ave = 0;
  i = NumAve;
  while(i--)
  {
	Accel_Yout_Val = ((read_i2c(ADXL345_Addr,RA_DATAY1,0)<<8)|
                       read_i2c(ADXL345_Addr,RA_DATAY0,0)    );
    Ave += Accel_Yout_Val;
  }
  Accel_Yout_Val = Ave / NumAve;
  Ave = 0;
  i = NumAve;
  while(i--)
  {
	Accel_Zout_Val = ((read_i2c(ADXL345_Addr,RA_DATAZ1,0)<<8)|
                       read_i2c(ADXL345_Addr,RA_DATAZ0,0)    );
    Ave += Accel_Zout_Val;
  }
  Accel_Zout_Val = Ave / NumAve;
  Ave=0;

    //  CALCULATE CALIBRATION VALUE
    Offset_X_Val = -(Accel_Xout_Val/4.0);
    Offset_Y_Val = -(Accel_Yout_Val/4.0);
    Offset_Z_Val = -((Accel_Zout_Val-256)/4.0);

    //  WRITE TO OFSTx REGISTERS
    Set_X_Offset(Offset_X_Val);
    Set_Y_Offset(Offset_Y_Val);
    Set_Z_Offset(Offset_Z_Val);
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void ADXL345_Init(void)
{
    write_i2c(ADXL345_Addr,RA_DATA_FORMAT,0x0B);   //  +/-16g , 13-BIT MODE
    write_i2c(ADXL345_Addr,RA_POWER_CTL,0x08);     //  START MEASUREMENT
    write_i2c(ADXL345_Addr,RA_INT_ENABLE,0x80);    //  ENABLE DATA_READY INTERRUPT
}
////|/============ S_Ahmad  >>>>  www.RoboticNGO.com  >>>>  ADXL345 Lib  /|/|/|/|/
void ADXL345_Regs_Reset(void)
{
    //Set Register to default value.

    write_i2c(ADXL345_Addr,RA_THRESH_TAP,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_OFSX,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_OFSY,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_OFSZ,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_DUR,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_Latent,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_Window,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_THRESH_ACT,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_THRESH_INACT,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_TIME_INACT,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_ACT_INACT_CTL,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_THRESH_FF,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_TIME_FF,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_TAP_AXES,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_BW_RATE,0xA0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_POWER_CTL,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_INT_ENABLE,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_INT_MAP,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_DATA_FORMAT,0);
    delayms(50);
    write_i2c(ADXL345_Addr,RA_FIFO_CTL,0);
    delayms(50);

    //Registers Reset successfully, you can use accelerometer now.
}


