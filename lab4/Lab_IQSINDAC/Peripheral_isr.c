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
int FOSR = 16;

// unsigned нужен чтобы грамотно реализовать логику переполнения интеграторов
// только в unsigned long разрешается переполнение
// Звенья считаются с конца (от последнего к первому), как регистры в железе:
// там все звенья обновляются одновременно по одному такту, и каждое берёт
// значение предыдущего звена, записанное на прошлом такте. Если считать
// с конца, предыдущее звено ещё не обновлено, и в нём как раз старое значение.
unsigned long Sinc_Integr[K];    // выходы интеграторов (регистры)
unsigned long Sinc_Diff[K];      // выходы дифференциаторов (регистры)
unsigned long Sinc_Diff_Prev[K]; // прошлый вход каждого дифференциатора (z^-1)
int Sinc_Count = 0;

_iq Sinc_A_Norm;
int Sinc_DW;
_iq Sinc_Out;
int Sinc_Ready;
int DAC_Sinc_Value;

void sinc_filter(_iq x)
{
	unsigned long v;
	unsigned long in;
	int i;

	Sinc_Ready = 0;

	// IQ (Q28) -> входной код с IN_BITS битами после запятой
	v = x >> (GLOBAL_Q - IN_BITS);

	// интеграторы с конца: I[i] = I[i] + I[i-1] (старое), I[0] = I[0] + x
	for (i = K - 1; i > 0; i--)
	{
		Sinc_Integr[i] = Sinc_Integr[i] + Sinc_Integr[i - 1];
	}
	Sinc_Integr[0] = Sinc_Integr[0] + v;

	Sinc_Count = Sinc_Count + 1;
	if (Sinc_Count == FOSR)
	{
		Sinc_Count = 0;

		// дифференциаторы с конца: D[i] = вход - прошлый вход,
		// вход D[i] - старое D[i-1], вход D[0] - последний интегратор
		for (i = K - 1; i >= 0; i--)
		{
			if (i == 0)
			{
				in = Sinc_Integr[K - 1];
			}
			else
			{
				in = Sinc_Diff[i - 1];
			}
			Sinc_Diff[i] = in - Sinc_Diff_Prev[i];
			Sinc_Diff_Prev[i] = in;
		}

		// нормировка и обратно в IQ (Q28)
		Sinc_Out = _IQmpy((long)Sinc_Diff[K - 1], Sinc_A_Norm) << (GLOBAL_Q - IN_BITS);
		Sinc_Ready = 1;
	}
}

// ---------------- АЧХ ----------------
// Частота входного синуса перебирается по AFC_N точкам
//   f_j = (j + 0.5) * FS / (2 * AFC_N), j = 0..AFC_N-1  (от ~0 до FS/2)
// На каждой точке:
//   - первые AFC_SETTLE выходных отсчётов пропускаются (переходный процесс)
//   - по следующим AFC_MEAS отсчётам ищется максимум |Sinc_Out|
//   - |H(f_j)| = максимум / A1 -> Afc_Meas[j]
// Сдвиг на полшага (j + 0.5) нужен, чтобы прореженные выходные отсчёты попадали
// на разные фазы синуса и максимум был близок к настоящей амплитуде.
// Желаемая АЧХ - формула (1) при z = exp(i*2*pi*f/FS):
//   |H(f)| = |sin(pi*f*FOSR/FS) / (FOSR*sin(pi*f/FS))|^K  -> Afc_Theory[j]
// Оба массива можно построить в CCS: Tools -> Graph -> Single Time (Q28, AFC_N точек).
//
// Чтобы увидеть АЧХ прямо на осциллографе: после окончания измерения оба ЦАП
// сами переключаются на вывод готовой кривой (Afc_Plot становится 1) -
// DAC_0 = Afc_Meas (измеренная), DAC_1 = Afc_Theory (желаемая), по кругу,
// каждая точка держится AFC_PLOT_HOLD отсчётов. Каналы CH1(DAC_0)/CH2(DAC_1),
// режим Y-T, развёртка одна и та же каждый проход - картинка стоит на месте.
// Длительность одного прохода = AFC_N*AFC_PLOT_HOLD/FS секунд, весь проход -
// это частоты от ~0 до FS/2 Гц, по горизонтали линейно. Триггер - по фронту
// любого из каналов (в конце прохода сигнал резко прыгает от ~0 к ~1).
// Вернуться к обычному синусу на ЦАП: поставить Afc_Plot = 0 в Watch Window
// (или запустить новое измерение - Afc_Run = 1 - оно само выключит Afc_Plot).

#define AFC_N 128
#define AFC_SETTLE 8
#define AFC_MEAS 64
#define AFC_PLOT_HOLD 8 // отсчётов на одну точку при выводе кривой на ЦАП

