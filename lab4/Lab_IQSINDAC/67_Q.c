#include "Timer.h"
#include "Peripheral_isr.h"

#include "DSP2833x_Device.h"
#include "CPU_Rate.h"

#include "GPIO.h"
#include "IQMathLib.h"

#if GLOBAL_Q != 28 || MATH_TYPE != IQ_MATH
#error This implementation requires fixed-point IQ28 (GLOBAL_Q = 28).
#endif

void DAC_0_Write(int Value);
void DAC_1_Write(int Value);

#define PI 3.1415926535

#define SIN_METHOD_CHEBYSHEV 1
#define SIN_METHOD_CORDIC 2
#define SIN_METHOD SIN_METHOD_CORDIC

_iq A1 = _IQ(1.0);
int epwm1_irq_cnt;
_iq SIN_Value;
int DAC_Value;
extern _iq Time_Delta, Time;

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

#define K 3

_iq26 FOSR = _IQ26(16.0);
_iq26 Sinc_Count = _IQ26(0.0);
_iq Sinc_Fosr_Inv;

_iq18 Sinc_Integr[K];
_iq18 Sinc_Diff[K];
_iq18 Sinc_Diff_Prev[K];
_iq Sinc_A_Norm;
_iq Sinc_Out;
int Sinc_Ready;
int DAC_Sinc_Value;

static _iq18 sinc_iq_add(_iq18 a, _iq18 b)
{
    _iq18 low = (a & 0xffffL) + (b & 0xffffL);
    _iq18 high = (a >> 16) + (b >> 16) + (low >> 16);
    high = high & 0xffffL;
    if (high >= 0x8000L) high = high - 0x10000L;
    return high * 0x10000L + (low & 0xffffL);
}

static _iq18 sinc_iq_sub(_iq18 a, _iq18 b)
{
    _iq18 low = (a & 0xffffL) - (b & 0xffffL);
    _iq18 high = (a >> 16) - (b >> 16) + (low >> 16);
    high = high & 0xffffL;
    if (high >= 0x8000L) high = high - 0x10000L;
    return high * 0x10000L + (low & 0xffffL);
}

static void afc_init(void);

void sinc_init(void)
{
    _iq26 norm = _IQ26(1.0);
    int i;

    Sinc_Fosr_Inv = _IQ26toIQ(_IQ26div(_IQ26(1.0), FOSR));
    for (i = 0; i < K; i++)
    {
        norm = _IQ26div(norm, FOSR);
        Sinc_Integr[i] = _IQ18(0.0);
        Sinc_Diff[i] = _IQ18(0.0);
        Sinc_Diff_Prev[i] = _IQ18(0.0);
    }
    Sinc_A_Norm = _IQ26toIQ(norm);
    Sinc_Count = _IQ26(0.0);
    Sinc_Out = _IQ(0.0);
    Sinc_Ready = 0;
    DAC_Sinc_Value = 0;
    afc_init();
}

void sinc_filter(_iq x)
{
    _iq18 x_scale;
    _iq18 in;
    int i;

    Sinc_Ready = 0;

    x_scale = _IQtoIQ18(x);

    for (i = K - 1; i > 0; i--)
    {
        Sinc_Integr[i] = sinc_iq_add(Sinc_Integr[i], Sinc_Integr[i - 1]);
    }

    Sinc_Integr[0] = sinc_iq_add(Sinc_Integr[0], x_scale);

    Sinc_Count = Sinc_Count + _IQ26(1.0);

    if (Sinc_Count == FOSR)
    {
        Sinc_Count = _IQ26(0.0);

        for (i = K - 1; i >= 0; i--)
        {
            if (i == 0) {
                in = Sinc_Integr[K - 1];
            }
            else {
                in = Sinc_Diff[i - 1];
            }
            Sinc_Diff[i] = sinc_iq_sub(in, Sinc_Diff_Prev[i]);
            Sinc_Diff_Prev[i] = in;
        }

        Sinc_Out = _IQmpyIQX(Sinc_Diff[K - 1], 18, Sinc_A_Norm, GLOBAL_Q);
        Sinc_Ready = 1;
    }
}

#define AFC_N 128
#define AFC_SETTLE 8

#define AFC_MEAS (4 * AFC_N)

long Afc_Freq[AFC_N];
_iq Afc_Meas[AFC_N];
_iq Afc_Theory[AFC_N];
volatile int Afc_Run = 0;
volatile int Afc_Done = 0;
volatile int Afc_Error = 0;
int Afc_Index = 0;
int Afc_Count = 0;
_iq Afc_Delta_Saved;
static int Afc_Active = 0;
static _iq Afc_In_Sin, Afc_In_Cos;
static _iq Afc_Out_Sin, Afc_Out_Cos;

static _iq afc_u(int j)
{
    return (2 * j + 1) * (_IQ(1.0) / (4 * AFC_N));
}

static _iq afc_theory(_iq u)
{
    _iq b = _IQmpy(_IQ(PI), u);
    _iq a = 0;
    _iq r;
    _iq h = _IQ(1.0);
    _iq26 count;
    int i;

    for (count = _IQ26(0.0); count < FOSR; count += _IQ26(1.0))
    {
        a = a + b;
        if (a >= _IQ(PI))
        {
            a = a - _IQ(2 * PI);
        }
    }

    r = _IQdiv(_IQmpy(_IQsin(a), Sinc_Fosr_Inv), _IQsin(b));
    if (r < 0)
    {
        r = -r;
    }

    for (i = 0; i < K; i++)
    {
        h = _IQmpy(h, r);
    }
    return h;
}

