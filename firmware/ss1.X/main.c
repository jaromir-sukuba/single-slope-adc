#include <xc.h>
#include <stdio.h>
#pragma config FEXTOSC = ECH    // External Oscillator Selection (EC (external clock) above 8 MHz)
#pragma config RSTOSC = EXTOSC  // Reset Oscillator Selection (EXTOSC operating per FEXTOSC bits (device manufacturing default))
#pragma config CLKOUTEN = OFF   // Clock out Enable bit (CLKOUT function is disabled)
#pragma config PR1WAY = ON      // PRLOCKED One-Way Set Enable bit (PRLOCKED bit can be cleared and set only once)
#pragma config MCLRE = EXTMCLR  // MCLR Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RE3 pin function is MCLR )
#pragma config PWRTS = PWRT_OFF // Power-up timer selection bits (PWRT is disabled)
#pragma config MVECEN = ON      // Multi-vector enable bit (Multi-vector enabled, Vector table used for interrupts)
#pragma config IVT1WAY = ON     // IVTLOCK bit One-way set enable bit (IVTLOCKED bit can be cleared and set only once)
#pragma config LPBOREN = OFF    // Low Power BOR Enable bit (Low-Power BOR disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled , SBOREN bit is ignored)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (PPSLOCKED bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled; SWDTEN is ignored)

#define	 _XTAL_FREQ	64000000
#define	LED		LATCbits.LATC2
#define	SW_INT	LATCbits.LATC3
#define	SW_VIN	LATCbits.LATC0
#define	SW_REF	LATAbits.LATA2
#define	SW_ZER	LATCbits.LATC4
#define	COMP	PORTCbits.RC1	//see AN-260 A 20-Bit (1 ppm) Linear Slope-Integrating A/D Converter, snoa597b.pdf
#define	U1_BAUD	115200
#define	U1_BRG	(((_XTAL_FREQ/16)/U1_BAUD)-1)

uint32_t val1,val2,val0;
char txbuff[50];

void txs (char * data)		//transmit zero terminated string via UART1
	{
	while (*data!=0)
		{
		U1TXB=*data++;
		while (U1FIFObits.TXBF);
		}
	}

uint32_t do_adc (uint8_t inp)	//perform ADC conversion
	{
	SW_ZER = 0;
	SW_REF = 0;
	SW_VIN = 0;
	if (inp==0) SW_ZER = 1;
	if (inp==1) SW_REF = 1;
	if (inp==2) SW_VIN = 1;		//select appropriat einput
	SMT1STATbits.RST=1;
	SMT1CON1bits.SMT1GO = 1;	//reset and triger SMT
	SW_INT = 1;
	__delay_us(1500);			//discharge integrator
	SW_INT = 0;
	__delay_ms(8);				//wait while integrator charges up, gate SMT with comparator output
	return SMT1TMR;
	}

void main(void)
	{
	LATA = 0;
	LATC = 0;
	TRISC = 0x02;
	TRISA = 0x00;
	ANSELA = 0;
	ANSELC = 0;				//setup GPIO
	SMT1CON0 = 0x80;
	SMT1CON1 = 0x81;
	SMT1CLK = 0x01;			//setup SMT for gated timer
	U1CON0 = 0x20;
	U1CON1 = 0x80;
	U1BRG = U1_BRG;			//8N1 115200
	PPSLOCK = 0x55;
	PPSLOCK = 0xAA;
	PPSLOCKbits.PPSLOCKED = 0;
	SMT1SIGPPS = 0b010001;	//SMT input to gate the SMT timer
	RA4PPS = 0x10;			//UART1TX pin

	while (1)
		{
		LED = ~ LED;		//what is the point of an embedded project without a blinking LED?
		val0 = do_adc(0);	//measure zero input
		val1 = do_adc(1);	//measure reference
		val2 = do_adc(2);	//measure unknown signal
		sprintf(txbuff,"%6ld,%6ld,%6ld\n",val0,val1,val2);
		txs(txbuff);		//yeet in computer general direction, result = Vref * (val2-val0)/(val1-val0)
		}
	}
