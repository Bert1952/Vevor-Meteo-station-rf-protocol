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
u8 weatherdata[wlen];

u8 txrdpointer=0;
u8 txwrpointer=0;//
extern u8 txbuffer[];
char txuartbuffer[256];
char firstchar=0;
float temp=20;  //wordt 2 byte, temp is maal 10
u8	hum=11;  //als hex getal is in %
u16 waverage=10;  //km/uur
u16 gust=16; //km/uur
u16 direction=181;; //wordt 360
static u16 totalrain=2001;
u16 light=0;
u16 hpa=1024;
u8	dew=55;

static u8 MCRREG=0;

u8 tx_power=0;
//char dtemp[200];
//char buffer[30];
static u8 start=0;
static u8 wp=0;
static u16 wbyte=0;
static u8 wcount=0;
u16 pt=0;
u16 spl=0;
u8 bittime=90;
//u16 leadingpuls=0;
static RAIN   Regendata;


void PrepareMessage(void)
{

	txbuffer[packetpos]=meteo_packet; //energy code
	txbuffer[test_pos]=MCRREG;
	txbuffer[average_pos]=(u8) waverage;
	txbuffer[directon_pos]=direction;
	txbuffer[gust_pos]=(u8) gust;
	int tp=temp*10;
	txbuffer[temp_pos]= tp>>8;
	txbuffer[temp_pos+1]=tp & 0xff;
	txbuffer[hum_pos]=hum;
	txbuffer[rain_pos]=Regendata.rainhour;
	txbuffer[totalrain_pos]=totalrain>>8;
	txbuffer[totalrain_pos+1]=totalrain & 0xff;
	txbuffer[pressure_pos]=hpa>>8;
	txbuffer[pressure_pos+1]=hpa & 0xff;
	txbuffer[batt_voltage]=ReadAD();
	dew=(int) tlkRH_calculate_dewpoint(hum,temp);
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
/*
u8 SPI_transceive(char dataout)
{
	u8 SPIout=0;
	

	char i=0;
	for (i=0;i<8;i++) {
		SPIout<<=1;
		if ((dataout & 0x80)> 0) SET(MOSI); else RESET(MOSI);
		SET(SCLK);
		//_delay_us(dtm)
		RESET(SCLK);
		//  _delay_us(dtm)
		if IS_SET(MISO) SPIout|=1;
		
		dataout<<=1;
	}

	return SPIout;
}
*/
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
	Regendata.seccounter=0;
	Regendata.rainhour=0;
	Regendata.yesterdayrain=0;
	Regendata.lastcounter=0;
	Regendata.aligncounters=0;
	#if uart
	#else
	
	SET_OUTPUT(RX);
	
	//tx_power=1;
	#endif
}
/*
void EnableInterrupt(char nr,char sense)
{
	if (nr==0)
	{
		EIMSK|=1<<INT0;
		EICRA|=1<<ISC01;
		if (sense==1) EICRA|=1<<ISC00;
	}
	else
	{
		EIMSK|=1<<INT1;
		EICRA|=1<<ISC11;
		if (sense==1) EICRA|=1<<ISC10;
	}
}
*/
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
	static u8 c=0;

	c++;
	if (c==15) {
		c=0;
/*
		Regendata.verschil=(totalrain-Regendata.lastcounter)&0xffff;
		Regendata.lastcounter=totalrain;
		if (Regendata.verschil>0) {
			Regendata.rainhour=(2520/Regendata.seccounter)*Regendata.verschil; //aantal tiks per 2520 eenheid is 1 mm uur
			Regendata.seccounter=1;
		}
		Regendata.seccounter++;
		if (Regendata.seccounter>2520) {Regendata.seccounter=2520;    //komt overeen met 1mm/uur reset na 40 min
			Regendata.rainhour=0;
		}
		*/
	}
	
}
ISR(TIMER0_OVF_vect)
{
	if (IS_SET(HUMB)) {
		TCNT0=0xff-85;
		wbyte<<=1;
		if (IS_SET(Int0)) {wbyte|=1;}
		wcount++;
		TOGGLE(HUMA);
		if (wcount==8) {wcount=0;weatherdata[wp++]=wbyte;wbyte=0;}
		if (wp>30){SET(Led);RESET(HUMB);start=0;}
		if (wp==13) {SET(SCL);}
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
		if (preamble>msampels  && pretime>2600 && pretime<2800) {bittime=pretime/msampels;start=2;spl=0;pt=pretime;wp=0;RESET(SCL);RESET(HUMA);wbyte=0;wcount=2;return;}
		break;
		case 2:case 3:case 8: case 9:case 13:if (Pulse>130 && Pulse<250) {start++;} else {}break;
		case 4:case 5:case 6: case 7:case 10:case 11: case 12: if (Pulse<130) {start++;} else {start=0;} break;
		case 14: SET(HUMB);
		TCNT0=0xff-35;
		TIFR0 = 1<<TOV0;
		start=15;break;
		case 15: if (!IS_SET(Int0)) TCNT0=0xff-35;
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
		if (IS_SET(Led)) {
			if (weatherdata[0]==0x14) {
				temp=((weatherdata[6]<<8)+weatherdata[7])-500;
				//	buitentemp<<=8;
				//	buitentemp+=weatherdata[7];
				//	buitentemp-=500;
				hum=weatherdata[8];  //nog te controleren
				waverage=weatherdata[10]; //+9 als msb?
				gust=weatherdata[11];
				gust*=73;
				gust/=100;
				
				direction=weatherdata[13];
				if (weatherdata[12] & 2) direction+=256;
				totalrain=weatherdata[14]; //per stap 0.5 mm
				totalrain<<=8;
				totalrain+=weatherdata[15];
				//wat met 16 en 17?
			//	light=(weatherdata[18]<<8)+weatherdata[19];  //light of uv + pos 19 +20
				if (Regendata.aligncounters==0)  {
					Regendata.lastcounter=totalrain;    //one time voor startup
					Regendata.rainhour=0;
					Regendata.aligncounters=1;
				}
				Pout();
				RF22B_INIT();
				PrepareMessage();
				SendDataRfm22b();
				RFM22_OOK_INIT ();
			}
			RESET(Led);
		}
		if (start==2){start=0;}
		_delay_ms(55);
		
	}

}

