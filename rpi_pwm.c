#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <bcm2835.h>

#define PIN RPI_GPIO_P1_11

/* prototype(s) */
int readadc(int adc_channel);
long long int gettimer(void);


#define PWM_FREQ 24000U //24khz


int main(int argc, char **argv){
//bcm2835_set_debug(1);
if (!bcm2835_init()){ return 1; }
  bcm2835_spi_begin();
  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST); //default
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0); //default
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_2048); //122khz ~(250mhz/2048)
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);


static int adc;
static int hi, low, step;
double pwm_period = ((1/(PWM_FREQ))*1000000); //period in microseconds
// Set the pin to be an output
bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);
bcm2835_gpio_write(PIN, HIGH);
while(1){ 
bcm2835_st_delay(0, pwm_period);
bcm2835_gpio_write(PIN, LOW);
bcm2835_st_delay(0, pwm_period);
bcm2835_gpio_write(PIN, HIGH); }
// printf("Read %2.1f volts on mcp3008 channel %d\n", (adc_to_vref(readadc(x))), x); }
return 0; }




double adc_to_vref(int adcout){
double adc_bits = 1024;
double aref = 2.5;
double mvpd = (aref/adc_bits);
double adc_vout = (adcout * mvpd);
return adc_vout; }

int readadc(int adc_channel){
if(adc_channel>7||adc_channel<0){ printf("ERROR: Invalid Channel. Valid Channels are %d through %d\n", 0, 7); return -1; }
uint8_t buf[] = { 1, ((8+adc_channel)<<4), 0 };
bcm2835_spi_transfern(buf, sizeof(buf));
int adcout = (((buf[1]&3) << 8) + buf[2]);
return adcout; }

#define ST_BASE (0x20003000)
#define TIMER_OFFSET (4)

//must run as root
long long int gettimer(void) {
long long int t, prev, *timer; // 64 bit timer
    int fd;
    void *st_base; // byte ptr to simplify offset math
 
    // get access to system core memory
    if (-1 == (fd = open("/dev/mem", O_RDONLY))) {
        fprintf(stderr, "open() failed.\n");
        return 255;
    }
 
    // map a specific page into process's address space
    if (MAP_FAILED == (st_base = mmap(NULL, 4096,
                        PROT_READ, MAP_SHARED, fd, ST_BASE))) {
        fprintf(stderr, "mmap() failed.\n");
        return 254;
    }
 
    // set up pointer, based on mapped page
    timer = (long long int *)((char *)st_base + TIMER_OFFSET);
 
    // read initial timer
    t = *timer;
    // and wait
    //sleep(1);
 
    //while (1==1) { // forever
        // read new timer
      // t = *timer;
        // print difference (and flush output)
       // printf("Timer diff = %lld    \r", t - prev);
       // fflush(stdout);
        // save current timer
        //prev = t;
        // and wait
        //sleep(1);
    //}
    // will never get here
    return t;
}










/*http://raspi.tv/2013/how-to-use-soft-pwm-in-rpi-gpio-pt-2-led-dimming-and-motor-speed-control*/

/* From comment: 
Thank you for this tutorial. It’s very clear and helpful. I will suggest that a more common cycling loop pattern would be:

lo, hi, step = 0, 10, 1
while True:
for i in range(min(lo, hi), max(lo, hi), abs(step)):
#white.ChangeDutyCycle(lo)
#red.ChangeDutyCycle(hi)
print (lo, hi)
lo += step
hi -= step
step *= -1

and if you hate the use of min, max, and abs you can always duplicate the variable at the beginning:

lo, hi, step = 0, 10, 1
rlo, rhi, rstep = lo, hi, step
while True:
for i in range(rlo, rhi, rstep):
#white.ChangeDutyCycle(lo)
#red.ChangeDutyCycle(hi)
print (lo, hi)
lo += step
hi -= step
step *= -1
*/
