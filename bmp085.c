/*
bmp085 lib 0x01

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>

#include <util/delay.h>
#include "bmp085.h"
#include "global.h"

#define i2time 10
//variables
int bmp085_regac1, bmp085_regac2, bmp085_regac3, bmp085_regb1, bmp085_regb2, bmp085_regmb, bmp085_regmc, bmp085_regmd;
unsigned int bmp085_regac4, bmp085_regac5, bmp085_regac6;
long bmp085_rawtemperature, bmp085_rawpressure;
const unsigned char OSS = 0;
int AC1, AC2, AC3, B1, B2, MB, MC, MD;
unsigned short AC4, AC5, AC6;
long B5;
//------------------------------------------------------------------------------
// 	Routine:	i2c_start
//	Inputs:		none
//	Outputs:	none
//	Purpose:	Sends I2C Start Trasfer - State "B"
//------------------------------------------------------------------------------
/*
/void SoftI2CStart()
{
	SOFT_I2C_SCL_HIGH;
	H_DEL;
	
	SOFT_I2C_SDA_LOW;
	H_DEL;
}
*/
void i2c_start (void)
{
	SET_OUTPUT(SDA);
	SET(SCL);
	_delay_us(i2time);
	RESET(SDA);
	_delay_us(i2time);
}
//------------------------------------------------------------------------------
// 	Routine:	i2c_stop
//	Inputs:		none
//	Outputs:	none
//	Purpose:	Sends I2C Stop Trasfer - State "C"
//------------------------------------------------------------------------------
/*
void SoftI2CStop()
{
	SOFT_I2C_SDA_LOW;
	H_DEL;
	SOFT_I2C_SCL_HIGH;
	Q_DEL;
	SOFT_I2C_SDA_HIGH;
	H_DEL;
}
*/
void i2c_stop (void)
{
	SET_OUTPUT(SDA);
	//gpio_configure_pin(SDA,GPIO_DIR_OUTPUT);
//	RESET(SCL);
//	gpio_clr_gpio_pin(SCL) ;	// Set clock line low
	RESET(SDA);
//	gpio_clr_gpio_pin(SDA);		// Set data line low (START SIGNAL)
	_delay_us(i2time);
	SET(SCL);
	//gpio_set_gpio_pin(SCL);   // Set clock line high
	_delay_us(i2time);
	SET(SDA);
//	gpio_set_gpio_pin(SDA);   // Set data line high
	_delay_us(i2time);
}

//------------------------------------------------------------------------------
// 	Routine:	i2c_write
//	Inputs:		output byte
//	Outputs:	none
//	Purpose:	Writes data over the I2C bus
//------------------------------------------------------------------------------
u8 i2c_write (unsigned char output_data)
{
	u8  index;
	SET_OUTPUT(SDA);
	for(index = 0; index < 8; index++)  	// Send 8 bits to the I2C Bus
	{
		RESET(SCL);
		_delay_us(i2time);
		// Output the data bit to the I2C Bus
		if ((output_data & 0x80)) SET(SDA); else  RESET(SDA);
		
		output_data  <<= 1;            		// Shift the byte by one bit
		_delay_us(i2time);
		SET(SCL);
		_delay_us(i2time);
		
	}
	RESET(SCL);
	_delay_us(i2time);
	SET_INPUT(SDA);
	SET(SDA);
	_delay_us(i2time);
	SET(SCL);
	_delay_us(i2time);
	if (IS_SET(SDA)) {_delay_us(i2time);}
	
	RESET(SCL);
	_delay_us(i2time);
	SET_OUTPUT(SDA);
	SET(SDA);

	return index;

}
void i2c_init(void)
{
	SET(SCL);
	SET_OUTPUT(SDA);
	SET(SDA);
}
void i2c_start_wait(u8 add)
{
	i2c_start();
	i2c_write(add);
}


