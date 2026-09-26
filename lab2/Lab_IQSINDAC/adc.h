#ifndef __ADC_H__
#define __ADC_H__

#include "DSP2833x_Device.h"
extern unsigned long adc_isr_count;

typedef struct  {
	unsigned short conversion_num;
	unsigned short conversion0_src;
	unsigned short conversion1_src;
	unsigned short conversion2_src;
	unsigned short conversion3_src;
	unsigned short conversion4_src;
	unsigned short conversion5_src;
} ADC_CONFIG;

void adc_init(ADC_CONFIG *adc_config);

#define ADCINA0 0x0
#define ADCINA1 0x1 
#define ADCINA2 0x2
#define ADCINA3 0x3
#define ADCINA4 0x4 
#define ADCINA5 0x5
#define ADCINA6 0x6 

#endif