void Pout (void)
{
	char dtemp[200];
	/*
	strcat(dtemp,"");
	for (u8 i=4;i<30;i++) {sprintf(buffer,"%02X,",weatherdata[i]);strcat(dtemp,buffer);}
	sprintf(buffer,"-%d",direction);strcat(dtemp,buffer);
	sprintf(buffer,"-%d",(int) temp);strcat(dtemp,buffer);
	sprintf(buffer,"-%d",waverage);strcat(dtemp,buffer);
	sprintf(buffer,"-%d",gust);strcat(dtemp,buffer);
	sprintf(buffer,"-%d",hum);strcat(dtemp,buffer);
	sprintf(buffer,"-%d", totalrain);strcat(dtemp,buffer);
	sprintf(buffer,"-%d", light);strcat(dtemp,buffer);
	sprintf(buffer,"\r\n"),strcat(dtemp,buffer);
	*/
		sprintf(dtemp,"%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X---%d,%d,%d,%d,%d,%d\r\n",weatherdata[0],weatherdata[1],weatherdata[2],weatherdata[3],weatherdata[4],weatherdata[5],weatherdata[6],weatherdata[7],weatherdata[8],weatherdata[9],weatherdata[10],weatherdata[11],weatherdata[12],weatherdata[13],weatherdata[14],weatherdata[15],weatherdata[16],weatherdata[17],weatherdata[18],weatherdata[19],weatherdata[20],weatherdata[21],weatherdata[22],weatherdata[23],weatherdata[24],weatherdata[25],weatherdata[26],weatherdata[27],weatherdata[28],direction,(int) temp,waverage,gust,hum,totalrain);
	usart_pstr(dtemp);
	_delay_ms(255);
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

