/* This is a short example of adc10 usage on an m430g2553 (using Launchpad)
   It was compiled using TI's msp430g2x31_adc10_temp.c example and a USCI / ADC10 example found on the web
   The WDT is configured to interrupt every ~1 sec and set a flag (int_flag) in the main loop.
   int_flag triggers a conversion on a10 (internal temp sensor), which is
   calculated as degrees F and C and printed over the UART
  */

#include <msp430.h>

#define LED1        BIT6
#define LED0        BIT0

static char buffer[4] = { 0 }; //array to hold itoa conversions
static unsigned int int_flag;
long temp;
long IntDegF;
long IntDegC;

//prototypes:
void USCI_init(void);
void ADC_init(void);
void TX(char *tx_message);
static char *i2a(unsigned i, char *a, unsigned r);
char *itoa(int i, char *a, int r);

int main(void)
{
 //WDT config
  WDTCTL = WDT_ADLY_1000;                    // WDT 1000ms, ACLK, interval timer
  IE1 |= WDTIE;                             // Enable WDT interrupt
  BCSCTL1 = CALBC1_1MHZ;            		// Set DCO to 1MHz
  DCOCTL = CALDCO_1MHZ;
  USCI_init();
  ADC_init();
  int_flag = 0;
  __enable_interrupt();
  while(1) {
	 if(int_flag != 0) { //wdt trigger
		 int_flag = 0; //clear flag

		 ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		 __bis_SR_register(CPUOFF + GIE);        // LPM0 with interrupts enabled
	     // oF = ((A10/1024)*1500mV)-923mV)*1/1.97mV = A10*761/1024 - 468
	     temp = ADC10MEM;
	     IntDegF = ((temp - 630) * 761) / 1024;
	     // oC = ((A10/1024)*1500mV)-986mV)*1/3.55mV = A10*423/1024 - 278
	     temp = ADC10MEM;
	     IntDegC = ((temp - 673) * 423) / 1024;
	     //print result over uart
	     TX("$A10 Deg F:");
	     itoa(IntDegF, buffer, 10);
	     TX(buffer);
	     TX("!\r\n");
	     TX("$A10 Deg C:");
	     itoa(IntDegC, buffer, 10);
	     TX(buffer);
	     TX("!\r\n");
	 }
  }

return 0; }

// Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void) {
  int_flag = 1;
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
  __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}

void ADC_init(void) {
	ADC10CTL1 = INCH_10 + ADC10DIV_3;
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;
}

void USCI_init(void) {
	////////////////USCI setup////////////////
	    P1SEL = BIT1 + BIT2;            // Set P1.1 to RXD and P1.2 to TXD
	    P1SEL2 = BIT1 + BIT2;            //
	    UCA0CTL1 |= UCSSEL_2;            // Have USCI use SMCLK AKA 1MHz main CLK
	    UCA0BR0 = 104;                  // Baud: 9600, N= CLK/Baud, N= 10^6 / 9600
	    UCA0BR1 = 0;                  // Set upper half of baud select to 0
	    UCA0MCTL = UCBRS_1;               // Modulation UCBRSx = 1
	    UCA0CTL1 &= ~UCSWRST;             // Start USCI
}

void TX(char *tx_message)
{
    unsigned int i=0; //Define end of string loop int
    char *message; // message variable
    unsigned int message_num; // define ascii int version variable
    message = tx_message; // move tx_message into message
    while(1)
    {
        if(message[i]==0) // If end of input string is reached, break loop.
        {break;}
        message_num = (int)message[i]; //Cast string char into a int variable
        UCA0TXBUF = message_num; // write INT to TX buffer
        i++; // increase string index
        __delay_cycles(10000); //transmission delay
        if(i>50) //prevent infinite transmit
        {
			#define Error BIT0
            P1OUT |= Error;
            break;
        }
    } // End TX Main While Loop
} // End TX Function

static char *i2a(unsigned i, char *a, unsigned r)
{
    if (i/r > 0) a = i2a(i/r,a,r);
    *a = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[i%r];
    return a+1;
}

char *itoa(int i, char *a, int r)
{
    if ((r < 2) || (r > 36)) r = 10;
    if (i < 0)
    {
        *a = '-';
        *i2a(-(unsigned)i,a+1,r) = 0;
    }
    else *i2a(i,a,r) = 0;
    return a;
}
