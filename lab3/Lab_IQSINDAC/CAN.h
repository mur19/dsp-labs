#ifndef CAN_H
#define CAN_H

void cana_init(void);
void cana_send(unsigned long *data);

extern struct ECAN_REGS ECanaShadow;

#endif
