  // TI File $Revision: /main/16 $
// Checkin $Date: October 3, 2007   14:50:19 $
//###########################################################################
//
// FILE:	DSP2833x_McBSP.c
//
// TITLE:	DSP2833x Device McBSP Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x/DSP2823x Header Files V1.20 $
// $Release Date: August 1, 2008 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
//#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//---------------------------------------------------------------------------
// MCBSP_INIT_DELAY determines the amount of CPU cycles in the 2 sample rate
// generator (SRG) cycles required for the Mcbsp initialization routine.
// MCBSP_CLKG_DELAY determines the amount of CPU cycles in the 2 clock
// generator (CLKG) cycles required for the Mcbsp initialization routine.
// For the functions defined in Mcbsp.c, MCBSP_INIT_DELAY and MCBSP_CLKG_DELAY
// are based off of either a 150 MHz SYSCLKOUT (default) or a 100 MHz SYSCLKOUT.
//
// CPU_FRQ_100MHZ and CPU_FRQ_150MHZ are defined in DSP2833x_Examples.h
//---------------------------------------------------------------------------

//#if CPU_FRQ_150MHZ                                          // For 150 MHz SYSCLKOUT(default)
  #define CPU_SPD              150E6
  #define MCBSP_SRG_FREQ       CPU_SPD/8                    // SRG input is LSPCLK/4 (SYSCLKOUT/8) for examples
//#endif
//#if CPU_FRQ_100MHZ                                          // For 100 MHz SYSCLKOUT
//  #define CPU_SPD              100E6
//  #define MCBSP_SRG_FREQ       CPU_SPD/4                    // SRG input is LSPCLK (SYSCLKOUT/4) for examples
//#endif

#define CLKGDV_VAL           1
#define MCBSP_INIT_DELAY     2*(CPU_SPD/MCBSP_SRG_FREQ)                  // # of CPU cycles in 2 SRG cycles-init delay
#define MCBSP_CLKG_DELAY     2*(CPU_SPD/(MCBSP_SRG_FREQ/(1+CLKGDV_VAL))) // # of CPU cycles in 2 CLKG cycles-init delay
//---------------------------------------------------------------------------
// InitMcbsp:
//---------------------------------------------------------------------------
// This function initializes the McBSP to a known state.
//

void delay_loop(void);		// Delay function used for SRG initialization
void clkg_delay_loop(void); // Delay function used for CLKG initialization
void InitMcbspbGpio();


