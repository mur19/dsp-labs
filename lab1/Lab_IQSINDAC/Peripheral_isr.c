#include "Timer.h"
//#include "PWM.h"
//#include "QEP.h"
#include "DSP2833x_Device.h"
#include "CPU_Rate.h"
//#include "ADC.h"
//#include "CAN.h"
#include "GPIO.h"
#include "IQMathLib.h"

void DAC_0_Write(int Value);
void DAC_1_Write(int Value);

#define PI 3.14159

int epwm1_irq_cnt;
_iq SIN_Value, SIN_Value_2;
int DAC_Value;
extern _iq Time_Delta, Time_Delta_2, Time_Delta_3, Time, Time_2, Time_3;


interrupt void timer0_isr(void) {

    gpio0_set();

	SIN_Value = _IQsin(Time);
	SIN_Value =  _IQmpy(SIN_Value,_IQ(0.5));
//	if (SIN_Value > _IQ(0.999)) SIN_Value = _IQ(0.999);
//	if (SIN_Value < _IQ(-0.999)) SIN_Value = _IQ(-0.999);

	if (SIN_Value > _IQ(0.999)) SIN_Value = _IQ(0.999);
	if (SIN_Value < _IQ(-0.999)) SIN_Value = _IQ(-0.999);

	gpio0_clear();

// Sin Value is written in DAC Channel 0.
	DAC_Value = _IQtoQ11(SIN_Value);
	DAC_1_Write(DAC_Value);

	Time = Time + Time_Delta;
	if (Time >= _IQ(2*PI))
		{ Time = Time - _IQ(2*PI);  
		}

// Timer0 Interrupt Flag Clear
	CpuTimer0Regs.TCR.bit.TIF = 1;
// PIE Group1 Acknowledgement Reset
   	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