void bmp085_writemem(uint8_t reg, uint8_t value) {
	i2c_start_wait(BMP085_ADDR | I2C_WRITE);
	i2c_write(reg);
	i2c_write(value);
	i2c_stop();
}
u8 i2c_readAck(void)
{	u8 a= i2c_read();
	RESET(SCL);
	SET_OUTPUT(SDA);
	RESET(SDA);
	_delay_us(i2time);
	SET(SCL);
	_delay_us(i2time);
	RESET(SCL);
	_delay_us(i2time);
	return a;
}
u8 i2c_readNak (void)
{

	u8 a= i2c_read();
	RESET(SCL);
	SET_OUTPUT(SDA);
	SET(SDA);
	_delay_us(i2time);
	SET(SCL);
	_delay_us(i2time);
	RESET(SCL);
	_delay_us(i2time);
	return (a);
	
}
void i2c_rep_start (u8 adr)
{
	i2c_start();
	i2c_write(adr);
}
u8 i2c_read (void)
{
	unsigned char index, input_data;
	_delay_us(i2time);
	input_data = 0x00;
	SET_INPUT(SDA);
	for(index = 0; index < 8; index++)  	// Send 8 bits to the I2C Bus
	{
		RESET(SCL);
		_delay_us(i2time);
		SET(SCL);
		_delay_us(i2time);
		input_data <<= 1;					// Shift the byte by one bit
		if (IS_SET(SDA)) input_data |= 1;
	}

	return input_data;
}

u8 bmp_idcheck(void)
{

	return (getDataFromBMP180Register(BMP085_REGID));
	//i2c_start_wait(BMP085_ADDR | I2C_WRITE);
	//i2c_write(BMP085_REGID);
	//return (i2c_readNak());
}
void bmp085_readmem(uint8_t reg, uint8_t buff[], uint8_t bytes) {

	uint8_t i =0;
	i2c_start_wait(BMP085_ADDR | I2C_WRITE);
	i2c_write(reg);
	i2c_rep_start(BMP085_ADDR | I2C_READ);
	for(i=0; i<bytes; i++) {
		if(i==bytes-1)
			buff[i] = i2c_readNak();
		else
			buff[i] = i2c_readAck();
	}
	i2c_stop();
	
}

u16 getDataFromBMP180Register(uint8_t loc)
{
	uint8_t data;
	
	i2c_start_wait(BMP180_WRITE);


	i2c_write(loc);
	
	i2c_stop();

	i2c_start_wait(BMP180_READ);
	data = i2c_readNak();
	
	i2c_stop();
		
	return data;
}

#if BMP085_FILTERPRESSURE == 1
#define BMP085_AVARAGECOEF 21
static long k[BMP085_AVARAGECOEF];


long bmp085_avaragefilter(long input) {
	uint8_t i=0;
	long sum=0;
	for (i=0; i<BMP085_AVARAGECOEF; i++) {
		k[i] = k[i+1];
	}
	k[BMP085_AVARAGECOEF-1] = input;
	for (i=0; i<BMP085_AVARAGECOEF; i++) {
		sum += k[i];
	}
	return (sum /BMP085_AVARAGECOEF) ;
}
#endif

/*
 * read calibration registers
 */
void bmp085_getcalibration() {
	uint8_t buff[2];
	memset(buff, 0, sizeof(buff));

	bmp085_readmem(BMP085_REGAC1, buff, 2);
	bmp085_regac1 = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGAC2, buff, 2);
	bmp085_regac2 = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGAC3, buff, 2);
	bmp085_regac3 = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGAC4, buff, 2);
	bmp085_regac4 = ((unsigned int)buff[0] <<8 | ((unsigned int)buff[1]));
	bmp085_readmem(BMP085_REGAC5, buff, 2);
	bmp085_regac5 = ((unsigned int)buff[0] <<8 | ((unsigned int)buff[1]));
	bmp085_readmem(BMP085_REGAC6, buff, 2);
	bmp085_regac6 = ((unsigned int)buff[0] <<8 | ((unsigned int)buff[1]));
	bmp085_readmem(BMP085_REGB1, buff, 2);
	bmp085_regb1 = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGB2, buff, 2);
	bmp085_regb2 = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGMB, buff, 2);
	bmp085_regmb = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGMC, buff, 2);
	bmp085_regmc = ((int)buff[0] <<8 | ((int)buff[1]));
	bmp085_readmem(BMP085_REGMD, buff, 2);
	bmp085_regmd = ((int)buff[0] <<8 | ((int)buff[1]));
		#ifdef debug
		bmp085_regac1=408;
		bmp085_regac2=-72;
		bmp085_regac3=-14383;
		bmp085_regac4=32741;
		bmp085_regac5=32757;
		bmp085_regac6=23153;
		bmp085_regb1=6190;
		bmp085_regb2=4;
		bmp085_regmc=-8711;
		bmp085_regmb=-32768;
		bmp085_regmd=2868;
		#endif
}