/*void InitMcbspa(void)
{
// McBSP-A register settings

    McbspaRegs.SPCR2.all=0x0000;		// Reset FS generator, sample rate generator & transmitter
	McbspaRegs.SPCR1.all=0x0000;		// Reset Receiver, Right justify word
	McbspaRegs.SPCR1.bit.DLB = 1;       // Enable loopback mode for test. Comment out for normal McBSP transfer mode.


	McbspaRegs.MFFINT.all=0x0;			// Disable all interrupts

    McbspaRegs.RCR2.all=0x0;			// Single-phase frame, 1 word/frame, No companding	(Receive)
    McbspaRegs.RCR1.all=0x0;

    McbspaRegs.XCR2.all=0x0;			// Single-phase frame, 1 word/frame, No companding	(Transmit)
    McbspaRegs.XCR1.all=0x0;

    McbspaRegs.PCR.bit.FSXM = 1;		// FSX generated internally, FSR derived from an external source
	McbspaRegs.PCR.bit.CLKXM = 1;		// CLKX generated internally, CLKR derived from an external source

    McbspaRegs.SRGR2.bit.CLKSM = 1;		// CLKSM=1 (If SCLKME=0, i/p clock to SRG is LSPCLK)
	McbspaRegs.SRGR2.bit.FPER = 31;		// FPER = 32 CLKG periods

    McbspaRegs.SRGR1.bit.FWID = 0;              // Frame Width = 1 CLKG period
    McbspaRegs.SRGR1.bit.CLKGDV = CLKGDV_VAL;	// CLKG frequency = LSPCLK/(CLKGDV+1)

    delay_loop();                // Wait at least 2 SRG clock cycles

    McbspaRegs.SPCR2.bit.GRST=1; // Enable the sample rate generator
	clkg_delay_loop();           // Wait at least 2 CLKG cycles
	McbspaRegs.SPCR2.bit.XRST=1; // Release TX from Reset
	McbspaRegs.SPCR1.bit.RRST=1; // Release RX from Reset
    McbspaRegs.SPCR2.bit.FRST=1; // Frame Sync Generator reset

}


void InitMcbspb(void)
{

	  InitMcbspbGpio();
// McBSP-B register settings

    McbspbRegs.SPCR2.all=0x0000;		// Reset FS generator, sample rate generator & transmitter
	McbspbRegs.SPCR1.all=0x0000;		// Reset Receiver, Right justify word
//	McbspbRegs.SPCR1.bit.DLB = 1;       // Enable loopback mode for test. Comment out for normal McBSP transfer mode.

	McbspbRegs.MFFINT.all=0x0;			// Disable all interrupts

	McbspbRegs.SPCR1.bit.CLKSTP=2;		// Clock Stop Mode without Clock Delay

    McbspbRegs.RCR2.all=0x0;			// Single-phase frame, 1 word/frame, No companding	(Receive)
    McbspbRegs.RCR1.all=0x0;

    McbspbRegs.XCR2.all=0x0;			// Single-phase frame, 1 word/frame, No companding	(Transmit)
    McbspbRegs.XCR1.all=0x0;

	McbspbRegs.PCR.bit.CLKXP = 0;		// Transmit data is sampled on the rising edge of CLKX
	McbspbRegs.PCR.bit.CLKRP = 0;		// Receive data is sampled on the falling edge of CLKX
	McbspbRegs.PCR.bit.CLKXM = 1;		// MCLKX generated internally, MCLKR is driven internally by CLKX
	McbspbRegs.PCR.bit.SCLKME = 0;		// The clock generated by the sample rate generator (CLKG) is derived from LSPCLK
    McbspbRegs.SRGR2.bit.CLKSM = 1;		// 

	McbspbRegs.SRGR2.bit.FPER = 1;		// FPER = 1 CLKG periods

	McbspbRegs.RCR1.bit.RWDLEN1=2;     // 16-bit word
    McbspbRegs.XCR1.bit.XWDLEN1=2;     // 16-bit word

    McbspbRegs.SRGR1.bit.FWID = 0;              // Frame Width = 1 CLKG period
    McbspbRegs.SRGR1.bit.CLKGDV = 3;	// CLKG frequency = LSPCLK/(CLKGDV+1) 

   	McbspbRegs.PCR.bit.FSXM = 1;		// FSX generated internally
	McbspbRegs.SRGR2.bit.FSGM = 1;		//The transmitter drives a frame-synchronization pulse on the FSX pin every time data is
										//transferred from DXR1 to XSR1
	McbspbRegs.PCR.bit.FSXP = 1;		//The FSX pin is active low.

	McbspbRegs.XCR2.bit.XDATDLY = 1;	//This setting provides the correct setup time on the FSX signal
	McbspbRegs.RCR2.bit.RDATDLY = 1;	//

    delay_loop();                // Wait at least 2 SRG clock cycles
    McbspbRegs.SPCR2.bit.GRST=1; // Enable the sample rate generator
	clkg_delay_loop();           // Wait at least 2 CLKG cycles
	McbspbRegs.SPCR2.bit.XRST=1; // Release TX from Reset
	McbspbRegs.SPCR1.bit.RRST=1; // Release RX from Reset
    McbspbRegs.SPCR2.bit.FRST=1; // Frame Sync Generator reset

//	McbspbRegs.DXR1.all=0xffff;
}
*/

