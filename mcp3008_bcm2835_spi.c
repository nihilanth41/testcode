#include <stdio.h>
#include <bcm2835.h>

/* prototype(s) */
int readadc(int adc_channel);


int main(int argc, char **argv){
//bcm2835_set_debug(1);
if (!bcm2835_init()){ return 1; }
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST); //default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0); //default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_2048); //122khz ~(250mhz/2048)
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
    
    //Read mcp3008 channels 0-7 and report their values w/ respect to AREF
    static int x; 
    for(x=0; x<8; x++){
    printf("Read %2.1f volts on mcp3008 channel %d\n", (adc_to_vref(readadc(x))), x); }
	return 0; }
    
    
    
    
double adc_to_vref(int adcout){
double adc_bits = 1024;
double aref = 2.5;
double mvpd = (aref/adc_bits);
double adc_vout = (adcout * mvpd);
return adc_vout; }

int readadc(int adc_channel){
if(adc_channel>7||adc_channel<0){  printf("ERROR: Invalid Channel. Valid Channels are %d through %d\n", 0, 7); return -1; }
uint8_t buf[] = { 1, ((8+adc_channel)<<4), 0 };
bcm2835_spi_transfern(buf, sizeof(buf));
int adcout = (((buf[1]&3) << 8) + buf[2]);
return adcout; }
