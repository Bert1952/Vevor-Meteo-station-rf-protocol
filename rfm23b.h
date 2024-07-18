void RF22B_INIT(void);
void to_ready_mode(void);
void SendDataRfm22b(void);
void SetInterrupts(void);
unsigned char RFXX_WRT_CMD(unsigned int);
void send_read_address(unsigned char);
unsigned char spi_read(unsigned char address);
void SleepRFM(void);
unsigned char read_8bit_data(void);
void to_rx_mode(void);
unsigned char ReadADC(void);
unsigned char spi_read(unsigned char address);
void Receivedata(void);
void rx_reset(void);
u8 Meettemperatuur(void);
void LoadTxFreq(void);
void LoadRxFreq(void);
char ShiftNode (char);
void RepeatPackage(void);
void RFM22_OOK_INIT (void);
void TX_ON (void);
void TX_OFF(void);
#define RF22B_PWRSTATE_READY 01
#define RF22B_PWRSTATE_TX 0x09
#define RF22B_PACKET_SENT_INTERRUPT 04
#define RF22B_PWRSTATE_POWERDOWN 00
#define RF22B_PWRSTATE_READY 01
#define RF22B_PWRSTATE_TX 0x09
#define RF22B_PWRSTATE_RX 05
//registere 3 interrupt 1 flags
#define icrcerror 1
#define ipkvalid 2
#define ipksent 4
#define iext 8
#define irxffafull 16
#define itxffaem 32
#define itxffafull 64
#define ifferr 128
// register 4 interrupt 2 flags
#define ipor 1
#define ichiprdy 2
#define ilbd 4
#define iwut 8
#define irssi 16
#define ipreainval 32
#define ipreaval 64
#define iswdet 128
// register 5 interrupt 1 
#define RF22B_encrcerror	1
#define RF22B_enpkvalid		2
#define RF22B_enpksent		4
#define RF22B_enext			8
#define RF22B_enrxffafull	16
#define RF22B_entxffaem		32
#define RF22B_entxffafull	64
#define RF22B_enfferr	 	128
//Register 06h. Interrupt Enable 2
#define RF22B_enwut 8
#define RF22B_enpreaval 64
#define RF22B_enswdet	128
//Register 07h. Operating Mode and Function Control 1
#define RF22B_xton	1
#define	RF22B_pllon	2
#define	RF22B_rxon	4
#define	RF22B_txon	8
#define RF22B_enwt	32
#define RF22B_swres	128
//Register 08h. Operating Mode and Function Control 2
#define RF22B_ffclrtx	1
#define RF22B_ffclrrx	2
#define RF22B_autotx	8
#define GPIO_0 	0x90
#define GPIO_1 	0xa0
#define GPIO_2	0xb0
#define ADREF_BG 0
#define ADREF_2	4
#define ADREF_3	8

#define RX_FIFOThreshold	40
//Register 05h. Interrupt Enable 1