void InitMcbspb_SPIMaster(void)
{

//	InitMcbspa_SPIMaster_Gpio();
	  InitMcbspbGpio();
// McBSP-A register settings

    McbspbRegs.SPCR2.all=0x0000;		// Reset FS generator, sample rate generator & transmitter
	McbspbRegs.SPCR1.all=0x0000;		// Reset Receiver, Right justify word

//	McbspbRegs.PCR.all=0x0a0c;           //CLKRP=0, CLKXP=0, FSRP=1, FSXP=1, SCLKME=0, CLKRM=0, CLKXM=1, FSRM=0, FSXM=1
	McbspbRegs.PCR.all=0x0a0f;           //CLKRP=1, CLKXP=1, FSRP=1, FSXP=1, SCLKME=0, CLKRM=0, CLKXM=1, FSRM=0, FSXM=1

    McbspbRegs.RCR2.bit.RDATDLY=1;      // FSX setup time is 0 in slave mode (Receive)
    McbspbRegs.RCR1.bit.RWDLEN1=2;     // 16-bit word
   
    McbspbRegs.XCR2.bit.XDATDLY=1;      // FSX setup time  is 0 for slave mode (Transmit)
	McbspbRegs.XCR1.bit.XWDLEN1=2;     // 16-bit word

//	McbspbRegs.SPCR1.bit.CLKSTP = 2;     //Clock Stop Mode without Clock Delay
	McbspbRegs.SPCR1.bit.CLKSTP = 3;     //Clock Stop Mode with Clock Delay

    McbspbRegs.SRGR2.bit.CLKSM = 1;		// CLKSM=1 (If SCLKME=0, i/p clock to SRG is LSPCLK)
	McbspbRegs.SRGR2.bit.FPER = 1;		// FPER = 1 CLKG period
	McbspbRegs.SRGR2.bit.FSGM = 0;		//Transmit Frame Sincronization is generated when DXR is copied ro XSR

    McbspbRegs.SRGR1.bit.FWID = 0;              // Frame Width = 1 CLKG period
    McbspbRegs.SRGR1.bit.CLKGDV = 3;	// CLKG frequency = LSPCLK/(CLKGDV+1)

    delay_loop();                // Wait at least 2 SRG clock cycles

    McbspbRegs.SPCR2.bit.GRST=1; // Enable the sample rate generator
	clkg_delay_loop();           // Wait at least 2 CLKG cycles
	McbspbRegs.SPCR2.bit.XRST=1; // Release TX from Reset
	McbspbRegs.SPCR1.bit.RRST=1; // Release RX from Reset
    McbspbRegs.SPCR2.bit.FRST=1; // Frame Sync Generator reset

 	McbspbRegs.DXR1.all=0xffff;

	McbspbRegs.MFFINT.all=0x0;			// Disable all interrupts

}

