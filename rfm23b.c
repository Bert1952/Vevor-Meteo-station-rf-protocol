#include "main.h"
#include "rfm23b.h"
#include <util/delay.h>
#include <avr\io.h>
#include "global.h"
#include "defaults.h"

char ItStatus1;
char ItStatus2;
u8 txbuffer[20];
extern u8 tx_power;

//////////////////////////////////////////////////////////
void TX_ON (void)
{
	SET(MOSI);
//RFXX_WRT_CMD(0x8700 | RF22B_PWRSTATE_READY );

//RFXX_WRT_CMD(0x8700 | RF22B_PWRSTATE_TX ) ;//tx on
}
void TX_OFF(void)
{
	RESET(MOSI);
//RFXX_WRT_CMD(0x8700);	
}
void RFM22_OOK_INIT (void)

{
	
	RFXX_WRT_CMD(0x8700 |RF22B_swres );
	_delay_ms(1);
	u8 i=255;
	while ((spi_read(0x04)!= 3) & (i>0))
	{
		_delay_us(100);
		i--;
	}
	RFXX_WRT_CMD(0x8705);
	RFXX_WRT_CMD(0x8b15);// reg 0b =gpio0 rxdata
	RFXX_WRT_CMD(0x8c14);// reg 0b =gpio1 rxdata
	RFXX_WRT_CMD(0x8d14);// reg 0b =gpio2 rxdata	
	RFXX_WRT_CMD(0x9CAC); //reg 1c bandwidth
	RFXX_WRT_CMD(0x9F03);// reg 1f
	RFXX_WRT_CMD(0xA096);// reg 20
	RFXX_WRT_CMD(0xA100);// reg 21
	RFXX_WRT_CMD(0xA2DA);// reg 22
	RFXX_WRT_CMD(0xA374);// reg 23
	RFXX_WRT_CMD(0xA400);// reg 24
	RFXX_WRT_CMD(0xA5B1);// reg 25
	RFXX_WRT_CMD(0xA701);// reg 27 //rssi threshold
	RFXX_WRT_CMD(0xAC28);// reg 2c ook register manchester enable bit
	RFXX_WRT_CMD(0xAD7D);// reg 2d
	RFXX_WRT_CMD(0xAE29);// reg 2E
	RFXX_WRT_CMD(0xb061);//reg 30 disable packethandling
	RFXX_WRT_CMD(0xb30A);//reg 33 headercontrol
	RFXX_WRT_CMD(0xb402);//reg 34 preamble length
	RFXX_WRT_CMD(0xb52A);//reg 35 preamble detection control1
	RFXX_WRT_CMD(0xb6CA);//reg 36 sync word1
	RFXX_WRT_CMD(0xb7CA);//reg 37 sync word0 
	RFXX_WRT_CMD(0xE960);// reg 69
	RFXX_WRT_CMD(0xeD07); //tx power
	RFXX_WRT_CMD(0xf024);  //modulation mode control 1
	RFXX_WRT_CMD(0xf122);  //modulation mode control 2
//	RFXX_WRT_CMD(0xf250);  //freq deviation
	RFXX_WRT_CMD(0xf573);
	RFXX_WRT_CMD(0xf668); //869.2 mhz sub band 4
	RFXX_WRT_CMD(0xf760);

//	RFXX_WRT_CMD(0x8500| irxffafull);
//	RFXX_WRT_CMD(0x8600| irssi|ipreaval |iswdet); //interrupt rssi en preamble
//	RFXX_WRT_CMD(0xfE05); //rx fifo almost full
//	readInterruptStatus();
	
//	RFXX_WRT_CMD(0x8e01);// reg 0b =gpio0 dio
}
void RF22B_INIT(void)
{
	
	RFXX_WRT_CMD(0x8700 |RF22B_swres );
	_delay_ms(1);
	u8 i=255;
	while ((spi_read(0x04)!= 3) & (i>0))
	{
	 _delay_us(100);
	 i--;
	}
	RFXX_WRT_CMD(0x8700|RF22B_PWRSTATE_READY );
	RFXX_WRT_CMD(0x8600| RF22B_enwut);
	RFXX_WRT_CMD(0x8a05);

	RFXX_WRT_CMD(0x8E00);
//wakeup counter set 1 sec steps
	RFXX_WRT_CMD(0x9401); //multiplier 
	RFXX_WRT_CMD(0x9570); // 1 sec steps was 7

	RFXX_WRT_CMD(0x9cab);	 //if filter bandwidth
	RFXX_WRT_CMD(0x9d40);	   //afc loop filter

	RFXX_WRT_CMD(0xb0ac);
	RFXX_WRT_CMD(0xb28c);
	RFXX_WRT_CMD(0xb312);
	RFXX_WRT_CMD(0xb408);
	RFXX_WRT_CMD(0xb522);
	RFXX_WRT_CMD(0xb62D);
	RFXX_WRT_CMD(0xb7D4);
	RFXX_WRT_CMD(0xb800);
	RFXX_WRT_CMD(0xb900);
	RFXX_WRT_CMD(0xbA0A);
	RFXX_WRT_CMD(0xbB12);
	RFXX_WRT_CMD(0xbC7A);
	RFXX_WRT_CMD(0xbD3F);
	RFXX_WRT_CMD(0xbE00 | tx_frame); //package is 10 byte long
	////////////
	RFXX_WRT_CMD(0xbF0A); //Relevant for RX settings Only
	RFXX_WRT_CMD(0xc012);
	RFXX_WRT_CMD(0xc17A);
	RFXX_WRT_CMD(0xc23F);
	RFXX_WRT_CMD(0xc3FF);
	RFXX_WRT_CMD(0xc4FF);
	RFXX_WRT_CMD(0xc5FF);
	RFXX_WRT_CMD(0xc6FF);
	///////////////////////////
	if (tx_power) 	RFXX_WRT_CMD(0xeD0F); else RFXX_WRT_CMD(0xeD0B);
	RFXX_WRT_CMD(0xeE51); //DATA RATE 1
	RFXX_WRT_CMD(0xeFEC); //DATA RATE 0

	RFXX_WRT_CMD(0xf123);  //modulation mode control
	RFXX_WRT_CMD(0xf02C); //modulation control
	RFXX_WRT_CMD(0xf250); //freq deviation
	RFXX_WRT_CMD(0xf300);
	RFXX_WRT_CMD(0xf400);

	//carrier frequentie 868 mhz exact
	RFXX_WRT_CMD(0xf573); //band selectie
	/////////////////////
	RFXX_WRT_CMD(0xf900);
	RFXX_WRT_CMD(0xfA00);
	////////////
	 RFXX_WRT_CMD(0xfA00);
}
void LoadTxFreq(void)
{
	RFXX_WRT_CMD(0xf573);
	RFXX_WRT_CMD(0xf673); //869.2 mhz sub band 4
	RFXX_WRT_CMD(0xf700);
}

