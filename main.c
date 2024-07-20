/*
* meteorepeater.c
*
* Created: 29-6-2024 19:23:37
* Author : bertv
*/

#include <avr/io.h>
#include <inttypes.h>
#include "main.h"
#include <util/delay.h>
#include "global.h"
#include "defaults.h"
#include <string.h>
#include "rfm23b.h"
#include <avr/wdt.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include "tlkRH.h"
#include <avr/boot.h>
#include "global.h"
#include "defaults.h"
#include "bmp085.h"
#define wlen 35
#define prepos 40 //35 geeft 125 fouten in 2uur
u8 weatherdata[wlen];

u16 checksum=0;
u8 txrdpointer=0;
u8 txwrpointer=0;//
extern u8 txbuffer[];
char txuartbuffer[256];
char firstchar=0;
int temp=20;  //wordt 2 byte, temp is maal 10
int tempold=20;
u8	hum=11;  //als hex getal is in %
u16 waverage=10;  //km/uur
u16 gust=16; //km/uur
u16 direction=181; //wordt 360
u16 totalrain=2001;

int32_t light=0;
u16 hpa=1024;
u8	dew=55;
u8 tx_power=0;
//char dtemp[200];
//char buffer[30];
static u8 start=0;
static u8 wp=0;
static u16 wbyte=0;
static u8 wcount=0;
//u16 pt=0;
//u16 spl=0;
u8 bittime=90;
//u16 leadingpuls=0;
RAIN   Regendata;
/*
14,AA,00,24,0D,1E,02,D0,45,01,1C,04,01,55,01,1A,05,96,95,E3,B5,E4,C1,B2,65,9A,74,39,C7,38,C9---85,220,3,2,69,213-0
14,AA,00,24,0D,1E,02,D8,42,01,01,01,01,56,01,1A,06,98,4D,58,CD,59,C1,B2,65,9A,74,39,C7,38,C9---86,228,0,0,66,74-0
14,AA,00,24,0D,1E,02,DA,40,01,01,00,01,57,01,1A,06,99,13,7F,BB,80,C0,B2,65,9A,74,31,C7,38,C9---87,230,0,0,64,113-0
14,AA,00,24,0D,1E,02,DD,40,01,01,01,01,57,01,1A,06,98,AD,86,7F,A7,C1,B2,65,9A,74,39,C7,38,C9---87,233,0,0,64,152-0
14,AA,00,24,0D,1E,02,DA,40,01,06,01,01,57,00,1A,06,98,88,F4,CA,F5,C1,B2,65,9A,74,39,C7,38,C9---87,230,0,0,64,230-0
14,AA,00,24,0D,1E,02,DA,41,01,0E,02,01,8D,01,1A,06,98,61,42,11,43,C1,B2,65,9A,74,39,C7,38,C9---141,230,1,1,65,52-0
14,AA,00,24,0D,1E,02,D0,41,01,1A,07,01,93,01,1A,05,94,C6,DE,1C,DF,C1,B2,65,9A,74,39,C7,38,C9---147,220,3,5,65,208-0
14,AA,00,24,0D,1E,02,CC,42,01,26,0A,01,A6,01,1A,05,95,BF,2C,12,2D,C1,B2,65,9A,74,39,C7,38,C9---166,216,4,7,66,30-0
14,AA,00,24,0D,1E,02,CD,43,01,2E,0B,01,B5,01,1A,05,95,6E,53,71,54,C1,B2,65,9A,74,39,C7,38,C9---181,217,5,8,67,69-0
14,AA,00,24,0D,1E,02,D6,3F,01,0B,01,01,BC,01,1A,06,99,3A,16,E4,17,C1,B2,65,9A,74,39,C7,38,C9---188,226,1,0,63,8-0
14,8A,00,24,0D,1E,02,B8,3F,01,07,02,01,BC,01,1A,06,98,A7,3D,76,3E,C1,B2,E5,9A,74,39,C7,38,C9---188,196,0,1,63,79-0
14,AA,00,24,0D,1E,02,DC,3E,01,01,02,01,BB,01,1A,06,98,D2,64,C4,65,C1,A2,65,9A,74,39,C7,38,C9---187,232,0,1,62,86-0
14,AA,00,24,0D,1E,02,DB,3F,01,09,03,01,B5,01,1A,06,98,48,D9,B2,DA,C1,B2,65,9A,74,39,C7,38,C9---181,231,1,2,63,203-0
14,AA,00,24,0D,1E,02,D4,42,01,26,07,01,44,01,1A,06,98,09,75,BB,76,C1,B2,65,9A,74,39,C7,38,C9---68,224,4,5,66,103-0
14,A2,00,24,0D,1E,02,D3,41,01,19,05,01,31,01,1A,05,96,DF,9C,93,8D,C1,B2,65,9A,74,38,C7,38,C9---49,223,2,3,65,134-0
14,AA,00,24,0D,1E,02,D8,41,01,16,05,01,34,01,1A,05,96,77,C3,55,C4,C1,B2,65,9A,74,39,C7,38,C9---52,228,2,3,65,181-0
14,AA,00,24,0D,1E,02,DA,C0,01,0E,03,01,3E,01,1A,05,95,D4,EA,D9,EB,C1,B2,65,9A,64,39,C7,38,C9---62,230,1,2,192,220-0
14,AA,00,24,0D,1E,02,DB,3F,01,0A,03,01,3C,01,1A,04,94,87,11,B3,12,C1,B2,65,9A,74,39,C7,38,C9
*/