void InitMcbspaGpio(void)
{
	EALLOW;

/* Configure McBSP-A pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be McBSP functional pins.
// Comment out other unwanted lines.

	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 2;	// GPIO20 is MDXA pin
	GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 2;	// GPIO21 is MDRA pin
    GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 2;	// GPIO22 is MCLKXA pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 2;		// GPIO7 is MCLKRA pin (Comment as needed)
	//GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 1;	// GPIO58 is MCLKRA pin (Comment as needed)
    GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 2;	// GPIO23 is MFSXA pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 2;		// GPIO5 is MFSRA pin (Comment as needed)
	//GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 1;	// GPIO59 is MFSRA pin (Comment as needed)

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

	GpioCtrlRegs.GPAPUD.bit.GPIO20 = 0;     // Enable pull-up on GPIO20 (MDXA)
	GpioCtrlRegs.GPAPUD.bit.GPIO21 = 0;     // Enable pull-up on GPIO21 (MDRA)
	GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;     // Enable pull-up on GPIO22 (MCLKXA)
	GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;      // Enable pull-up on GPIO7 (MCLKRA) (Comment as needed)
	//GpioCtrlRegs.GPBPUD.bit.GPIO58 = 0;   // Enable pull-up on GPIO58 (MCLKRA) (Comment as needed)
	GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;     // Enable pull-up on GPIO23 (MFSXA)
	GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;      // Enable pull-up on GPIO5 (MFSRA) (Comment as needed)
	//GpioCtrlRegs.GPBPUD.bit.GPIO59 = 0;   // Enable pull-up on GPIO59 (MFSRA) (Comment as needed)

/* Set qualification for selected input pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.


    GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = 3;   // Asynch input GPIO21 (MDRA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO22 = 3;   // Asynch input GPIO22 (MCLKXA)
    GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 3;    // Asynch input GPIO7 (MCLKRA) (Comment as needed)
    //GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 3; // Asynch input GPIO58(MCLKRA) (Comment as needed)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3;   // Asynch input GPIO23 (MFSXA)
    GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 3;    // Asynch input GPIO5 (MFSRA) (Comment as needed)
    //GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 3; // Asynch input GPIO59 (MFSRA) (Comment as needed)

	EDIS;
}


void InitMcbspbGpio(void)
{
    EALLOW;
/* Configure McBSP-A pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be McBSP functional pins.
// Comment out other unwanted lines.

	//GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 3;	// GPIO12 is MDXB pin (Comment as needed)
	GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 3;	// GPIO24 is MDXB pin (Comment as needed)
	//GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 3;	// GPIO13 is MDRB pin (Comment as needed)
//	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 3;	// GPIO25 is MDRB pin (Comment as needed)
	//GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 3;	// GPIO14 is MCLKXB pin (Comment as needed)
	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 3;	// GPIO26 is MCLKXB pin (Comment as needed)
//	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 3;		// GPIO3 is MCLKRB pin (Comment as needed)
	//GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 1;	// GPIO60 is MCLKRB pin (Comment as needed)
	//GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 3;	// GPIO15 is MFSXB pin (Comment as needed)
	GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 3;	// GPIO27 is MFSXB pin (Comment as needed)
//	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 3;		// GPIO1 is MFSRB pin (Comment as needed)
	//GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 1;	// GPIO61 is MFSRB pin (Comment as needed)

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.
	GpioCtrlRegs.GPAPUD.bit.GPIO24 = 0;	    // Enable pull-up on GPIO24 (MDXB) (Comment as needed)
	//GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;	// Enable pull-up on GPIO12 (MDXB) (Comment as needed)
//	GpioCtrlRegs.GPAPUD.bit.GPIO25 = 0;	    // Enable pull-up on GPIO25 (MDRB) (Comment as needed)
	//GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;	// Enable pull-up on GPIO13 (MDRB) (Comment as needed)
	GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;	    // Enable pull-up on GPIO26 (MCLKXB) (Comment as needed)
	//GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;	// Enable pull-up on GPIO14 (MCLKXB) (Comment as needed)
//	GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;		// Enable pull-up on GPIO3 (MCLKRB) (Comment as needed)
	//GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;	// Enable pull-up on GPIO60 (MCLKRB) (Comment as needed)
	GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;	    // Enable pull-up on GPIO27 (MFSXB) (Comment as needed)
	//GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;	// Enable pull-up on GPIO15 (MFSXB) (Comment as needed)
//	GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;		// Enable pull-up on GPIO1 (MFSRB) (Comment as needed)
	//GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;	// Enable pull-up on GPIO61 (MFSRB) (Comment as needed)


/* Set qualification for selected input pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.
//     GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 3;   // Asynch input GPIO25 (MDRB) (Comment as needed)
    //GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 3; // Asynch input GPIO13 (MDRB) (Comment as needed)
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 3;   // Asynch input GPIO26(MCLKXB) (Comment as needed)
    //GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 3; // Asynch input GPIO14 (MCLKXB) (Comment as needed)
//    GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 3;    // Asynch input GPIO3 (MCLKRB) (Comment as needed)
    //GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 3; // Asynch input GPIO60 (MCLKRB) (Comment as needed)
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO27 = 3;   // Asynch input GPIO27 (MFSXB) (Comment as needed)
	//GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3; // Asynch input GPIO15 (MFSXB) (Comment as needed)
//	GpioCtrlRegs.GPAQSEL1.bit.GPIO1 = 3;    // Asynch input GPIO1 (MFSRB) (Comment as needed)
	//GpioCtrlRegs.GPBQSEL2.bit.GPIO61 = 3; // Asynch input GPIO61 (MFSRB) (Comment as needed)


	EDIS;
}


void delay_loop(void)
{
    long      i;
    for (i = 0; i < MCBSP_INIT_DELAY; i++) {} //delay in McBsp init. must be at least 2 SRG cycles
}

void clkg_delay_loop(void)
{
    long      i;
    for (i = 0; i < MCBSP_CLKG_DELAY; i++) {} //delay in McBsp init. must be at least 2 SRG cycles
}
//===========================================================================
// No more.
//===========================================================================











