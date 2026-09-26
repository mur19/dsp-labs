#include "ADC.h"     // DSP280x Headerfile Include File
#include "Peripheral_isr.h"

int ADC_DATA0;
int ADC_DATA1;
int ADC_DATA2;

long ADC_Offset_0 = 0;
long ADC_Offset_1 = 0;
long ADC_Offset_2 = 0;

long ADC_Koef_0 = 1;
long ADC_Koef_1 = 1;
long ADC_Koef_2 = 1;

unsigned long adc_isr_count = 0;


//---------------------------------------------------------------------------
// adc_init: 
//---------------------------------------------------------------------------
// This function initializes ADC to a known state.
void adc_init(ADC_CONFIG *adc_config) {
	AdcRegs.ADCTRL1.all = 0;
	AdcRegs.ADCTRL2.all = 0;
	AdcRegs.ADCTRL3.all = 0;

 	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1  Cascaded mode
 	AdcRegs.ADCTRL1.bit.SEQ_OVRD = 0;        // Sequencer override disable
 	AdcRegs.ADCTRL1.bit.CONT_RUN = 0;        // Start-stop mode
 	AdcRegs.ADCTRL1.bit.CPS = 0;       		// Core clock prescaler = 0 (frequency is not divided)
 	AdcRegs.ADCTRL1.bit.ACQ_PS = 6;			// Aquisition time = 7 ADCCLK

 	AdcRegs.ADCREFSEL.bit.REF_SEL = 1;		// External Reference 2.048V
  	AdcRegs.ADCMAXCONV.all = adc_config->conversion_num - 1; 

   	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = adc_config->conversion0_src;	// First conversion ADCINA4
  	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = adc_config->conversion1_src;	// Second conversion ADCINA5
   	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = adc_config->conversion2_src;	// 3-d conversion - ADCINA6
   	
   	AdcRegs.ADCTRL3.bit.SMODE_SEL = 0; 		// Sequential Sampling Mode
   	AdcRegs.ADCTRL3.bit.ADCCLKPS = 3;     	// ADCCLK = HSPCLK/6 (25 MHz)
   	AdcRegs.ADCTRL3.bit.ADCPWDN = 1;     	// ADC is powered up
   	AdcRegs.ADCTRL3.bit.ADCBGRFDN = 3;     // Reference is powered up

	asm("\t mov acc,#0x8 << 14\nadc_wait: sub acc,#1\n\t sb adc_wait,neq"); // -- задержка для иницализации АЦП

   	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1;				// Start ADC EPWM SOCA
	// ISR functions found within this file.  
   	EALLOW;  // This is needed to write to EALLOW protected registers
  	PieVectTable.ADCINT = &adc_isr;
   	EDIS;

   	IER |= 0x1; // -- Enable CPU INT1 which is connected to ADCINT:

   	PieCtrlRegs.PIEIER1.bit.INTx6 = 1; // -- Enable ADCINT in the PIE: Group 1 interrupt 6

	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 0;				// ADC INT Flag is set on every end of conversiobn sequence
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;	
}	

//===========================================================================
// End of file.
//===========================================================================
