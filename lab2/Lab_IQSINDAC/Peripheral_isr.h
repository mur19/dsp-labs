#ifndef PERIPHERAL_ISR_H
#define PERIPHERAL_ISR_H

interrupt void pwm_isr(void);
interrupt void qep1_isr(void);
interrupt void timer0_isr(void);
interrupt void adc_isr(void);
interrupt void can_receive_isr(void); 
interrupt void can_transmit_isr(void);  

#endif