static void afc_init(void)
{
    int j;

    Afc_Run = 0;
    Afc_Done = 0;
    Afc_Error = 0;
    Afc_Active = 0;
    Afc_Index = 0;
    Afc_Count = 0;
    Afc_In_Sin = Afc_In_Cos = _IQ(0.0);
    Afc_Out_Sin = Afc_Out_Cos = _IQ(0.0);
    for (j = 0; j < AFC_N; j++)
    {
        Afc_Freq[j] = (2L * j + 1) * FS / (4L * AFC_N);
        Afc_Theory[j] = afc_theory(afc_u(j));
        Afc_Meas[j] = 0;
    }
}

static _iq afc_magnitude(_iq a, _iq b)
{
    _iq ratio;
    _iq temp;

    if (a < 0) a = -a;
    if (b < 0) b = -b;
    if (b > a)
    {
        temp = a;
        a = b;
        b = temp;
    }
    if (a == 0) return _IQ(0.0);
    ratio = _IQdiv(b, a);
    return _IQmpy(a, _IQsqrt(_IQ(1.0) + _IQmpy(ratio, ratio)));
}

static void afc_step(void)
{
    _iq ref_sin, ref_cos;
    _iq input_amplitude, output_amplitude;
    int j;

    if (Afc_Run == 0)
    {
        if (Afc_Active)
        {
            Time_Delta = Afc_Delta_Saved;
            Afc_Active = 0;
            Afc_Index = 0;
            Afc_Count = 0;
        }
        return;
    }

    if (!Afc_Active)
    {
        Afc_Active = 1;
        Afc_Done = 0;
        Afc_Error = 0;
        Afc_Index = 0;
        Afc_Count = 0;
        Afc_In_Sin = Afc_In_Cos = _IQ(0.0);
        Afc_Out_Sin = Afc_Out_Cos = _IQ(0.0);
        for (j = 0; j < AFC_N; j++) Afc_Meas[j] = _IQ(0.0);
        Afc_Delta_Saved = Time_Delta;
        Time_Delta = _IQmpy(_IQ(2 * PI), afc_u(0));
        return;
    }

    Afc_Count = Afc_Count + 1;
    if (Afc_Count <= AFC_SETTLE)
    {
        return;
    }

    ref_sin = _IQsin(Time);
    ref_cos = _IQcos(Time);
    Afc_In_Sin += _IQmpy(SIN_Value, ref_sin) / AFC_MEAS;
    Afc_In_Cos += _IQmpy(SIN_Value, ref_cos) / AFC_MEAS;
    Afc_Out_Sin += _IQmpy(Sinc_Out, ref_sin) / AFC_MEAS;
    Afc_Out_Cos += _IQmpy(Sinc_Out, ref_cos) / AFC_MEAS;

    if (Afc_Count == AFC_SETTLE + AFC_MEAS)
    {
        input_amplitude = afc_magnitude(Afc_In_Sin, Afc_In_Cos);
        output_amplitude = afc_magnitude(Afc_Out_Sin, Afc_Out_Cos);
        if (input_amplitude == 0)
        {
            Afc_Error = 1;
            Afc_Run = 0;
            Afc_Active = 0;
            Time_Delta = Afc_Delta_Saved;
            return;
        }

        Afc_Meas[Afc_Index] = _IQdiv(output_amplitude, input_amplitude);
        Afc_In_Sin = Afc_In_Cos = _IQ(0.0);
        Afc_Out_Sin = Afc_Out_Cos = _IQ(0.0);
        Afc_Count = 0;
        Afc_Index = Afc_Index + 1;

        if (Afc_Index == AFC_N)
        {

            Afc_Index = 0;
            Afc_Run = 0;
            Afc_Active = 0;
            Time_Delta = Afc_Delta_Saved;
            Afc_Done = 1;
        }
        else
        {

            Time_Delta = _IQmpy(_IQ(2 * PI), afc_u(Afc_Index));
        }
    }
}

void reducer(_iq *val) {
    if (*val > _IQ(0.999))
        *val = _IQ(0.999);
    if (*val < _IQ(-0.999))
        *val = _IQ(-0.999);
}

interrupt void timer0_isr(void)
{
    _iq Period = _IQ(2 * PI);

    gpio0_clear();

#if SIN_METHOD == SIN_METHOD_CHEBYSHEV
    SIN_Value = sin_chebyshev(Time);
#else
    SIN_Value = sin_cordic(Time);
#endif
    SIN_Value = _IQmpy(SIN_Value, A1);

    reducer(&SIN_Value);

    sinc_filter(SIN_Value);

    reducer(&Sinc_Out);

    gpio0_set();

    DAC_Value = _IQtoQ11(SIN_Value);
    DAC_1_Write(DAC_Value);
    if (Sinc_Ready)
    {
        DAC_Sinc_Value = _IQtoQ11(Sinc_Out);
        DAC_0_Write(DAC_Sinc_Value);
        afc_step();
    }

    if (Time >= Period - Time_Delta)
    {
        Time = Time - (Period - Time_Delta);
    }

    else
    {
        Time = Time + Time_Delta;
    }

    CpuTimer0Regs.TCR.bit.TIF = 1;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
