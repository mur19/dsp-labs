#ifndef MCBSP_H
#define MCBSP_H

#include "DSP2833x_Device.h"

#define CPU_SPD              150E6
#define MCBSP_SRG_FREQ       CPU_SPD/8                    // SRG input is LSPCLK/4 (SYSCLKOUT/8) for examples

#define CLKGDV_VAL           1
#define MCBSP_INIT_DELAY     2*(CPU_SPD/MCBSP_SRG_FREQ)                  // # of CPU cycles in 2 SRG cycles-init delay
#define MCBSP_CLKG_DELAY     2*(CPU_SPD/(MCBSP_SRG_FREQ/(1+CLKGDV_VAL))) // # of CPU cycles in 2 CLKG cycles-init delay

void mcbspb_init(void);
void mcbspb_write(int value);
unsigned int mcbspb_read(void);

#endif