long Afc_Freq[AFC_N];  // f_j, Гц
_iq Afc_Meas[AFC_N];   // измеренная |H(f_j)|
_iq Afc_Theory[AFC_N]; // желаемая |H(f_j)|
int Afc_Run = 1;       // 1 - идёт измерение; чтобы повторить, поставить 1 в Watch Window
int Afc_Index = 0;     // номер текущей точки j
int Afc_Count = 0;     // выходных отсчётов на текущей точке
_iq Afc_Peak = 0;
_iq Afc_Delta_Saved; // Time_Delta до начала измерения, восстанавливается после

int Afc_Plot = 0;        // 1 - ЦАП выводит готовую кривую по кругу (см. выше)
int Afc_Plot_Index = 0;  // номер точки, которая сейчас на ЦАП
int Afc_Plot_Count = 0;  // сколько отсчётов эта точка уже держится

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
		Afc_Freq[j] = (2 * j + 1) * FS / (4 * AFC_N); // (j + 0.5)*Fs/(2*AFC_N)
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
		Afc_Plot = 0; // новое измерение - ЦАП обратно на обычный синус
		Afc_Delta_Saved = Time_Delta;
		Time_Delta = _IQmpy(_IQ(2 * PI), afc_u(0)); // переключаемся на рабочую частоту
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
			// все точки пройдены: возвращаем исходную частоту сигнала,
			// ЦАП переключаются на вывод готовой кривой
			Afc_Index = 0;
			Afc_Run = 0;
			Time_Delta = Afc_Delta_Saved;
			Afc_Plot = 1;
			Afc_Plot_Index = 0;
			Afc_Plot_Count = 0;
		}
		else
		{
			// Time_Delta = 2*pi*f/FS - шаг фазы синуса за один отсчёт
			Time_Delta = _IQmpy(_IQ(2 * PI), afc_u(Afc_Index));
		}
	}
}

// выводит Afc_Meas/Afc_Theory на ЦАП по кругу; вызывается вместо обычной
// записи SIN_Value/Sinc_Out, пока Afc_Plot == 1
static void afc_plot_step(void)
{
	DAC_1_Write(_IQtoQ11(Afc_Meas[Afc_Plot_Index]));
	DAC_0_Write(_IQtoQ11(Afc_Theory[Afc_Plot_Index]));

	Afc_Plot_Count = Afc_Plot_Count + 1;
	if (Afc_Plot_Count == AFC_PLOT_HOLD)
	{
		Afc_Plot_Count = 0;
		Afc_Plot_Index = Afc_Plot_Index + 1;
		if (Afc_Plot_Index == AFC_N)
		{
			Afc_Plot_Index = 0;
		}
	}
}

// вызывается один раз из main() до разрешения прерываний
void sinc_init(void)
{
	int fosr_bits = 0;
	int i;

	// A_norm = 1 / FOSR^K, формула (2)
	Sinc_A_Norm = _IQ(1.0);
	for (i = 0; i < K; i++)
	{
		Sinc_A_Norm = Sinc_A_Norm / FOSR;
	}

	// DW = 1 + log2(D_IN) + K*log2(FOSR), формула (4); log2(FOSR) округляем вверх
	while ((1L << fosr_bits) < FOSR)
	{
		fosr_bits = fosr_bits + 1;
	}
	Sinc_DW = 1 + IN_BITS + K * fosr_bits;

	// DW должно помещаться в unsigned long (32 бита), иначе выход фильтра переполнится.
	// Если программа остановилась здесь - уменьшить K, FOSR или IN_BITS (см. Sinc_DW)
	if (Sinc_DW > 32)
	{
		while (1)
		{
		}
	}

	afc_init();
}

interrupt void timer0_isr(void)
{

	gpio0_clear();

	if (Afc_Plot)
	{
		afc_plot_step();
		gpio0_set();
		goto timer0_isr_end; // ЦАП заняты кривой АЧХ - обычный синус не считаем
	}

	A1 = 1;

#if SIN_METHOD == SIN_METHOD_CHEBYSHEV
	SIN_Value = sin_chebyshev(Time);
#else
	SIN_Value = sin_cordic(Time);
#endif
	SIN_Value = _IQmpy(SIN_Value, _IQ(A1));

	if (SIN_Value > _IQ(0.999))
		SIN_Value = _IQ(0.999);
	if (SIN_Value < _IQ(-0.999))
		SIN_Value = _IQ(-0.999);

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

	// Time = (Time + Time_Delta) mod 2*pi. Сначала вычитаем 2*pi: Time + Time_Delta
	// может быть больше 8 (предел Q28), если Time_Delta близко к pi (f близко к FS/2)
	Time = Time - _IQ(2 * PI) + Time_Delta;
	if (Time < 0)
	{
		Time = Time + _IQ(2 * PI);
	}

timer0_isr_end:

	// Timer0 Interrupt Flag Clear
	CpuTimer0Regs.TCR.bit.TIF = 1;
	// PIE Group1 Acknowledgement Reset
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
