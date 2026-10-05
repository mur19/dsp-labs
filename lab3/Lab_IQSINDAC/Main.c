#include "DSP2833x_Device.h"
#include "IQmathLib.h"
#include "CPU_Rate.h"
#include "Main.h"
#include "PWM.h"
// #include "ADC.h"
#include "GPIO.h"
#include "Timer.h"
// #include "QEP.h"
// #include "CAN.h"
// #include "MCBSP.h"

void InitMcbspb_SPIMaster(void);
void DAC_0_Write(int Value);
void DAC_1_Write(int Value);

#define PI 3.14159265

unsigned long Buffer[2];
unsigned int Null_Value;

// Define GlabalQ for IQmath.gel
long GlobalQ = GLOBAL_Q;
_iq time_delta, time;

void main(void)
{
	TIMER_CONFIG timer_config;
	//	QEP_CONFIG qep_config;
	//	PWM_CONFIG pwm_config;
	//	ADC_CONFIG adc_config;

	// -- ���������� WatchDog �������
	disable_watchdog();
	// -- ���������� ���������� ����������
	disable_interrupts();
	// -- ����� ��������� ��������� �����������
	system_control_init();

	// -- ������������� ������ �����-������
	gpio_init();

	// -- ������������� ��������� ��� ��������� �������
	timer_config.period_in_cpu_cycles = 49L;
	timer_config.prescale = 149;
	// -- ������������� ������� � ������
	timer0_init(&timer_config);

	// -- ���������� ��������� ��� ��������� PWM
	//	pwm_config.period_in_cpu_cycles = TBPRDVALUE;
	//	pwm_config.count_mode = PWM_COUNT_UP_DOWN;
	//	pwm_config.tristate_enable = PWM_TRISTATE_DISABLE;
	//	pwm_config.deadband = DBREDVALUE;
	// -- ������������� PWM
	//	pwm_init(&pwm_config);

	// -- ���������� ��������� ��� ��������� ���
	//	adc_config.conversion_num = 3;
	//	adc_config.conversion0_src = ADCINA0;
	//	adc_config.conversion1_src = ADCINA1;
	//	adc_config.conversion2_src = ADCINA2;
	// -- ������������� ���
	//	adc_init(&adc_config);

	// -- ���������� ��������� ��� ��������� QEP-1
	//	qep_config.reset_mode = QEP_RESET_ON_MAXIMUM;
	//	qep_config.resolution_in_discretes = 1999;
	//	qep_config.value_to_compare = 0x500;
	// -- ������������ QEP-1
	//	qep1_init(&qep_config);

	// -- ������������� CAN-A
	//	cana_init();

	// -- ������������� MCPSB-B
	//	mcbspb_init();

	// Initialize SPI for DAC Interface.
	InitMcbspb_SPIMaster();

	// Initialize DAC/
	McbspbRegs.DXR1.all = 0xd002; // DAC in fast mode, normal operation, internar vref = 2.048V
	while (McbspbRegs.SPCR1.bit.RRDY == 0)
	{
	}
	Null_Value = McbspbRegs.DRR1.all;

	time_delta = _IQ(0.00824 * PI);

	time = _IQ(0.);

	EINT;

	enable_interrupts();

	forever_loop();
}

//------------------------------------------------------------------------
// ��������� ������������� ������ �����-������
void gpio_init(void)
{
	// -- LED pins initialization
	gpio0_init(GPIO_OUT, GPIO_PIN, GPIO_ASYNC_MODE, GPIO_PULLUP_ON);
	//	gpio1_init(GPIO_OUT, GPIO_PIN, GPIO_ASYNC_MODE, GPIO_PULLUP_ON);

	// -- PWM pins initialization
	//	gpio0_init(GPIO_IN, GPIO_PWM, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio1_init(GPIO_IN, GPIO_PWM, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio2_init(GPIO_IN, GPIO_PWM, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio3_init(GPIO_IN, GPIO_PWM, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio4_init(GPIO_IN, GPIO_PWM, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio5_init(GPIO_IN, GPIO_PWM, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio6_init(GPIO_IN, GPIO_PWM, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio7_init(GPIO_IN, GPIO_PWM, GPIO_SYNC_MODE, GPIO_PULLUP_ON);

	// -- QEP1 pins initialization
	//	gpio20_init(GPIO_IN, GPIO_QEP, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio21_init(GPIO_IN, GPIO_QEP, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio23_init(GPIO_IN, GPIO_QEP, GPIO_SYNC_MODE, GPIO_PULLUP_ON);

	// -- CAN pin initialization
	//	gpio30_init(GPIO_IN, GPIO_CAN, GPIO_ASYNC_MODE, GPIO_PULLUP_ON);
	//	gpio31_init(GPIO_IN, GPIO_CAN, GPIO_SYNC_MODE, GPIO_PULLUP_ON);

	// -- MCBSP-b pin initialization
	//	gpio24_init(GPIO_IN, GPIO_MCBSPB, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio25_init(GPIO_IN, GPIO_MCBSPB, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio26_init(GPIO_IN, GPIO_MCBSPB, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
	//	gpio27_init(GPIO_IN, GPIO_MCBSPB, GPIO_SYNC_MODE, GPIO_PULLUP_ON);
}

void forever_loop(void)
{
	// int i;
	while (1)
	{
		// insert code here
		//		if (pwm_isr_count >= 10) {
		//			DINT;
		//			pwm_isr_count = 0;
		//			EINT;
		//		}
	}
}

// ���������� ��������� ����������
void enable_interrupts(void)
{
	EINT;
}

// ���������� ���������� ����������
void disable_interrupts(void)
{
	DINT;
}

void DAC_0_Write(int Value)
{
	int DAC_Value;
	DAC_Value = (Value & 0xfff) ^ 0x800;
	McbspbRegs.DXR1.all = DAC_Value | 0x4000;
	// Transfer End Wait.
	while (McbspbRegs.SPCR1.bit.RRDY == 0)
	{
	}
	// Transfer End Flag Clear.
	DAC_Value = McbspbRegs.DRR1.all;
}

void DAC_1_Write(int Value)
{
	int DAC_Value;
	DAC_Value = (Value & 0xfff) ^ 0x800;
	McbspbRegs.DXR1.all = DAC_Value | 0xc000;
	// Transfer End Wait.
	while (McbspbRegs.SPCR1.bit.RRDY == 0)
	{
	}
	// Transfer End Flag Clear.
	DAC_Value = McbspbRegs.DRR1.all;
}
