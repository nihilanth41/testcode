// Specify device at compile time
#include <stdio.h>
#include <htc.h>  	//HI-TECH C // Automatically imports device header

#define OUTPUT 0
#define INPUT 1

#ifndef _XTAL_FREQ
// Unless specified elsewhere, 20MHz system frequency is assumed
//#define _XTAL_FREQ 20000000  //20mhz
#define _XTAL_FREQ 4000000   //4mhz
#endif


//definitions for LED pins
#define LED_B RA0       // 	1 basement
#define LED_L RA1 	    //  2 lobby
#define LED_2 RC0		//  3
#define LED_3 RC1		//  4
#define	LED_4 RC2		//  5
#define LED_5 RC3		//  6
#define LED_6 RC4		//  7
#define LED_7 RC5		//  8
#define LED_8 RC6		//  9
#define LED_9 RC7		//  10
#define LED_U RB4		//	11 up
#define LED_D RB6 		//	12 down

#define LOBBY_BTN RA2 //LOBBY BUTTON

//bit manipulation routines
#define testbit_on(data,bitno) ((data>>bitno)&0x01)
#define bit_set(var,bitno) ((var) |= 1 << (bitno))
#define bit_clr(var,bitno) ((var) &= ~(1 << (bitno)))

//for config options see: <pic16f690.h>
__CONFIG(
  FOSC_HS 		// HS Oscillator
  //FOSC_INTRCIO	//  Internal OSC / i/o on ra4/ra5 (osc1/clkin)/(os2/clkout) default 4mhz
& WDTE_OFF 		// Watchdog Timer Disabled
& PWRTE_OFF 	// Power-up Timer Disabled
& MCLRE_OFF		// MCLR pin function is not MCLR (button input)
& BOREN_OFF 	// Brown-Out Reset Disabled
& CPD_OFF 		// Data EEPROM code protection off
& CP_OFF		// Code protection off
		 );
		 


//SET TRISTATE MANUALLY(0 = Output, 1 = Input)

volatile unsigned char rx_byte = 0x00;
volatile unsigned char tx_byte = 0x00;		

//variables for floor and direction for each elevator
volatile unsigned char floor_num = 0; 
volatile bit btn_state = 0;
volatile bit btn_state_prev = 0;
volatile unsigned int btn_count; //holds the button pin's state
volatile bit btn_wasPressed = 0;
volatile bit floor_state = 0; //light on or off
volatile bit pins_setup = 0; //for first-start
volatile bit new_data_rx = 0; // new data received flag

//To leave a bit unchanged, OR is used with a 0.  OR to set 
//To leave a bit unchanged, AND is used with a 1. AND to clear
// 0xF0 = 0b11110000	//used with an & to clear low bits and ignore high bits
// 0x0F = 0b00001111	// used with an & to clear high bits and ignore low bits
const unsigned char portc_mask = 0xF0;	// 0xF0 = 0b11110000
const unsigned char counter_mask = 0x0F; // 0x0F = 0b00001111
const unsigned char lobby_n = 2;

//function prototypes
void usart_init(void);

void pin_setup(void)
{
	//port directions: 1=input, 0=output
	TRISC = 0b00000000; //LED outputs 
	TRISB = 0b10100000;// RB7 AND RB5 (TX/RX) and RB6 AND RB4 ARE LED DRIVING PINS
	TRISA = 0b00111100; // RA5/RA4 (CLKIN/CLKOUT), RA3 MCLR, RA2-RA1 LED OUTPUTS, RA2 LOBBY_BTN input
	
	ANSEL=0x00;
	ANSELH=0x00; //set all analog channels as digital input
	
	PORTC=0xFF; // all LED outputs high (OFF)
	RB6 = 1;
	RB4 = 1;
	RA1 = 1;
	RA0 = 1;
	
	INTEDG = 0; //interrupt on falling edge of RA2
	INTF = 0; // clear ra2 interrupt flag
	INTE = 0; //disable interrupt on RA2 (don't use ext. interrupt for pushbutton, too sensitive)
	
}	

void txd(unsigned char tx)
{
	TXREG = tx;
    while(!TRMT);
} 


void interrupt receive(void)
{
	if(RCIE && RCIF) // Received Interrupt Flag and Recieve buffer full
	{
		RCIF = 0; // clear RCIF flag (to avoid loop)
		//assign recieve register to temp variable
		rx_byte = RCREG; //RCREG is R/O and automatically clears
		floor_num = rx_byte; //store rx_byte as floor number
		new_data_rx = 1;
	}
} //end isr


void switch_debounce(void) 
{
btn_count = 0; //reset counter
for(int x=0; x<500; x++) //sample 200 times
{
  btn_state = LOBBY_BTN; //record current state of the lobby button pin 
  btn_state = !(btn_state); //digital low is 1 for the button sample
  btn_count += btn_state; //add the previous state to the overall count
}
  if(btn_count > 250) //button is low for 100 samples
	{ btn_wasPressed = 1; }
	 else
	  { btn_wasPressed = 0; }
} //end switch_debounce();
  