void PrepareMessage(void)
{

	txbuffer[packetpos]=meteo_packet; //energy code
	txbuffer[test_pos]=((direction>>1)&0x80) | weatherdata[16];
	txbuffer[average_pos]=(u8) waverage;
	txbuffer[gust_pos]=(u8) gust;
	txbuffer[directon_pos]=(direction>>1) & 0xff;
	txbuffer[batt_voltage]=ReadAD();
	txbuffer[temp_pos]= temp>>8;
	txbuffer[temp_pos+1]=temp & 0xff;
	txbuffer[hum_pos]=hum;
	txbuffer[rain_pos]=Regendata.rainhour;
	txbuffer[totalrain_pos]=totalrain>>8;
	txbuffer[totalrain_pos+1]=totalrain & 0xff;
	txbuffer[pressure_pos]=hpa>>8;
	txbuffer[pressure_pos+1]=hpa & 0xff;
	dew=(int) tlkRH_calculate_dewpoint(hum,temp/10);
	if (dew>100) dew=100;
	txbuffer[dew_pos]=dew;
}
void SPI_MasterInit(void)
{
	
	/* Set MOSI and SCK output, all others input */
	SET_OUTPUT(MOSI);
	SET(MOSI);
	SET_INPUT(MISO);
	SET_OUTPUT(SCLK);
	SET(SCLK);
	/* Enable SPI, Master, set clock rate fck/16 */
	SET_OUTPUT(nSel);
	SET(nSel);
	SPCR = (1<<SPE)|(1<<MSTR)| (0<<DORD); //1us speed (1<<SPR0)|
}

u8 SPI_MasterTransmit_Command(u8 cCommand)
{
	//return (SPI_transceive(cCommand)); //<<-----soft
	/* Start transmission */
	SPDR = cCommand;
	/* set dc and cs low to send command */
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}
/*
void Blink(char nr)
{
char i;
for (i=0;i<nr;i++) {SET(Led);
_delay_ms(400);
RESET(Led);
_delay_ms(400);}
}
void BlinkFast(char nr)
{
char i;
for (i=0;i<nr;i++) {SET(Led);
_delay_ms(40);
RESET(Led);
_delay_ms(40);}
}
*/
void Init(void)
{
	SET_OUTPUT(Led);
	SET_OUTPUT(PWROFF);
	SET(PWROFF);
	SET(Led);
	SET_INPUT(Int0);
	//SET(Int0);
	SET(Int1);
	SET_INPUT(Int1);
	SET_OUTPUT(SCL);
	SET_OUTPUT(SDA);
	RESET(SCL);
	RESET(SDA);
	SET_OUTPUT(SDA);
	SET_OUTPUT(TXE);
	RESET(TXE);
	SET_INPUT(INP);
	RESET(INP);
	SET_OUTPUT(HUMA);
	RESET(HUMA);
	SET_OUTPUT(HUMB);
	RESET(HUMB);
	SET_INPUT(ANA1);
	
	SET_OUTPUT(BARO);
	RESET(BARO);
	SET_OUTPUT(COMPASS);
	RESET(COMPASS);
	
	SET_OUTPUT(MOSI);
	SET(MOSI);
	SET_INPUT(MISO);
	SET_OUTPUT(SCLK);
	SET(SCLK);
	SET_OUTPUT(nSel);
	SET(nSel);
	Regendata.lastreading=0;
	Regendata.rainhour=0;
	Regendata.actualreading=0;
	Regendata.lastcounter=0;
	Regendata.aligncounters=0;
	#if uart
	#else
	
	SET_OUTPUT(RX);
	
	//tx_power=1;
	#endif
}

