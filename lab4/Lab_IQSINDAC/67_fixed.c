#include "Timer.h"
#include "Peripheral_isr.h"
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

#define SIN_METHOD_CHEBYSHEV 1
#define SIN_METHOD_CORDIC 2
#define SIN_METHOD SIN_METHOD_CORDIC

double A1;
int epwm1_irq_cnt;
_iq SIN_Value;
int DAC_Value;
extern _iq time_delta, time;

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

#define K 3
#define IN_BITS 15
int FOSR = 16   ;

// unsigned нужен чтобы грамотно реализовать логику переполнения интеграторов
// только в unsigned long разрешается переполнение
unsigned long Sinc_Integr[K];
unsigned long Sinc_Diff[K];
unsigned long Sinc_Diff_Prev[K];
int Sinc_Count = 0;

_iq Sinc_A_Norm;
int Sinc_DW;
_iq Sinc_Out;
int Sinc_Ready;
int DAC_Sinc_Value;

void reducer(_iq *val) {
    if (*val > _IQ(0.999))
        *val = _IQ(0.999);
    if (*val < _IQ(-0.999))
        *val = _IQ(-0.999);
}

void sinc_filter(_iq x)
{
    unsigned long v;
    unsigned long in;
    int i;

    Sinc_Ready = 0;

    // IQ (Q28) -> входной код с IN_BITS битами после запятой
    v = x >> (GLOBAL_Q - IN_BITS);

    for (i = K - 1; i > 0; i--)
    {
        Sinc_Integr[i] = Sinc_Integr[i] + Sinc_Integr[i - 1];
    }
    Sinc_Integr[0] = Sinc_Integr[0] + v;

    Sinc_Count = Sinc_Count + 1;
    if (Sinc_Count == FOSR)
    {
        Sinc_Count = 0;

        for (i = K - 1; i >= 0; i--)
        {
            if (i == 0) {
                in = Sinc_Integr[K - 1];
            }
            else {
                in = Sinc_Diff[i - 1];
            }
            Sinc_Diff[i] = in - Sinc_Diff_Prev[i];
            Sinc_Diff_Prev[i] = in;
        }

        // A_norm = 1 / FOSR^K, формула (2). Считаем здесь, а не в main(),
        // чтобы фильтр не зависел от того, задали ли коэффициент снаружи
        Sinc_A_Norm = _IQ(1.0);
        for (i = 0; i < K; i++)
        {
            Sinc_A_Norm = Sinc_A_Norm / FOSR;
        }

        // нормировка выхода последнего дифференциатора и обратно в IQ (Q28)
        Sinc_Out = _IQmpy((long)Sinc_Diff[K - 1], Sinc_A_Norm) << (GLOBAL_Q - IN_BITS);
        reducer(&Sinc_Out);
        Sinc_Ready = 1;
    }
}

#define AFC_N 128
#define AFC_SETTLE 8
#define AFC_MEAS 64

long Afc_Freq[AFC_N];  // f_j, Гц
_iq Afc_Meas[AFC_N];   // измеренная |H(f_j)|
_iq Afc_Theory[AFC_N]; // желаемая |H(f_j)|
int Afc_Run = 1;       // 1 - идёт измерение; чтобы повторить, поставить 1 в Watch Window
int Afc_Index = 0;     // номер текущей точки j
int Afc_Count = 0;     // выходных отсчётов на текущей точке
_iq Afc_Peak = 0;
_iq Afc_Delta_Saved;   // time_delta до начала измерения, восстанавливается после

// f_j / FS
static _iq afc_u(int j)
{
    return (2 * j + 1) * (_IQ(1.0) / (4 * AFC_N));
}

// желаемая |H| на частоте u = f / FS
static _iq afc_theory(_iq u)
{
    _iq b = _IQmpy(_IQ(PI), u); // pi*f/FS, от 0 до pi/2
    _iq a = 0;                  // pi*f*FOSR/FS = FOSR*b, держим в [-pi, pi)
    _iq r;
    _iq h = _IQ(1.0);
    int i;

    for (i = 0; i < FOSR; i++)
    {
        a = a + b;
        if (a >= _IQ(PI))
        {
            a = a - _IQ(2 * PI);
        }
    }

    r = _IQdiv(_IQsin(a) / FOSR, _IQsin(b));
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

    for (j = 0; j < AFC_N; j++)
    {
        Afc_Freq[j] = (2 * j + 1) / (time_delta * 4 * AFC_N); // (j + 0.5)*Fs/(2*AFC_N)
        Afc_Theory[j] = afc_theory(afc_u(j));
        Afc_Meas[j] = 0;
    }
}

// вызывается на каждом новом выходном отсчёте фильтра
static void afc_step(void)
{
    _iq y;

    if (Afc_Run == 0)
    {
        return;
    }

    // начало измерения: запоминаем частоту сигнала, ставим первую точку
    if (Afc_Index == 0 && Afc_Count == 0)
    {
        Afc_Delta_Saved = time_delta;
        time_delta = _IQmpy(_IQ(2 * PI), afc_u(0)); // переключаемся на рабочую частоту
    }

    Afc_Count = Afc_Count + 1;
    if (Afc_Count <= AFC_SETTLE)
    {
        return; // пропуск первых AFC_SETTLE отсчетов
    }

    y = Sinc_Out;
    if (y < 0)
    {
        y = -y;
    }
    if (y > Afc_Peak)
    {
        Afc_Peak = y;
    }

    if (Afc_Count == AFC_SETTLE + AFC_MEAS)
    {
        Afc_Meas[Afc_Index] = _IQdiv(Afc_Peak, _IQ(A1));
        Afc_Peak = 0;
        Afc_Count = 0;
        Afc_Index = Afc_Index + 1;

        if (Afc_Index == AFC_N)
        {
            // все точки пройдены: возвращаем исходную частоту сигнала
            Afc_Index = 0;
            Afc_Run = 0;
            time_delta = Afc_Delta_Saved;
        }
        else
        {
            // time_delta = 2*pi*f/FS - шаг фазы синуса за один отсчёт
            time_delta = _IQmpy(_IQ(2 * PI), afc_u(Afc_Index));
        }
    }
}

interrupt void timer0_isr(void)
{

    A1 = 1;

    gpio0_clear();

#if SIN_METHOD == SIN_METHOD_CHEBYSHEV
    SIN_Value = sin_chebyshev(time);
#else
    SIN_Value = sin_cordic(time);
#endif
    SIN_Value = _IQmpy(SIN_Value, _IQ(A1));

    reducer(&SIN_Value);

    sinc_filter(SIN_Value);

    gpio0_set();

    DAC_Value = _IQtoQ11(SIN_Value);
    DAC_1_Write(DAC_Value);
    if (Sinc_Ready)
    {
        DAC_Sinc_Value = _IQtoQ11(Sinc_Out);
        DAC_0_Write(DAC_Sinc_Value);
        afc_step();
    }

    // time = (time + time_delta) mod 2*pi; сначала вычитаем 2*pi, чтобы не выйти за 8 (предел Q28)
    time = time - _IQ(2 * PI) + time_delta;
    if (time < 0)
    {
        time = time + _IQ(2 * PI);
    }

    // Timer0 Interrupt Flag Clear
    CpuTimer0Regs.TCR.bit.TIF = 1;
    // PIE Group1 Acknowledgement Reset
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
