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
_iq sin_value_prev;
_iq differentiation_sin;
_iq differentiation_second_sin;
_iq int_sin = _IQ(-1);
_iq diff_prev = _IQ(1); 
int epwm1_irq_cnt;
_iq sin_value;
int DAC_Value_1, DAC_Value_0;
extern _iq time_delta, time;
int i;

static void Reducer(_iq *x)
{
	if (*x > _IQ(0.999))
		*x = _IQ(0.999);
	if (*x < _IQ(-0.999))
		*x = _IQ(-0.999);
}

static _iq QuadrantReduce(_iq theta, int *sign)
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
// 	_iq x = _IQdiv(QuadrantReduce(theta, &sign), _IQ(PI / 2));
// 	_iq x2 = _IQmpy(x, x);
// 	_iq x3 = _IQmpy(x2, x);
// 	_iq x4 = _IQmpy(x2, x2);
// 	_iq x5 = _IQmpy(x4, x);
// 	_iq poly = _IQmpy(_IQ(1.57035062), x) + _IQmpy(_IQ(0.00508719), x2) - _IQmpy(_IQ(0.66666099), x3) + _IQmpy(_IQ(0.03610310), x4) + _IQmpy(_IQ(0.05512166), x5);

// 	return (sign > 0) ? poly : -poly;
// }

#define CORDIC_ITERATIONS 14
#define CORDIC_K_IDEAL _IQ(0.60725293500888)

static _iq CordicKReal(void)
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

static _iq SinCordic(_iq theta)
{
	int sign;
	_iq z = _IQ(0.0);
	_iq x = CORDIC_K_IDEAL;
	// _iq x = CordicKReal();
	_iq y = 0;
	int i;
	_iq poww = _IQ(1.0);

	theta = QuadrantReduce(theta, &sign);

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

static _iq DifferentiationFirst(_iq f_curr, _iq f_prev, _iq dT)
{
	return _IQdiv((f_curr - f_prev), dT);
}

static _iq DifferentiationSecond(_iq diff_curr, _iq diff_prev, _iq dT)
{
	return _IQdiv((diff_curr - diff_prev), dT);
}

static _iq RectangularIntegration(_iq f_curr, _iq f_prev, _iq int_prev, _iq dT)
{
	return int_prev + _IQmpy(dT, f_curr);
}

static _iq TrapezoidIntegration(_iq f_curr, _iq f_prev, _iq int_prev, _iq dT)
{  
	return int_prev + _IQmpy(dT, (f_curr + f_prev) >> 1);
} 

interrupt void timer0_isr(void)
{

	A1 = 1;

	gpio0_clear();

#if SIN_METHOD == SIN_METHOD_CHEBYSHEV
	sin_value = sin_chebyshev(time);
#else
	sin_value = SinCordic(time);
#endif
	sin_value = _IQmpy(sin_value, _IQ(A1));

	Reducer(&sin_value);

	if (time != 0)
	{
		differentiation_sin = DifferentiationFirst(sin_value, sin_value_prev, time_delta);

		differentiation_second_sin = DifferentiationSecond(differentiation_sin, diff_prev, time_delta);
		diff_prev = differentiation_sin;
		Reducer(&differentiation_sin);
		rectan_int_sin = RectangularIntegration(sin_value, sin_value_prev, int_sin, time_delta);
		trapezoid_int_sin = TrapezoidIntegration(sin_value, sin_value_prev, int_sin, time_delta);
		Reducer(&rectan_int_sin);
		Reducer(&trapezoid_int_sin);
	}

	sin_value_prev = sin_value;

	gpio0_set();

	DAC_Value_1 = _IQtoQ11(sin_value);
	DAC_1_Write(DAC_Value_1);
	if (time != 0)
	{
		DAC_Value_0 = _IQtoQ11(differentiation_sin);
		DAC_1_Write(DAC_Value_0);
	}

	time = time + time_delta;
	if (time >= _IQ(2 * PI))
	{
		time = time - _IQ(2 * PI);
	}

	// Timer0 Interrupt Flag Clear
	CpuTimer0Regs.TCR.bit.TIF = 1;
	// PIE Group1 Acknowledgement Reset
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
