#include "DSP2833x_Device.h"     // DSP280x Headerfile Include File
#include "PWM.h"     // DSP280x Headerfile Include File
#include "GPIO.h"
#include "Peripheral_isr.h"

unsigned long pwm_period_in_cpu_cycles_global = 0;
unsigned int pwm_isr_count = 0;

void pwm1_init(PWM_CONFIG *pwm_config);
void pwm2_init(PWM_CONFIG *pwm_config);
void pwm3_init(PWM_CONFIG *pwm_config);
interrupt void pwm_isr(void);

void pwm_init(PWM_CONFIG *pwm_config) {
	DINT;   
	EALLOW;  // This is needed to write to EALLOW protected registers
	PieVectTable.EPWM1_INT = &pwm_isr;
	EDIS;    // This is needed to disable write to EALLOW protected registers

	pwm1_init(pwm_config);    
//	pwm2_init(pwm_config);
//	pwm3_init(pwm_config);
//	pwm_set_zero_voltage(pwm_config->period_in_cpu_cycles);

	IER |= 4;
	pwm_period_in_cpu_cycles_global = pwm_config->period_in_cpu_cycles;
	// Enable EPWM INTn in the PIE: Group 3 interrupt 1
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
} 

void pwm1_init(PWM_CONFIG *pwm_config) {
   // Setup TBCLK
   EPwm1Regs.TBPRD = pwm_config->period_in_cpu_cycles;           			  // Set timer period
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm1Regs.TBCTR = 0x0000;                      // Clear counter
 
   // Set Compare values
   EPwm1Regs.CMPA.half.CMPA = pwm_config->period_in_cpu_cycles/2;     		// Set compare A value
   EPwm1Regs.CMPB = pwm_config->period_in_cpu_cycles/2;               		// Set Compare B value

  // Setup counter mode
   EPwm1Regs.TBCTL.bit.CTRMODE = pwm_config->count_mode; 		// Count up and down
   EPwm1Regs.TBCTL.bit.PHSEN = 0;        	// Disable phase loading
   EPwm1Regs.TBCTL.bit.PRDLD = 0;        	// Period Register is loaded from its shadow
   EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;        // SYNCO - CTR = ZERO
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = HSPCLKDIVVALUE;       // Clock ratio to SYSCLKOUT
   EPwm1Regs.TBCTL.bit.CLKDIV = CLKDIVVALUE;
   EPwm1Regs.TBCTL.bit.PHSDIR = 1;        	// Count up after sync event
   EPwm1Regs.TBCTL.bit.FREE_SOFT = 2;       // Free runing during emulation

   // Setup shadowing
   EPwm1Regs.CMPCTL.bit.SHDWAMODE = 0;
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = 0;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = 2;  // Load on Zero or Period
   EPwm1Regs.CMPCTL.bit.LOADBMODE = 2;   

   // Set actions
   /*-- для отключения стойки 
   EPwm1Regs.AQCTLA.bit.ZRO = 2;             	  // Nothing at CNTR =0
   EPwm1Regs.AQCTLA.bit.PRD = 2;             	  // Nothing at CNTR =PRD
   EPwm1Regs.AQCTLA.bit.CAU = 2;             	  // Set PWM1A on event A, up count
   EPwm1Regs.AQCTLA.bit.CAD = 2;           		  // Clear PWM1A on event A, down count
   EPwm1Regs.AQCTLA.bit.CBU = 2;             	  // Nothing on event B, up count
   EPwm1Regs.AQCTLA.bit.CBD = 2;           	   	  // Nothingon event B, down count

   EPwm1Regs.AQCTLB.bit.ZRO = 2;             	  // Nothing at CNTR =0
   EPwm1Regs.AQCTLB.bit.PRD = 2;             	  // Nothing at CNTR =PRD
   EPwm1Regs.AQCTLB.bit.CAU = 2;             	  // cLEAR PWM1B on event A, up count
   EPwm1Regs.AQCTLB.bit.CAD = 2;           		  // sET PWM1B on event A, down count
   EPwm1Regs.AQCTLB.bit.CBU = 2;             	  // Nothing on event B, up count
   EPwm1Regs.AQCTLB.bit.CBD = 2;           	   	  // Nothingon event B, down count

   EPwm1Regs.DBCTL.bit.IN_MODE = 0;           	  // EPWMA is source for both delays
   EPwm1Regs.DBCTL.bit.POLSEL = 2;           	  // EPWMB is inverted
   EPwm1Regs.DBCTL.bit.OUT_MODE = 0;           	  // Deadband for rising adge EPWMA 
   */
   EPwm1Regs.AQCTLA.bit.ZRO = 0;             	  // Nothing at CNTR =0
   EPwm1Regs.AQCTLA.bit.PRD = 0;             	  // Nothing at CNTR =PRD
   EPwm1Regs.AQCTLA.bit.CAU = 2;             	  // Set PWM1A on event A, up count
   EPwm1Regs.AQCTLA.bit.CAD = 1;           		  // Clear PWM1A on event A, down count
   EPwm1Regs.AQCTLA.bit.CBU = 0;             	  // Nothing on event B, up count
   EPwm1Regs.AQCTLA.bit.CBD = 0;           	   	  // Nothingon event B, down count

   EPwm1Regs.AQCTLB.bit.ZRO = 0;             	  // Nothing at CNTR =0
   EPwm1Regs.AQCTLB.bit.PRD = 0;             	  // Nothing at CNTR =PRD
   EPwm1Regs.AQCTLB.bit.CAU = 1;             	  // cLEAR PWM1B on event A, up count
   EPwm1Regs.AQCTLB.bit.CAD = 2;           		  // sET PWM1B on event A, down count
   EPwm1Regs.AQCTLB.bit.CBU = 0;             	  // Nothing on event B, up count
   EPwm1Regs.AQCTLB.bit.CBD = 0;           	   	  // Nothingon event B, down count

   EPwm1Regs.DBCTL.bit.IN_MODE = 0;           	  // EPWMA is source for both delays
   EPwm1Regs.DBCTL.bit.POLSEL = 2;           	  // EPWMB is inverted
   EPwm1Regs.DBCTL.bit.OUT_MODE = 3;           	  // Deadband for rising adge EPWMA 
   												  // and falling edge EPWMB
   EPwm1Regs.DBRED = pwm_config->deadband; 		
   EPwm1Regs.DBFED = pwm_config->deadband; 			

   EPwm1Regs.ETPS.bit.INTPRD = 1;            	   // Generate Interrupt on the first event 
   EPwm1Regs.ETPS.bit.SOCAPRD = 1;            	   // Start ADC Sequence on the first event

   EPwm1Regs.ETSEL.bit.INTSEL = 1;      		   // Select INT on Zero event
   EPwm1Regs.ETSEL.bit.INTEN = 1;                 // Enable INT
   EPwm1Regs.ETSEL.bit.SOCASEL = 2;      		   // Start ADC on Period event
   EPwm1Regs.ETSEL.bit.SOCAEN = 1;      		   // Enable ADC Start

	if (pwm_config->tristate_enable == PWM_TRISTATE_ENABLE) {
	   	EALLOW;
		EPwm1Regs.TZSEL.bit.OSHT1 = 1;	// TZ1 - one shot event source
		EPwm1Regs.TZCTL.bit.TZA = 0;		// EPWMxA - to high impedance on TZ1	
		EPwm1Regs.TZCTL.bit.TZB = 0;		// EPWMxB - to high impedance on TZ1

		EPwm1Regs.TZFRC.bit.OST = 1;	// force trip event
		EDIS;
	}
}
void pwm2_init(PWM_CONFIG *pwm_config) {
   // Setup TBCLK
   EPwm2Regs.TBPRD = pwm_config->period_in_cpu_cycles;           			  // Set timer period
   EPwm2Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm2Regs.TBCTR = 0x0000;                      // Clear counter
 
   // Set Compare values
   EPwm2Regs.CMPA.half.CMPA = pwm_config->period_in_cpu_cycles/2;     		// Set compare A value
   EPwm2Regs.CMPB = pwm_config->period_in_cpu_cycles/2;               		// Set Compare B value
//   EPwm2Regs.CMPA.half.CMPA = 3040;     		// Set compare A value
//   EPwm2Regs.CMPB = TBPRDVALUE/2;               		// Set Compare B value


  // Setup counter mode
   EPwm2Regs.TBCTL.bit.CTRMODE = pwm_config->count_mode; 		// Count up and down
   EPwm2Regs.TBCTL.bit.PHSEN = 1;        	// Enable phase loading
   EPwm2Regs.TBCTL.bit.PRDLD = 0;        	// Period Register is loaded from its shadow
   EPwm2Regs.TBCTL.bit.SYNCOSEL = 0;        // SYNCO - SYNCI
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = HSPCLKDIVVALUE;       // Clock ratio to SYSCLKOUT
   EPwm2Regs.TBCTL.bit.CLKDIV = CLKDIVVALUE;
   EPwm2Regs.TBCTL.bit.PHSDIR = 1;        	// Count up after sync event
   EPwm2Regs.TBCTL.bit.FREE_SOFT = 2;       // Free runing during emulation

   // Setup shadowing
   EPwm2Regs.CMPCTL.bit.SHDWAMODE = 0;
   EPwm2Regs.CMPCTL.bit.SHDWBMODE = 0;
   EPwm2Regs.CMPCTL.bit.LOADAMODE = 2;  // Load on Zero or Period
   EPwm2Regs.CMPCTL.bit.LOADBMODE = 2;   

   // Set actions
   EPwm2Regs.AQCTLA.bit.ZRO = 0;             	  // Nothing at CNTR =0
   EPwm2Regs.AQCTLA.bit.PRD = 0;             	  // Nothing at CNTR =PRD
   EPwm2Regs.AQCTLA.bit.CAU = 2;             	  // Set PWM1A on event A, up count
   EPwm2Regs.AQCTLA.bit.CAD = 1;           		  // Clear PWM1A on event A, down count
   EPwm2Regs.AQCTLA.bit.CBU = 0;             	  // Nothing on event B, up count
   EPwm2Regs.AQCTLA.bit.CBD = 0;           	   	  // Nothingon event B, down count

   EPwm2Regs.AQCTLB.bit.ZRO = 0;             	  // Nothing at CNTR =0
   EPwm2Regs.AQCTLB.bit.PRD = 0;             	  // Nothing at CNTR =PRD
   EPwm2Regs.AQCTLB.bit.CAU = 1;             	  // cLEAR PWM1B on event A, up count
   EPwm2Regs.AQCTLB.bit.CAD = 2;           		  // sET PWM1B on event A, down count
   EPwm2Regs.AQCTLB.bit.CBU = 0;             	  // Nothing on event B, up count
   EPwm2Regs.AQCTLB.bit.CBD = 0;           	   	  // Nothingon event B, down count

   EPwm2Regs.DBCTL.bit.IN_MODE = 0;           	  // EPWMA is source for both delays
   EPwm2Regs.DBCTL.bit.POLSEL = 2;           	  // EPWMB is inverted
   EPwm2Regs.DBCTL.bit.OUT_MODE = 3;           	  // Deadband for rising adge EPWMA 

   EPwm2Regs.DBRED = DBREDVALUE; 			          	  // Deadband 1 us
   EPwm2Regs.DBFED = DBFEDVALUE; 			          	  // Deadband 1 us
	
	if (pwm_config->tristate_enable == PWM_TRISTATE_ENABLE) {
	   	EALLOW;
		EPwm2Regs.TZSEL.bit.OSHT1 = 1;	// TZ1 - one shot event source
		EPwm2Regs.TZCTL.bit.TZA = 0;		// EPWMxA - to high impedance on TZ1	
		EPwm2Regs.TZCTL.bit.TZB = 0;		// EPWMxB - to high impedance on TZ1

		EPwm2Regs.TZFRC.bit.OST = 1;	// force trip event
		EDIS;
	}
}

