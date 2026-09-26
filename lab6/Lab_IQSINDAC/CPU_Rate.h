#ifndef CPU_RATE_H
#define CPU_RATE_H

void DSP28x_usDelay(Uint32 Count);

#define CPU_RATE    6.667L   // for a 150MHz CPU clock speed (SYSCLKOUT)
//#define CPU_RATE    6.944L   // for a 144MHz CPU clock speed (SYSCLKOUT)

// DO NOT MODIFY THIS LINE.
#define DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)

#define CPU_FREQ 150000000L



#endif
