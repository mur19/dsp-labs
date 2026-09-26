#ifndef QEP_HEADER_H
#define QEP_HEADER_H

#define QEP_RESET_ON_INDEX 0
#define QEP_RESET_ON_MAXIMUM 1

typedef struct {
	unsigned long resolution_in_discretes; 
	unsigned short reset_mode; 
	unsigned long value_to_compare;
} QEP_CONFIG;
	
void qep1_init(QEP_CONFIG *qep_config);
interrupt void qep1_isr(void);

#endif
