#ifndef __PWM_ISR_H__
#define __PWM_ISR_H__

#include "DSP2833x_Device.h"     // DSP280x Headerfile Include File
#include "IQmathLib.h"     				// IQMath Include file

extern int epwm1_irq_cnt;
extern _iq iqZadVoltage[];

interrupt void epwm1_isr(void);

#endif