void main()
{
if(pins_setup == 0)
	{
		pin_setup(); // setup TRIS register and initial output pin values
		usart_init(); // setup usart, interrupt registers
		pins_setup = 1; //set pins_setup flag
		ei(); 		  // enable global interrupts AFTER setting interrupt registers
	}
	while(pins_setup == 1)
	{
		switch_debounce(); //check the state of LOBBY_BTN
			if(btn_wasPressed == 1) 
			{ txd(lobby_n); } //if the button is pressed transmit a '2' to signal the relay
		
		if(new_data_rx == 1) //check for serial data
		{
			if(testbit_on(floor_num,7)){ floor_state = 0; } //if MSB set, turn on LED
			else { floor_state = 1; } //if MSB not set, turn off LED
			bit_clr(floor_num,7); //clear MSB off floor number
			switch(floor_num)
			{	
				case 12: {LED_D = floor_state; break;}
				case 11: {LED_U = floor_state; break;}
				case 10: {LED_9 = floor_state; break;}
				case  9: {LED_8 = floor_state; break;}
				case  8: {LED_7 = floor_state; break;}
				case  7: {LED_6 = floor_state; break;}
				case  6: {LED_5 = floor_state; break;}
				case  5: {LED_4 = floor_state; break;}
				case  4: {LED_3 = floor_state; break;}
				case  3: {LED_2 = floor_state; break;}
				case  2: {LED_L = floor_state; break;}
				case  1: {LED_B = floor_state; break;}
			}
			new_data_rx = 0; //clear flag
		}
	}	// endless loop
} //end main
		
		
	
	
	void usart_init(void)
{
/* USART OPERATION DESCRIPTION:
When a byte is received by UART module it is stored in RXREG register, 
a RXIF flag rises which gives interrupt and interrupt subroutine “void interrupt receive(void)” is automatically called up on this interrupt. 
To test this program whether it is able to communicate via serial we will transmit the received byte. 
The transmission begins when the TXREG register is loaded. 
TRMT bit is the status bit of transmission. 
if it is ’0' the transmission is in process and if it is ’1' then it indicates that transmission is completed and it is ready to transmit new byte.
*/

				//port directions: 1=input, 0=output
    SPBRG=25;  //baud rate 9600 @ 4mhz
	
				//SPBRG = 129; // 9600 brgh = 1; 20 Mhz
				//SPBRG = 64;  // 19200 brgh = 1; 20 Mhz
				//TXSTA=0b00110100;    //CHECK THE DATA SHEET FOR TXSTA
				//RCSTA=0b10010000;    //SEE THE DATA SHEET FOR RCSTA
	TXSTA=0b00100110;	//Asynchronous, 8-Bit, High Speed Baud.
	GIE=1; 		// general interrupt enable
	PEIE=1; 	// peripheral interrupt enable
	RCIE=1; 			// RECIEVE interrupt enable
	RCSTA=0b10010000;	// Serial Enabled (tx/rx as c6/c7), 8-Bit Reception, Continuous recieve enable, no address detection
}
	

	

	/*
Asynchronous Only:
SPBRG = (Fosc / (16 x Baud rate)) - 1, BRGH = 1 High Speed
SPBRG = (Fosc / (64 x Baud rate)) - 1, BRGH = 0 Low Speed 

TXSTA: TRANSMIT STATUS AND CONTROL REGISTER (ADDR: 0x98)
Bit 7: CSRC: Clock Source Select bit		// Only Matters for Synchronous Mode. Determines if device is: Transmitter (1) or Reciever (0)

Bit 6: TX9 Transmit Enable bit 			// Select 9-Bit Transmission (1) or Select 8-Bit Transmission (0)

Bit 5: TXEN: Transmit Enable bit			// Transmit Enabled (1) or Transmit Disabled (0)

Bit 4: SYNC: USART Mode Select bit  		// Synchronous Mode (1) or Asynchronous Mode (0)

Bit 3: U-0: ---

Bit 2: BRGH - High Baud Rate Select bit		// Only used for Asynchronous Mode .  High Speed (1) or Low Speed (0)

Bit 1: TRMT - Transmit Shift Register Status bit // TSR Empty (1) or TSR Full (0)

Bit 0: TX9D -    Place for a 9th bit		// In Case of 9-Bit Transmission
//
//
RCSTA : RECEIVE STATUS AND CONTROL REGISTER (ADDR: 0x18)
Bit 7: SPEN: Serial Port Enable bit		// Serial Port Enabled on RC6/RC7 (TX/RX) (1) or Serial Port Disabled (0) 

Bit 6: RX9: 9th -bit Receive Enable bit 		// enable reception of 9-bit (1) or enable reception of 8-bit (0)

Bit 5: SREN - Single Receive Enable bit		// Enable single recieve (1) or Disable single recieve (0) Enables/Cancels Packet Transmission Only for synchronous mode w/ PIC as Master.  This bit is cleared after reception is complete. 

Bit 4: CREN - Continuous Receive Enable bit 	// Asynchronous mode: 1 = Enables continuous receive or 0 = Disables continuous receive 
							// Synchronous mode: 1 = Enables continuous receive until enable bit CREN is cleared (CREN overrides SREN)  or 0 = Disables continuous receive 

Bit 3: ADDEN - Address Detect Enable bit	// Asynchronous mode 9-bit (RX9 = 1): 
							//  1 = Enables address detection, enables interrupt and load of the receive buffer when RSR[8] is set  
							//  0 = Disables address detection, all bytes are received and ninth bit can be used as parity bit

Bit 2: FERR – Framing Error bit 			// 1 Logic level “1” – means the STOP bit was not received. In serial communication we--
							// --use START bit and STOP bit when transmitting the information. 
							//1 = Framing error (can be updated by reading RCREG register and receive next valid byte) 
							// 0 = No framing error Bit 1: OERR - Overrun Error bit			
Bit 1: OERR - Overrun Error bit							
							// Logical level “1” means that new byte of data was received, while there is still--
							//--previous data that did not proceed into the PIC. In this case, the new received information is lost.  
							//1 = Overrun error (can be cleared by clearing bit CREN) 
							//0 = No overrun error  

Bit 0: RX9D: 9th bit of Received Data		// RX9D: 9th bit of Received Data, in the case of receiving 9-bits.   

*/
		
