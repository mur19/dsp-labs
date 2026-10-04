#include "Timer.h"
// #include "PWM.h"
// #include "QEP.h"
#include "DSP2833x_Device.h"
#include "CPU_Rate.h"
// #include "ADC.h"
// #include "CAN.h"
#include "GPIO.h"
#include "IQMathLib.h"

void DAC_0_Write(int Value);
void DAC_1_Write(int Value);

#define PI 3.1415926535

// #define SIN_METHOD_CHEBYSHEV 1
#define SIN_METHOD_CORDIC 2
#define SIN_METHOD SIN_METHOD_CORDIC

double A1;
_iq SIN_value_prev;
_iq diff_sin, int_sin;
int epwm1_irq_cnt;
_iq SIN_Value;
int DAC_Value_1, DAC_Value_0;
extern _iq Time_Delta, Time;
int i;

static void reducer(_iq *x)
{
	if (*x > _IQ(0.999))
		*x = _IQ(0.999);
	if (*x < _IQ(-0.999))
		*x = _IQ(-0.999);
}

static _iq quadrant_reduce(_iq theta, int *sign)
{
	_iq quadrant_q = _IQdiv(theta, _IQ(PI / 2));
	long quadrant = _IQint(quadrant_q);

	switch (quadrant)
	{
	case 0:
		*sign = 1;
		return theta;
	case 1:
		*sign = 1;
		return _IQ(PI) - theta;
	case 2:
		*sign = -1;
		return theta - _IQ(PI);
	default:
		*sign = -1;
		return _IQ(2 * PI) - theta;
	}
}

// static _iq sin_chebyshev(_iq theta)
// {
// 	int sign;
// 	_iq x = _IQdiv(quadrant_reduce(theta, &sign), _IQ(PI / 2));
// 	_iq x2 = _IQmpy(x, x);
// 	_iq x3 = _IQmpy(x2, x);
// 	_iq x4 = _IQmpy(x2, x2);
// 	_iq x5 = _IQmpy(x4, x);
// 	_iq poly = _IQmpy(_IQ(1.57035062), x) + _IQmpy(_IQ(0.00508719), x2) - _IQmpy(_IQ(0.66666099), x3) + _IQmpy(_IQ(0.03610310), x4) + _IQmpy(_IQ(0.05512166), x5);

// 	return (sign > 0) ? poly : -poly;
// }

#define CORDIC_ITERATIONS 14
#define CORDIC_K_IDEAL _IQ(0.60725293500888)

static _iq CORDIC_K_REAL(void)
{
	_iq K = _IQ(1.0);
	_iq poww = _IQ(1.0);
	int i;
	for (i = 0; i < CORDIC_ITERATIONS; i++)
	{
		K = _IQdiv(K, _IQsqrt(_IQ(1.0) + _IQmpy(poww, poww)));
		poww = _IQdiv(poww, _IQ(2.0));
	}
	return K;
}

static const _iq cordic_atan_table[CORDIC_ITERATIONS] = {
	_IQ(0.785398163397448), _IQ(0.463647609000806), _IQ(0.244978663126864),
	_IQ(0.124354994546761), _IQ(0.062418809995957), _IQ(0.031239833430268),
	_IQ(0.015623728620477), _IQ(0.007812341060101), _IQ(0.003906230131967),
	_IQ(0.001953122516479), _IQ(0.000976562189559), _IQ(0.000488281211195),
	_IQ(0.000244140620149), _IQ(0.000122070311894)};

static _iq sin_cordic(_iq theta)
{
	int sign;
	_iq z = _IQ(0.0);
	_iq x = CORDIC_K_IDEAL;
	// _iq x = CORDIC_K_REAL();
	_iq y = 0;
	int i;
	_iq poww = _IQ(1.0);

	theta = quadrant_reduce(theta, &sign);

	for (i = 0; i < CORDIC_ITERATIONS; i++)
	{
		_iq d;
		_iq x_new;
		_iq y_new;

		if (z <= theta)
		{
			d = _IQ(1.0);
		}
		else
		{
			d = _IQ(-1.0);
		}

		x_new = x - _IQmpy(d, _IQmpy(y, poww));
		y_new = y + _IQmpy(d, _IQmpy(x, poww));

		x = x_new;
		y = y_new;

		z = z + _IQmpy(d, cordic_atan_table[i]);

		poww = _IQdiv(poww, _IQ(2.0));
	}

	return (sign > 0) ? y : -y;
}

static _iq diff(_iq f_curr, _iq f_prev, _iq dT)
{
	return _IQdiv((f_curr - f_prev), dT);
}

static _iq integr(_iq f_curr, _iq F_prev, _iq dT)
{
	return F_prev + _IQmpy(dT, f_curr);
}

interrupt void timer0_isr(void)
{

	A1 = 1;

	gpio0_clear();

#if SIN_METHOD == SIN_METHOD_CHEBYSHEV
	SIN_Value = sin_chebyshev(Time);
#else
	SIN_Value = sin_cordic(Time);
#endif
	SIN_Value = _IQmpy(SIN_Value, _IQ(A1));

	reducer(&SIN_Value);

	if (Time != 0)
	{
		diff_sin = diff(SIN_Value, SIN_value_prev, Time_Delta);
		reducer(&diff_sin);

		int_sin = integr(SIN_Value, int_sin, Time_Delta);
		reducer(&int_sin);
	}

	SIN_value_prev = SIN_Value;

	gpio0_set();

	DAC_Value_1 = _IQtoQ11(SIN_Value);
	DAC_1_Write(DAC_Value_1);
	if (Time != 0)
	{
		DAC_Value_0 = _IQtoQ11(diff_sin);
		DAC_1_Write(DAC_Value_0);
	}

	Time = Time + Time_Delta;
	if (Time >= _IQ(2 * PI))
	{
		Time = Time - _IQ(2 * PI);
	}

	// Timer0 Interrupt Flag Clear
	CpuTimer0Regs.TCR.bit.TIF = 1;
	// PIE Group1 Acknowledgement Reset
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
