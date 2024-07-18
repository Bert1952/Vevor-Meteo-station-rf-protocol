/*
bmp085 lib 0x01

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.

References:
  - this library is a porting of the bmp085driver 0.4 ardunio library
    http://code.google.com/p/bmp085driver/

Notes:
  - to compile with avrgcc you may define -lm flag for some math functions
*/


#ifndef BMP085_H_
#define BMP085_H_

//#include <stdio.h>
//#include <avr/io.h>


//BMP180 Defines and Variables
#define TW_WRITE 0
#define TW_READ 1
#define BMP180_ADDR 0b11101110
#define BMP180_WRITE (BMP180_ADDR|TW_WRITE)
#define BMP180_READ (BMP180_ADDR|TW_READ)
#define BMP180_AC1_MSB 0xAA
#define BMP180_AC1_LSB 0xAB
#define BMP180_AC2_MSB 0xAC
#define BMP180_AC2_LSB 0xAD
#define BMP180_AC3_MSB 0xAE
#define BMP180_AC3_LSB 0xAF
#define BMP180_AC4_MSB 0xB0
#define BMP180_AC4_LSB 0xB1
#define BMP180_AC5_MSB 0xB2
#define BMP180_AC5_LSB 0xB3
#define BMP180_AC6_MSB 0xB4
#define BMP180_AC6_LSB 0xB5
#define BMP180_B1_MSB 0xB6
#define BMP180_B1_LSB 0xB7
#define BMP180_B2_MSB 0xB8
#define BMP180_B2_LSB 0xB9
#define BMP180_MB_MSB 0xBA
#define BMP180_MB_LSB 0xBB
#define BMP180_MC_MSB 0xBC
#define BMP180_MC_LSB 0xBD
#define BMP180_MD_MSB 0xBE
#define BMP180_MD_LSB 0xBF



#define I2C_WRITE 0
#define I2C_READ 1
#define BMP085_ADDR (0x77<<1) //0x77 default I2C address
#define BMP085_I2CINIT 1 //init i2c
//registers
#define BMP085_REGAC1 0xAA
#define BMP085_REGAC2 0xAC
#define BMP085_REGAC3 0xAE
#define BMP085_REGAC4 0xB0
#define BMP085_REGAC5 0xB2
#define BMP085_REGAC6 0xB4
#define BMP085_REGB1 0xB6
#define BMP085_REGB2 0xB8
#define BMP085_REGMB 0xBA
#define BMP085_REGMC 0xBC
#define BMP085_REGMD 0xBE
#define BMP085_REGID 0xD0
#define BMP085_REGCONTROL 0xF4 //control
#define BMP085_REGCONTROLOUTPUT 0xF6 //output 0xF6=MSB, 0xF7=LSB, 0xF8=XLSB
#define BMP085_REGREADTEMPERATURE 0x2E //read temperature
#define BMP085_REGREADPRESSURE 0x34 //read pressure

//modes
#define BMP085_MODEULTRALOWPOWER 0 //oversampling=0, internalsamples=1, maxconvtimepressure=4.5ms, avgcurrent=3uA, RMSnoise_hPA=0.06, RMSnoise_m=0.5
#define BMP085_MODESTANDARD 1 //oversampling=1, internalsamples=2, maxconvtimepressure=7.5ms, avgcurrent=5uA, RMSnoise_hPA=0.05, RMSnoise_m=0.4
#define BMP085_MODEHIGHRES 2 //oversampling=2, internalsamples=4, maxconvtimepressure=13.5ms, avgcurrent=7uA, RMSnoise_hPA=0.04, RMSnoise_m=0.3
#define BMP085_MODEULTRAHIGHRES 3 //oversampling=3, internalsamples=8, maxconvtimepressure=25.5ms, avgcurrent=12uA, RMSnoise_hPA=0.03, RMSnoise_m=0.25

//autoupdate temperature enabled
#define BMP085_AUTOUPDATETEMP 1 //autoupdate temperature every read

//setup parameters
#define BMP085_MODE BMP085_MODEHIGHRES //define a mode
#define BMP085_UNITPAOFFSET -8 //define a unit offset (pa)
#define BMP085_UNITMOFFSET 0 //define a unit offset (m)

//avarage filter
#define BMP085_FILTERPRESSURE 0 //avarage filter for pressure



//functions
char bmp085_init();
int32_t bmp085_getpressure();
double bmp085_getaltitude();
double bmp085_gettemperature();
u8 i2c_write(u8 reg);
void i2c_start_wait(u8 add);
void i2c_init(void);
u8 i2c_readNak (void);
void i2c_rep_start (u8);
u8 i2c_readAck(void);
u8 i2c_read (void);
u8 bmp_idcheck(void);
void calibrate_bmp180(void);
long getPressure(void); // Returns pressure in Pascals;
uint32_t getTemp(void); // Returns temperature in degrees C;
u16 getDataFromBMP180Register(uint8_t loc);
long getUcTemp(void);
uint32_t getTrueTemp(long UT);
unsigned long getUcPressure(void);
long getTruePressure(unsigned long UP);
#endif
