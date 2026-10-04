// Standalone host-side test for the integr() numerical-integration block
// added in lab3/Lab_IQSINDAC/Peripheral_isr.c (int_sin = running integral
// of SIN_Value over time, via rectangular/Euler integration).
//
// No DSP / CCS / hardware needed: IQmathLib.h has a MATH_TYPE == FLOAT_MATH
// mode that replaces all _IQ* fixed-point macros with plain float + math.h
// calls, so the exact same algorithm code compiles with a normal host
// compiler (gcc). We drive "Time" manually in a loop instead of relying on
// the real hardware timer interrupt, exactly like test_diff_lab3.c does.
//
// Build:   gcc -I "../Lab_IQSINDAC" -DMATH_TYPE=FLOAT_MATH -lm test_integr_lab3.c -o test_integr_lab3.exe
// Run:     ./test_integr_lab3.exe

#include <stdio.h>
#include <math.h>
#include "IQmathLib.h"

#define PI 3.1415926535

// ---- copied verbatim from Peripheral_isr.c (pure math, no hardware) ----

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

static _iq integr(_iq f_curr, _iq F_prev, _iq dT)
{
	return F_prev + _IQmpy(dT, f_curr);
}

// ---- test harness: manually drive "Time" like the real ISR would ----

int main(void)
{
	_iq Time = _IQ(0.0);
	_iq Time_Delta = _IQ(0.02); // same step as Peripheral_isr.c would use per tick

	_iq SIN_Value;
	_iq int_sin = _IQ(0.0);

	double max_err = 0.0;
	double max_err_time = 0.0;
	int checked_zero_is_skipped = 0;

	printf("%10s %12s %12s %12s %12s\n",
		   "Time", "SIN_Value", "int_sin", "ref_1-cos", "error");

	while (Time < _IQ(2 * PI))
	{
		// --- mirrors the body of timer0_isr() in Peripheral_isr.c ---
		SIN_Value = sin_cordic(Time);
		reducer(&SIN_Value);

		if (Time != 0)
		{
			int_sin = integr(SIN_Value, int_sin, Time_Delta);
			reducer(&int_sin);
		}
		// -------------------------------------------------------------

		if (Time == 0)
		{
			// At Time == 0 there is no accumulation step yet, same as
			// diff_sin in Peripheral_isr.c: int_sin must stay untouched
			// (still its init value, 0) here.
			if (int_sin != _IQ(0.0))
			{
				printf("FAIL: int_sin was written at Time == 0 "
					   "(got %.6f), it must be skipped\n",
					   (double)int_sin);
				return 1;
			}
			checked_zero_is_skipped = 1;
			printf("%10.4f %12.6f %12s %12s %12s  (skipped, no accumulation yet)\n",
				   (double)Time, (double)SIN_Value, "-", "-", "-");
		}
		else
		{
			// Reference integral of sin(t) dt from 0 to Time is 1 - cos(Time).
			// Note: this is the integral of the IDEAL sin(), while int_sin
			// accumulates the reducer()-clamped sin_cordic() output, so a
			// little extra error beyond pure discretization is expected
			// once int_sin itself gets clamped to +/-0.999.
			double reference = 1.0 - cos((double)Time);
			double err = fabs((double)int_sin - reference);

			printf("%10.4f %12.6f %12.6f %12.6f %12.6f\n",
				   (double)Time, (double)SIN_Value, (double)int_sin, reference, err);

			if (err > max_err)
			{
				max_err = err;
				max_err_time = (double)Time;
			}
		}

		Time = Time + Time_Delta;
	}

	if (!checked_zero_is_skipped)
	{
		printf("FAIL: loop never visited Time == 0, the skip branch was not exercised\n");
		return 1;
	}

	printf("\nMax abs error: %.6f at Time=%.4f (Time_Delta=%.4f)\n",
		   max_err, max_err_time, (double)Time_Delta);

	return 0;
}
