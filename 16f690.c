/* Template for PIC 16f690 code using Hi-Tech C */

#include <stdio.h>
#include <htc.h>
#include <pic.h>


/* example pin definitions */
#define MULTIPLEXER_EN RC4
#define MULTIPLEXER_IN RA2
#define STATUS_LED RC6
#define SYNC_SIGNAL RC7
#define RELAY_PIN RB4

/* useful bitwise macros */

#define bit_set(var,bitno) ((var) |= 1 << (bitno))
#define bit_clr(var,bitno) ((var) &= ~(1 << (bitno)))

#ifndef _XTAL_FREQ
// Unless specified elsewhere, 20MHz system frequency is assumed
//#define _XTAL_FREQ 20000000
#define _XTAL_FREQ 4000000 //4mhz
#endif


/* set mcu config in code */
//for config options see: <pic16f690.h>
__CONFIG(
  FOSC_HS   	// HS Oscillator
& WDTE_OFF 		// Watchdog Timer Disabled
& PWRTE_OFF 	// Power-up Timer Disabled
& MCLRE_ON		// MCLR pin function is MCLR
& BOREN_OFF 	// Brown-Out Reset Disabled
& CPD_OFF 		// Data EEPROM code protection off
& CP_OFF		// Code protection off
		 );

volatile unsigned char pins_setup = 0; 
volatile unsigned char new_data_rx = 0;
volatile unsigned char relay_signal = 0;
volatile unsigned char tx_byte = 0;
volatile unsigned char rx_byte = 0;
volatile unsigned char portc_temp = 0;   	 //temp variable for bit manipulation
volatile unsigned char counter = 0;			 // loop increment counter
volatile unsigned char counter_temp = 0;     // variable for bit-manipulation
volatile unsigned char floor_num = 0;		// verified floor number

//To leave a bit unchanged, OR is used with a 0.  OR to set 
//To leave a bit unchanged, AND is used with a 1. AND to clear
// 0xF0 = 0b11110000  //used with an & to clear low bits and ignore high bits
// 0x0F = 0b00001111	// used with an & to clear high bits and ignore low bits
const unsigned char portc_mask = 0xF0;	// 0xF0 = 0b11110000
const unsigned char counter_mask = 0x0F; // 0x0F = 0b00001111

/* prototypes */
void pin_setup(void);
void usart_init(void);
void txd(unsigned char tx);
void portc_bitset(unsigned char c);

void interrupt receive(void)
{
  if(RCIE && RCIF) // Received Interrupt Flag and Recieve buffer full
	{
		RCIF = 0; // clear RCIF flag (to avoid loop)
		rx_byte = RCREG; // set rx_byte to recieve register
		new_data_rx = 1;
	}
}


int main(void) {
  if(!pins_setup)
	{
		pin_setup();  // Set TRIS registers and initial values 
		usart_init(); // setup usart, interrupt registers
		ei(); 		  // enable global interrupts
		pins_setup=1;
	}

return 1;}


void pin_setup(void) {
  //port directions: 1=input, 0=output
	TRISC=0b10100000; // RC7/6, RC5/4, RC3/0
  //Serial, inputs, binary code outputs
  PORTC=0b01000000; // zero out all ports (to be safe) //except LED
	TRISB=0b11100000; // RB7 tx, output, rb5 rx, input
	TRISA=0b00100100; //RA2 T0 INPUT
	RB4 = 0;
	ANSEL = 0x00; //digital i/o
	ANSELH = 0x00;
	T0CS=1; //set TIMER0 in counter mode
	T0SE=0; //increment on rising edge low > high
	T0IF=0; // clear flag
	TMR0=0x00;
	T0IE=1; // enable tmr0 interrupts
  return; }
  
void usart_init(void) {
//port directions: 1=input, 0=output
SPBRG=25;  //baud rate 9600 @ 4mhz, async, BRGH=1
				//SPBRG = 129; // 9600 brgh = 1; 20 Mhz
				//SPBRG = 64;  // 19200 brgh = 1; 20 Mhz
	TXSTA=0b00100110;	//Asynchronous, 8-Bit, High Speed Baud.
	GIE=1; 		// general interrupt enable
	PEIE=1; 	// peripheral interrupt enable
	RCIE=1; 			// RECIEVE interrupt enable
	RCSTA=0b10010000;	// Serial Enabled (tx/rx as c6/c7), 8-Bit Reception, Continuous recieve enabled, no address detection
  return; }
  
void txd(unsigned char tx)
{
  TXREG = tx;
    while(!TRMT);
} 

void portc_bitset(unsigned char c)
{
  	//To leave a bit unchanged, OR is used with a 0.  OR to set 
		//To leave a bit unchanged, AND is used with a 1. AND to clear
		
	portc_temp = PORTC;				// Read current binary value on PORTC 
	portc_temp &= portc_mask;		//AND the binary string on PORTC with '0b11110000'(0xF0) to clear any low bits
			
	counter_temp = c;				// Set the current loop count to a temp variable
	counter_temp &= counter_mask;	// AND with (0x0F, 0b00001111) to clear any high bits (just in case)
			
	portc_temp |= counter_temp;		// portc_temp gets portc_temp OR'd with counter_temp 
	PORTC = portc_temp;				//(Should ignore any high bits and set the low bits with the current count number)
}
