// Standalone host-side test for the sin_chebyshev / sin_cordic algorithm
// used in lab2/Lab_IQSINDAC/Peripheral_isr.c.
//
// No DSP / CCS / hardware needed: IQmathLib.h has a MATH_TYPE == FLOAT_MATH
// mode that replaces all _IQ* fixed-point macros with plain float + math.h
// calls, so the exact same algorithm code compiles with a normal host
// compiler (gcc). We drive "Time" manually in a loop instead of relying on
// the real hardware timer interrupt.
//
// Build:   gcc -I "../Lab_IQSINDAC" -DMATH_TYPE=FLOAT_MATH -lm test_sin_lab2.c -o test_sin_lab2.exe
// Run:     ./test_sin_lab2.exe

#include <stdio.h>
#include <math.h>
#include "IQmathLib.h"

#define PI 3.1415926535

// ---- copied verbatim from Peripheral_isr.c (pure math, no hardware) ----

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

static _iq sin_chebyshev(_iq theta)
{
	int sign;
	_iq x = _IQdiv(quadrant_reduce(theta, &sign), _IQ(PI / 2));
	_iq x2 = _IQmpy(x, x);
	_iq x3 = _IQmpy(x2, x);
	_iq x4 = _IQmpy(x2, x2);
	_iq x5 = _IQmpy(x4, x);
	_iq poly = _IQmpy(_IQ(1.57035062), x) + _IQmpy(_IQ(0.00508719), x2) - _IQmpy(_IQ(0.66666099), x3) + _IQmpy(_IQ(0.03610310), x4) + _IQmpy(_IQ(0.05512166), x5);

	return (sign > 0) ? poly : -poly;
}

#define CORDIC_ITERATIONS 14
#define CORDIC_K_IDEAL _IQ(0.60725293500888)

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
			d = _IQ(1.0);
		else
			d = _IQ(-1.0);

		x_new = x - _IQmpy(d, _IQmpy(y, poww));
		y_new = y + _IQmpy(d, _IQmpy(x, poww));

		x = x_new;
		y = y_new;

		z = z + _IQmpy(d, cordic_atan_table[i]);

		poww = _IQdiv(poww, _IQ(2.0));
	}

	return (sign > 0) ? y : -y;
}

// ---- test harness: manually drive "Time" like the real ISR would ----

int main(void)
{
	_iq Time = _IQ(0.0);
	_iq Time_Delta = _IQ(0.02); // constant step per "tick", instead of a real timer interrupt

	double max_err_cheb = 0.0, max_err_cordic = 0.0;

	printf("%10s %12s %12s %12s %12s %12s\n",
		   "Time", "reference", "chebyshev", "err_cheb", "cordic", "err_cordic");

	while (Time < _IQ(2 * PI))
	{
		double reference = sin((double)Time);
		_iq cheb = sin_chebyshev(Time);
		_iq cordic = sin_cordic(Time);

		double err_cheb = fabs((double)cheb - reference);
		double err_cordic = fabs((double)cordic - reference);
		if (err_cheb > max_err_cheb) max_err_cheb = err_cheb;
		if (err_cordic > max_err_cordic) max_err_cordic = err_cordic;

		printf("%10.4f %12.6f %12.6f %12.6f %12.6f %12.6f\n",
			   (double)Time, reference, (double)cheb, err_cheb, (double)cordic, err_cordic);

		Time = Time + Time_Delta;
	}

	printf("\nMax abs error: chebyshev=%.6f  cordic=%.6f\n", max_err_cheb, max_err_cordic);
	return 0;
}
