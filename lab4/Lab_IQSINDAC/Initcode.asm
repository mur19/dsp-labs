;
; Init Code
; 
;
;	.sect ".esystem"
;	.align 	2
;Flash_Init_Buffer	.int	0

	.text

WDCR		.set 	0x7029

XCLK		.set 	0x7010
PLLSTS		.set 	0x7011
HISPCP		.set 	0x701a
LOSPCP		.set 	0x701b
PCLKCR0		.set 	0x701c
PCLKCR1		.set 	0x701d
PLLCR		.set 	0x7021

PLLLOCKS	.set	0
CLKINDIV	.set	1
MCLKSTS		.set	3
MCLKOFF		.set	6

FOPT		.set	0X0A80
FBANKWAIT	.set	0X0A86

	.def 	init_code_start
	.ref	_c_int00

init_code_start:	
			SETC	OBJMODE

			LCR		Watchdog_Disable

;			LCR		PLL_Config
;
;			MOVL	XAR0,#Flash_Init_Buffer
;			MOVL	XAR1,#Flash_Init
;			MOVL	XAR2,#Flash_Init_End+1
;			MOVL	ACC,@XAR2
;			SUBL	ACC,@XAR1
;			SETC	SXM
;Flash_Init_Loop:
;			MOVZ	AR2,*XAR1++
;			MOV		*XAR0++,AR2
;			ADD		ACC,#-1
;			SB		Flash_Init_Loop,NEQ
;
;			LCR		Flash_Init_Buffer
;
			LB		_c_int00

;------------------- Disable Watchdog Timer-----------------
Watchdog_Disable:

			EALLOW

			MOVZ 	DP,#WDCR>>6
			MOV		@WDCR,#0x68
			EDIS

			LRETR
;-----------------------------------------------------------
			.align 2
;------------------PLL Cofig--------------------------------
PLL_Config:
			EALLOW

; Test limp mode
			MOVZ 	DP,#PLLSTS>>6

			TBIT	@PLLSTS,#MCLKSTS
			SB		PLL_End,TC

; Test and clead CLKINDIV before new value writing in PLLCR
			TBIT	@PLLSTS,#CLKINDIV
			SB		PLL_DIV_Yes,NTC

			TCLR	@PLLSTS,#CLKINDIV

PLL_DIV_Yes:
; Disable fialed oscillator detect logic
			TSET	@PLLSTS,#MCLKOFF

; PLLCR = 5. SYSCLKOUT = OSCCLK*5 (100 MHz)
			MOVZ 	DP,#PLLCR>>6
			MOV		@PLLCR,#5

; Wait PLL to lock.
			MOVZ 	DP,#PLLSTS>>6

PLL_Lock_Wiat:
			TBIT	@PLLSTS,#PLLLOCKS
			SB		PLL_Lock_Wiat,NTC

; Enable fialed oscillator detect logic
			TCLR	@PLLSTS,#MCLKOFF

; Set CLKINDIV to disable frequency divider
			TSET	@PLLSTS,#CLKINDIV

; HSPCLK = SYSCLKOUT
			MOVZ 	DP,#HISPCP>>6
			MOV		@HISPCP,#0

; LSPCLK = SYSCLKOUT
			MOVZ 	DP,#LOSPCP>>6
			MOV		@LOSPCP,#0

; XCLKOUT - off
			MOVZ 	DP,#XCLK>>6
			MOV		@XCLK,#3

PLL_End:
			EDIS

			LRETR
;-----------------------------------------------------
			.align 2
;---------------Flash Config---------------------------
Flash_Init:
			EALLOW

			MOVZ 	DP,#FOPT>>6
			TSET	@FOPT,#0

			MOVZ 	DP,#FBANKWAIT>>6
			MOV		@FBANKWAIT,#0X0303

			RPT		#8
		||	NOP

			EDIS

			LRETR
Flash_Init_End:
;-----------------------------------------------------
