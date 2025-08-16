#ifndef ADXL345
#define ADXL345


#define     ADXL345_Addr    0xA6

//ADXL345 REGISTER ADDRESS MAP:
#define     RA_DEVID            0x00    // R     0xE5   Device ID
#define     RA_THRESH_TAP       0x1D    // R/W   0      Tap threshold
#define     RA_OFSX             0x1E    // R/W   0      X-axis offset
#define     RA_OFSY             0x1F    // R/W   0      Y-axis offset
#define     RA_OFSZ             0x20    // R/W   0      Z-axis offset
#define     RA_DUR              0x21    // R/W   0      Tap duration
#define     RA_Latent           0x22    // R/W   0      Tap latency
#define     RA_Window           0x23    // R/W   0      Tap window
#define     RA_THRESH_ACT       0x24    // R/W   0      Activity threshold
#define     RA_THRESH_INACT     0x25    // R/W   0      Inactivity threshold
#define     RA_TIME_INACT       0x26    // R/W   0      Inactivity time
#define     RA_ACT_INACT_CTL    0x27    // R/W   0      Axis enable control for activity and inactivity detection
#define     RA_THRESH_FF        0x28    // R/W   0      Free-fall threshold
#define     RA_TIME_FF          0x29    // R/W   0      Free-fall time
#define     RA_TAP_AXES         0x2A    // R/W   0      Axis control for single tap/double tap
#define 	RA_ACT_TAP_STATUS   0x2B 	// R 	 0      Source of single tap/double tap
#define 	RA_BW_RATE 	        0x2C 	// R/W 	 0x0A 	Data rate and power mode control
#define 	RA_POWER_CTL 	    0x2D 	// R/W 	 0   	Power-saving features control
#define 	RA_INT_ENABLE 	    0x2E 	// R/W 	 0 	    Interrupt enable control
#define 	RA_INT_MAP 	        0x2F 	// R/W 	 0 	    Interrupt mapping control
#define 	RA_INT_SOURCE 	    0x30 	// R 	 0x02 	Source of interrupts
#define 	RA_DATA_FORMAT 	    0x31 	// R/W 	 0   	Data format control
#define 	RA_DATAX0 	        0x32 	// R 	 0 	    X-Axis Data 0
#define 	RA_DATAX1 	        0x33 	// R 	 0 	    X-Axis Data 1
#define 	RA_DATAY0 	        0x34 	// R 	 0 	    Y-Axis Data 0
#define 	RA_DATAY1 	        0x35 	// R 	 0 	    Y-Axis Data 1
#define 	RA_DATAZ0 	        0x36 	// R 	 0 	    Z-Axis Data 0
#define 	RA_DATAZ1 	        0x37 	// R 	 0 	    Z-Axis Data 1
#define 	RA_FIFO_CTL 	    0x38 	// R/W 	 0 	    FIFO control
#define 	RA_FIFO_STATUS 	    0x39 	// R 	 0 	    FIFO status

/*****************************************************************************************/

unsigned char read_i2c(unsigned char BusAddres , unsigned char Reg , unsigned char Ack );
void write_i2c(unsigned char BusAddres , unsigned char Reg , unsigned char Data);
unsigned char GetDeviceID(void);
void Set_X_Offset(unsigned char X_Val);
void Set_Y_Offset(unsigned char Y_Val);
void Set_Z_Offset(unsigned char Z_Val);
void Get_Offset_Values(void);
void Get_Accel_Values(void);
void Get_Averge_Accel_Values(void);
void Get_Accel_Angles(void);
void ADXL345_SelfTest(void);
void ADXL345_Offset_Calib(void);
void ADXL345_Init(void);
void ADXL345_Regs_Reset(void);





















#endif