void pwm3_init(PWM_CONFIG *pwm_config) {
   // Setup TBCLK
   EPwm3Regs.TBPRD = pwm_config->period_in_cpu_cycles;           			  // Set timer period
   EPwm3Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm3Regs.TBCTR = 0x0000;                      // Clear counter
 
   // Set Compare values
   EPwm3Regs.CMPA.half.CMPA = pwm_config->period_in_cpu_cycles/2;     		// Set compare A value
   EPwm3Regs.CMPB = pwm_config->period_in_cpu_cycles/2;               		// Set Compare B value
//   EPwm3Regs.CMPA.half.CMPA = 1960;     		// Set compare A value
//   EPwm3Regs.CMPB = TBPRDVALUE/2;               		// Set Compare B value


  // Setup counter mode
   EPwm3Regs.TBCTL.bit.CTRMODE = pwm_config->count_mode; 		// Count up and down
   EPwm3Regs.TBCTL.bit.PHSEN = 1;        	// Enable phase loading
   EPwm3Regs.TBCTL.bit.PRDLD = 0;        	// Period Register is loaded from its shadow
   EPwm3Regs.TBCTL.bit.SYNCOSEL = 0;        // SYNCO - SYNCI
   EPwm3Regs.TBCTL.bit.HSPCLKDIV = HSPCLKDIVVALUE;       // Clock ratio to SYSCLKOUT
   EPwm3Regs.TBCTL.bit.CLKDIV = CLKDIVVALUE;
   EPwm3Regs.TBCTL.bit.PHSDIR = 1;        	// Count up after sync event
   EPwm3Regs.TBCTL.bit.FREE_SOFT = 2;       // Free runing during emulation

   // Setup shadowing
   EPwm3Regs.CMPCTL.bit.SHDWAMODE = 0;
   EPwm3Regs.CMPCTL.bit.SHDWBMODE = 0;
   EPwm3Regs.CMPCTL.bit.LOADAMODE = 2;  // Load on Zero or Period
   EPwm3Regs.CMPCTL.bit.LOADBMODE = 2;   

   // Set actions
   EPwm3Regs.AQCTLA.bit.ZRO = 0;             	  // Nothing at CNTR =0
   EPwm3Regs.AQCTLA.bit.PRD = 0;             	  // Nothing at CNTR =PRD
   EPwm3Regs.AQCTLA.bit.CAU = 2;             	  // Set PWM1A on event A, up count
   EPwm3Regs.AQCTLA.bit.CAD = 1;           		  // Clear PWM1A on event A, down count
   EPwm3Regs.AQCTLA.bit.CBU = 0;             	  // Nothing on event B, up count
   EPwm3Regs.AQCTLA.bit.CBD = 0;           	   	  // Nothingon event B, down count

   EPwm3Regs.AQCTLB.bit.ZRO = 0;             	  // Nothing at CNTR =0
   EPwm3Regs.AQCTLB.bit.PRD = 0;             	  // Nothing at CNTR =PRD
   EPwm3Regs.AQCTLB.bit.CAU = 1;             	  // cLEAR PWM1B on event A, up count
   EPwm3Regs.AQCTLB.bit.CAD = 2;           		  // sET PWM1B on event A, down count
   EPwm3Regs.AQCTLB.bit.CBU = 0;             	  // Nothing on event B, up count
   EPwm3Regs.AQCTLB.bit.CBD = 0;           	   	  // Nothingon event B, down count
	
   EPwm3Regs.DBCTL.bit.IN_MODE = 0;           	  // EPWMA is source for both delays
   EPwm3Regs.DBCTL.bit.POLSEL = 2;           	  // EPWMB is inverted
   EPwm3Regs.DBCTL.bit.OUT_MODE = 3;           	  // Deadband for rising adge EPWMA 

   EPwm3Regs.DBRED = DBREDVALUE; 			          	  // Deadband 1 us
   EPwm3Regs.DBFED = DBFEDVALUE; 			          	  // Deadband 1 us
	
	if (pwm_config->tristate_enable == PWM_TRISTATE_ENABLE) {
	   	EALLOW;
		EPwm3Regs.TZSEL.bit.OSHT1 = 1;	// TZ1 - one shot event source
		EPwm3Regs.TZCTL.bit.TZA = 0;		// EPWMxA - to high impedance on TZ1	
		EPwm3Regs.TZCTL.bit.TZB = 0;		// EPWMxB - to high impedance on TZ1

		EPwm3Regs.TZFRC.bit.OST = 1;	// force trip event
		EDIS;
	}

}

void pwm_set_zero_voltage(unsigned long pwm_period_in_cpu_cycles) {
	EPwm1Regs.CMPA.half.CMPA = pwm_period_in_cpu_cycles/2;     		// Set compare A value
//   	EPwm2Regs.CMPA.half.CMPA = pwm_period_in_cpu_cycles/2;     		// Set compare A value
//  	EPwm3Regs.CMPA.half.CMPA = pwm_period_in_cpu_cycles/2;     		// Set compare A value
}

//===========================================================================
// No more.
//===========================================================================