/*
 * get raw temperature as read by registers, and do some calculation to convert it
 */
void bmp085_getrawtemperature() {
	uint8_t buff[2];
	memset(buff, 0, sizeof(buff));
	long ut,x1,x2;

	//read raw temperature
	bmp085_writemem  (BMP085_REGCONTROL, BMP085_REGREADTEMPERATURE);
	_delay_ms(5); // min. 4.5ms read Temp delay
	bmp085_readmem(BMP085_REGCONTROLOUTPUT, buff, 2);
	ut = ((long)buff[0] << 8 | ((long)buff[1])); //uncompensated temperature value

	//calculate raw temperature
	x1 = ((long)ut - bmp085_regac6) * bmp085_regac5 >> 15;
	x2 = ((long)bmp085_regmc << 11) / (x1 + bmp085_regmd);
	bmp085_rawtemperature = x1 + x2;
}

/*
 * get raw pressure as read by registers, and do some calculation to convert it
 */
void bmp085_getrawpressure() {
	uint8_t buff[3];
	memset(buff, 0, sizeof(buff));
	long up,x1,x2,x3,b3,b6,p;
	unsigned long b4,b7;

	#if BMP085_AUTOUPDATETEMP == 1
	bmp085_getrawtemperature();
	#endif

	//read raw pressure
	bmp085_writemem(BMP085_REGCONTROL, BMP085_REGREADPRESSURE+(BMP085_MODE << 6));
	_delay_ms(2 + (3<<BMP085_MODE));
	bmp085_readmem(BMP085_REGCONTROLOUTPUT, buff, 3);
	up = ((((long)buff[0] <<16) | ((long)buff[1] <<8) | ((long)buff[2])) >> (8-BMP085_MODE)); // uncompensated pressure value

	//calculate raw pressure
	b6 = bmp085_rawtemperature - 4000;
	x1 = (bmp085_regb2* (b6 * b6) >> 12) >> 11;
	x2 = (bmp085_regac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = (((((long)bmp085_regac1) * 4 + x3) << BMP085_MODE) + 2) >> 2;
	x1 = (bmp085_regac3 * b6) >> 13;
	x2 = (bmp085_regb1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (bmp085_regac4 * (uint32_t)(x3 + 32768)) >> 15;
	b7 = ((uint32_t)up - b3) * (50000 >> BMP085_MODE);
	p = b7 < 0x80000000 ? (b7 << 1) / b4 : (b7 / b4) << 1;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	bmp085_rawpressure = p + ((x1 + x2 + 3791) >> 4);

	#if BMP085_FILTERPRESSURE == 1
	bmp085_rawpressure = bmp085_avaragefilter(bmp085_rawpressure);
	#endif
}

/*
 * get celsius temperature
 */
double bmp085_gettemperature() {
	//();
	double temperature = ((bmp085_rawtemperature + 8)>>4);
	return temperature;
}

/*
 * get pressure
 */
int32_t bmp085_getpressure() {
	bmp085_getrawpressure();
	
	return bmp085_rawpressure/100 + BMP085_UNITPAOFFSET;
}

/*
 * get altitude
 */
double bmp085_getaltitude() {
	bmp085_getrawpressure();
	return ((1 - pow(bmp085_rawpressure/(double)101325, 0.1903 )) / 0.0000225577) + BMP085_UNITMOFFSET;
}

/*
 * init bmp085
 */
char bmp085_init() {
	#if BMP085_I2CINIT == 1
	//init i2c
	i2c_init();
	_delay_us(10);
	#endif
	bmp_idcheck();
	
	u8 ii;
	for (ii=0;ii<10;ii++) {while (bmp_idcheck()!=0x55) {BlinkFast(3);_delay_ms(1000);}}
	 //return (bmp_idcheck());
//	calibrate_bmp180();
	bmp085_getcalibration(); //get calibration data
	bmp085_getrawtemperature(); //update raw temperature, at least the first time

	#if BMP085_FILTERPRESSURE == 1
	//initialize the avarage filter
	uint8_t i=0;
	for (i=0; i<BMP085_AVARAGECOEF; i++) {
		bmp085_getrawpressure();
	}
	#endif
	
	return (-1);
}