void InitTimer1(void)
{
	TCCR1A=0;
	TCCR1B=(1<<CS11 ); //prescaler 128 1<<CS11;
	TIFR1 = 1<<TOV1; //Clear TOV1 / clear pending interrupts  timer0
	TCNT1=0;
}

void InitTimer0(void)
{
	TCCR0A=0;
	TCCR0B=(1<<CS01 ); //prescaler 128 1<<CS11;
	TIFR0 = 1<<TOV0; //Clear TOV1 / clear pending interrupts  timer0
	TCNT0=0xff-85;
	TIMSK0=1<<TOIE0;
}
ISR(TIMER1_OVF_vect) {
	static u16 c=0;

	c++;
	if (c==2700) {
		c=0;

		Regendata.verschil=(totalrain-Regendata.lastcounter);
		Regendata.lastcounter=totalrain;
		if (Regendata.verschil>0) {
			Regendata.rainhour=Regendata.verschil;
			
		}

		
	}
	
}
ISR(TIMER0_OVF_vect)
{
	if (IS_SET(HUMB)) {
		TCNT0=bittime;
		wbyte<<=1;
		if (IS_SET(Int0)) {wbyte|=1;}
		wcount++;
		if (wcount==8) {wcount=0;weatherdata[wp++]=wbyte;wbyte=0;}
		if (wp>22){RESET(HUMB);start=0; if (CheckData()==1) SET(Led);}
	}
}

ISR(INT0_vect)
{
	#define msampels 30
	
	static u16 OldTnct=0;
	static u8 preamble=0;
	static u16 pretime=0;
	u16 Pulse;
	Pulse=TCNT1-OldTnct;
	OldTnct=TCNT1;
	switch (start)
	{
		case 0: if (Pulse>1300 && Pulse<1400) {start=1;preamble=0;pretime=0;RESET(HUMB);}
		break;
		case 1: if (Pulse>30 && Pulse<140) {preamble++;pretime+=Pulse;} else {start=0;preamble=0;pretime=0;}
		if (preamble>msampels  && pretime>2600 && pretime<2800) {bittime=0xff-(pretime/msampels)+8;start=2;wp=0;wbyte=0;wcount=2;return;}
		break;
		case 2:case 3:case 8: case 9:case 13:if (Pulse>140 && Pulse<250) {start++;} else {}break;
		case 4:case 5:case 6: case 7:case 10:case 11: case 12: if (Pulse<140) {start++;} else {start=0;} break;
		case 14: SET(HUMB);
		TCNT0=0xff-prepos;
		TIFR0 = 1<<TOV0;
		start=15;break;
		case 15: if (!IS_SET(Int0)) TCNT0=0xff-prepos;
		//
		break;
	}
}

int main(void)

{
	Init();
	SPI_MasterInit();
	SET(Led);
	USART_Init(25);
	RESET(PWROFF);
	RF22B_INIT();
	
	PrepareMessage();
	SendDataRfm22b();
	RESET(Led);
	RFM22_OOK_INIT ();
	EICRA=1;
	EIMSK|=1;
	InitTimer1();
	InitTimer0();

	usart_pstr("start\r\n");

	SET_OUTPUT(HUMA);
	RESET(HUMA);
	SET_OUTPUT(HUMB);
	SET(HUMB);

	sei();

	while (1)
	{
		if (IS_SET(Led))
		{
			_delay_ms(255);
			start=0;
			Pout();
			RF22B_INIT();
			PrepareMessage();
			SendDataRfm22b();
			RFM22_OOK_INIT ();
			RESET(Led);
		}
		if (start>2){SET(HUMA);} else RESET(HUMA);
	}

	
}