///////////////////////////////////////////////////////
u8  RFXX_WRT_CMD(u16  aCmd)
{
	u16 temp=0;
	RESET(nSel);
	temp=SPI_MasterTransmit_Command(aCmd>>8);
	temp<<=8;
	temp|=SPI_MasterTransmit_Command(aCmd & 0xff);
	SET(nSel);

	return (u8) temp;
}

//--------------------------------------------------------------
u8 spi_read(u8 address)
{
	u8 result;
	RESET(nSel);
	result=SPI_MasterTransmit_Command(address);
	result=SPI_MasterTransmit_Command(0xff);
	SET(nSel);
	return(result);
}
void SleepRFM(void)
{
	RFXX_WRT_CMD(0x8700 );
}
///////////////////////////////////////
//void to_ready_mode(void)
//{
//	ItStatus1 = spi_read(0x03);
//	ItStatus2 = spi_read(0x04);
//	RFXX_WRT_CMD(0x8700 | RF22B_xton || RF22B_enwt );
//}
/////////////////////////////////////////////////
void rx_reset(void)
{
	RFXX_WRT_CMD(0x8700| RF22B_PWRSTATE_READY);
	//RFXX_WRT_CMD(0xfe00 | RX_FIFOThreshold); // threshold for rx almost full, interrupt when 1 byte received
//	RFXX_WRT_CMD(0x8800 |RF22B_ffclrtx | RF22B_ffclrrx); //clear fifo disable multi packet
//	RFXX_WRT_CMD(0x8800); // clear fifo, disable multi packet
//	RFXX_WRT_CMD(0x8700| RF22B_xton | RF22B_rxon ); // to rx mode
 //RFXX_WRT_CMD(0x8500 | RF22B_enpkvalid);
	ItStatus1 = spi_read(0x03); //read the Interrupt Status1 register
	ItStatus2 = spi_read(0x04);
//	RFXX_WRT_CMD(0x8500);
//	RFXX_WRT_CMD(0x8500 );

	RFXX_WRT_CMD(0x8600 );
}

///////////////////////////////

void SendDataRfm22b(void)
{
	int i;
	LoadTxFreq();
	SET(TXE);
	RFXX_WRT_CMD(0x8803); // disABLE AUTO TX MODE, enble multi packet clear fifo
	RFXX_WRT_CMD(0x8800);//


	for (i=0;i<tx_frame;i++)  RFXX_WRT_CMD(0xff00|txbuffer[i]);
	RFXX_WRT_CMD(0x8500| RF22B_PACKET_SENT_INTERRUPT);
	RFXX_WRT_CMD(0x8700|RF22B_PWRSTATE_TX );
	ServiceModule();
	i=spi_read(0x03);
	u8 timeout=255;

	while (((i & RF22B_PACKET_SENT_INTERRUPT)==0) & (timeout>0))
	{
		_delay_ms(1);
		i=spi_read(0x03);
		timeout--;
	}
	RESET(TXE);
//	SetInterrupts();
//	SleepRFM();


}
/*
void SetInterrupts(void) {
	ItStatus1 = spi_read(0x03); //read the Interrupt Status1 register
	ItStatus2 = spi_read(0x04);
	RFXX_WRT_CMD(0x8500| RF22B_enpksent);
//	RFXX_WRT_CMD(0x8600 | RF22B_enwut);
}

*/