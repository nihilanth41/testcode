#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#define ARRAY_SIZE 4096

int main(void) {
#define R1 10000
#define R2 680
#define VCC 3.56 //volts
#define AREF VCC //use VCC as ADC Reference voltage
#define ADC_RES 4095 //12 bit adc with oversampling/decimation
double resistor_divider = (((R1)+(R2))/(R2)); //10680/680
double mvpd = ((AREF)/(ADC_RES)); 
int array_size = 4096;
printf("static const double voltage_lt[%d] = { \n", array_size);
static int x; 
static double vout; 
for(x=0; x<ARRAY_SIZE; x++) {
vout = ((x*mvpd)*resistor_divider);
printf("%2.10f, ", vout);
}
printf("};\n");
return 0; }



