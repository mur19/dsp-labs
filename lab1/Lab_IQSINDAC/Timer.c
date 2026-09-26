#include "Timer.h"
#include "Peripheral_isr.h"

void timer0_init(TIMER_CONFIG *timer_config) {
	StopCpuTimer0();
	ReadCpuTimer0Period()= timer_config->period_in_cpu_cycles;
	CpuTimer0Regs.TCR.bit.FREE = 1;
	CpuTimer0Regs.TCR.bit.SOFT = 1;
	CpuTimer0Regs.TCR.bit.TIE = 1;//enable int
	CpuTimer0Regs.TCR.bit.TIF = 1;	 
	CpuTimer0Regs.TPR.bit.TDDR = timer_config->prescale & 0xFF;	
	CpuTimer0Regs.TPRH.bit.TDDRH = (timer_config->prescale >> 8) & 0xFF;

	EALLOW;
	PieVectTable.TINT0 = &timer0_isr;	
	IER |= 1;
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	EDIS;
	StartCpuTimer0();
}
