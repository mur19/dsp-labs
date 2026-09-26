#ifndef __PWM_H__
#define __PWM_H__

#include "DSP2833x_Device.h" 

#define 	SYSCLKOUT		150000000	//Hz

#define		CLKDIVVALUE			0
#define		HSPCLKDIVVALUE		0

#define		TBCLK			SYSCLKOUT/((HSPCLKDIVVALUE+1)*(CLKDIVVALUE+1))

#define		PWMFREQUENCY	100000		//Hz

#define		TBPRDVALUE			TBCLK/(2*PWMFREQUENCY)


//-- DEADBAND не может быть больше 6.8 мкс (при SYSCLKOUT=TBCLK=150ћ√ц), потому что регистры
//-- EPwmXRegs.DBRED и EPwmXRegs.DBFED всего 10-разр€дные

#define 	DEADBAND		1

#define		DBFEDVALUE			(TBCLK/1000000)*DEADBAND
#define		DBREDVALUE			(TBCLK/1000000)*DEADBAND

#if DBFEDVALUE > 1023
	#error DBFEDVALUE > 1023
#endif
#if DBREDVALUE > 1023
	#error DBREDVALUE > 1023
#endif

#define PWM_COUNT_UP		0
#define PWM_COUNT_DOWN		1
#define PWM_COUNT_UP_DOWN	2

#define PWM_TRISTATE_ENABLE  1
#define PWM_TRISTATE_DISABLE 0

typedef struct {
	unsigned long period_in_cpu_cycles;
 	unsigned short count_mode;
	unsigned short tristate_enable;
	unsigned int deadband;
} PWM_CONFIG;

void pwm_init(PWM_CONFIG *pwm_config);
void pwm_set_zero_voltage(unsigned long pwm_period_in_cpu_cycles);

extern unsigned int pwm_isr_count;

#endif