void Pout (void)
{
	char dtemp[200];

	//	sprintf(dtemp,"%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X=%02X=%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X---%d,%d,%d,%d,%d,%d-%d\r\n",weatherdata[0],weatherdata[1],weatherdata[2],weatherdata[3],weatherdata[4],weatherdata[5],weatherdata[6],weatherdata[7],weatherdata[8],weatherdata[9],weatherdata[10],weatherdata[11],weatherdata[12],weatherdata[13],weatherdata[14],weatherdata[15],weatherdata[16],weatherdata[17],weatherdata[18],weatherdata[19],weatherdata[20],weatherdata[21],weatherdata[22],weatherdata[23],weatherdata[24],weatherdata[25],weatherdata[26],weatherdata[27],weatherdata[28],weatherdata[29],weatherdata[30],direction,temp,waverage,gust,hum,totalrain,Regendata.rainhour);
	sprintf(dtemp,"%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X=%02X=%02X,%02X,%02X,%02X,%02X---%d,%d,%d,%d,%d,%d-%d\r\n",weatherdata[0],weatherdata[1],weatherdata[2],weatherdata[3],weatherdata[4],weatherdata[5],weatherdata[6],weatherdata[7],weatherdata[8],weatherdata[9],weatherdata[10],weatherdata[11],weatherdata[12],weatherdata[13],weatherdata[14],weatherdata[15],weatherdata[16],weatherdata[17],weatherdata[18],weatherdata[19],weatherdata[20],weatherdata[21],direction,temp,waverage,gust,hum,totalrain,Regendata.rainhour);
	usart_pstr(dtemp);
	_delay_ms(255);
}
u8 CheckData (void)
{
	u8 i;
	u8 checksum=0;
	if ( weatherdata[01]!=0xAA || weatherdata[02]!=0x00 ) return 0;
	for (i=1;i<20;i++) checksum+=weatherdata[i];
	if ((checksum-weatherdata[20])!=0) {weatherdata[0]=checksum-weatherdata[20];return 0;}
	temp=((weatherdata[6]<<8)+weatherdata[7])-500;
	if (temp>400 ) return 0;
	hum=weatherdata[8];
	if (hum>100) return 0;
	waverage=weatherdata[10]; //+9 als msb?
	waverage*=10;
	waverage/=85;
	gust=weatherdata[11];
	gust*=73;
	gust/=100;
	
	direction=weatherdata[13];
	if (weatherdata[12] & 2) direction+=256;
	//	if (direction>360) return 0;
	Regendata.actualreading=weatherdata[14]; //per stap 0.2 mm
	Regendata.actualreading<<=8;
	Regendata.actualreading+=weatherdata[15];
	totalrain=Regendata.actualreading;
	//if ((Regendata.actualreading-Regendata.lastreading)<100) {totalrain=Regendata.actualreading;}
	//Regendata.lastreading=Regendata.actualreading;
	if (Regendata.aligncounters==0)
	{
		Regendata.lastcounter=Regendata.actualreading;    //one time voor startup
		Regendata.rainhour=0;
		Regendata.lastreading=Regendata.actualreading;
		Regendata.aligncounters=1;
	}
	//first 6 bytes ident? securitycode? 14,AA,00,24,0D,1E
	
	//uv=weatherdata[16] index 0-15?
	/*
	light=weatherdata[17];
	light<<=8;
	light+=weatherdata[18];
	light<<=8;
	light+=weatherdata[19];
	*/
	// 21 = Checksum?
	//checksum=0;
	//for (u8 i=0;i<21;i++) {checksum+=weatherdata[i];}
	//totalrain=checksum;
	//last words 22 -32 : C1,B2,65,A9,74,39,C7,38,C9,1E
	
	return 1;
}
void ServiceModule(void)
{
	
	SET(BARO);
	bmp085_init();
	hpa= bmp085_getpressure();
	SET_OUTPUT(SDA);
	SET_OUTPUT(SCL);
	RESET(SDA);
	RESET(SCL);
	RESET(BARO);

}
ISR(USART_TX_vect)

{

	if (txrdpointer==txwrpointer) {firstchar=0;return;}
	UDR0 = txuartbuffer[txrdpointer++];
}
void USART_Init(unsigned int ubrr)
{
	/*
	Set baud rate
	*/
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char	)ubrr;
	/*Enable receiver and transmitter
	*/
	UCSR0B = (1<<TXEN0) | (1<<TXCIE0);
	/*
	Set frame format: 8data, 2stop bit
	*/
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	
}

void usart_putchar(char data) {
	// Wait for empty transmit buffer
	while ( !(UCSR0A & (1<<UDRE0)) );
	// Start transmission
	UDR0 = data;
	firstchar=1;
}
void usart_pstr(char const *s)
{
	

	// loop through entire string
	while (*s) {
		
		if (firstchar==0) usart_putchar(*s); else txuartbuffer[txwrpointer++]=*s;
		s++;
	}

}

u8 ReadAD(void)
{
	u8 PRR_reg=PRR;
	PRR&=~(1<<PRADC); //power down devices
	int res;
	ADMUX = (0x01 << REFS0)    /* AVCC with external capacitor at AREF pin */
	| (0 << ADLAR)        /* Left Adjust Result: disabled */
	| (0x0e << MUX0)    /* Internal Reference (VBG) */;
	ADCSRA = (1<<ADEN); // turn ADC on and initialise. no auto-trigger, no interrupt. prescaler to div2

	_delay_ms(1);
	//start a conversion
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC)) {_delay_us(10);};
	res=ADC;
	res>>=2;
	ADCSRA=0;
	PRR=PRR_reg; //power down devices
	return (res);
}



