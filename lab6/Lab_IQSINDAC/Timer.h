#ifndef TIMER_H
#define TIMER_H

#include "DSP2833x_Device.h"
#include "GPIO.h"

typedef struct {
	unsigned long period_in_cpu_cycles;
	unsigned int prescale;
} TIMER_CONFIG;

void timer0_init(TIMER_CONFIG *timer_config);
interrupt void timer0_isr(void);

#endif
