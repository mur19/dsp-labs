#ifndef __MAIN_H__
#define __MAIN_H__

#include "DSP2833x_Device.h"     // DSP280x Headerfile Include File
#include "IQmathLib.h"     				// IQMath Include file
#include "DSP2833x_CpuTimers.h"

#define TRUE 1
void forever_loop(void);
void gpio_init(void);
void enable_interrupts(void);
void disable_interrupts(void);

void system_control_init(void);
void disable_watchdog(void);

#define TECHNOSOFT_LAB 1
#define DENISOV_LAB 2

//#define LAB_TARGET TECHNOSOFT_LAB
#define LAB_TARGET DENISOV_LAB

#if LAB_TARGET == DENISOV_LAB
	#undef ADC_TWO_CURRENTS
	#undef PWM_TRISTATE
	#undef ADC_THREE_CURRENTS
	#undef HAS_USB_INTERFACE
	#undef HAS_LED

	#define PWM_TRISTATE
	#define ADC_THREE_CURRENTS
	#define HAS_USB_INTERFACE
#endif

#if LAB_TARGET == TECHNOSOFT_LAB
	#undef ADC_THREE_CURRENTS
	#undef HAS_USB_INTERFACE
	#undef PWM_TRISTATE
	#undef ADC_TWO_CURRENTS
	#undef HAS_LED

	#define ADC_TWO_CURRENTS
//	#define PWM_TRISTATE
	#define HAS_LED
#endif

// ---возможные типы системы ---
#define SYS_POSITION		0
#define SYS_SPEED			1
#define SYS_VOLTAGE			2
#define SYS_CURRENT			3
#define SYS_VMP				4
#define SYS_ONE_PHASE		8
#define SYS_VMP2			10
#define SYS_CORRIDOR		12 //--возможен конфликт с другим типом системы
#define SYS_HALL_SENSOR     14
#define SYS_FILTER			15

extern int StatusWord;
extern int Run_Flag;
extern int SimulinkMode;

extern int ADC_DATA0;
extern int ADC_DATA1;
extern int ADC_DATA2;
extern int ADC_DATA3;
extern int ADC_DATA4;
extern int ADC_DATA5;

extern long ADC_Offset_0;
extern long ADC_Offset_1;
extern long ADC_Offset_2;
extern long ADC_Offset_3;
extern long ADC_Offset_4;
extern long ADC_Offset_5;

extern long ADC_Koef_0;
extern long ADC_Koef_1;
extern long ADC_Koef_2;
extern long ADC_Koef_3;
extern long ADC_Koef_4;
extern long ADC_Koef_5;

extern _iq Current_IQ_A;
extern _iq Current_IQ_B;
extern _iq Current_IQ_C;

extern _iq Cur_Moment;

extern Uint16	System_Type;
extern _iq Given_Value_IQ;
extern _iq Given_Value_IQ2;

extern int PolusParCnt;
extern long DPDiscrCnt;
extern long OldDPDiscrCnt;
extern long QEP_Position;

extern int NewTargetFlag;

#endif


