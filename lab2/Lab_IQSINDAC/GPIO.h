#ifndef GPIO_INIT_H
#define GPIO_INIT_H

#include "DSP2833x_Device.h" 

// GPIO direction
#define GPIO_OUT 1
#define GPIO_IN  0

// GPIO synchronization mode
#define GPIO_ASYNC_MODE 3
#define GPIO_SYNC_MODE 0

// GPIO pin function
#define GPIO_PIN 0
#define GPIO_PWM 1
#define GPIO_QEP 1
#define GPIO_CAN 1
#define GPIO_CAN_TECHNOSOFT 3 
#define GPIO_MCBSPB 3

// GPIO pull up
#define GPIO_PULLUP_ON 0
#define GPIO_PULLUP_OFF 1

void gpio0_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio0_clear(void);
void gpio0_set(void);
void gpio0_toggle(void);

void gpio1_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio1_clear(void);
void gpio1_set(void);
void gpio1_toggle(void);

void gpio2_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio2_clear(void);
void gpio2_set(void);
void gpio2_toggle(void);

void gpio3_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio3_clear(void);
void gpio3_set(void);
void gpio3_toggle(void);

void gpio4_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio4_clear(void);
void gpio4_set(void);
void gpio4_toggle(void);

void gpio5_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio5_clear(void);
void gpio5_set(void);
void gpio5_toggle(void);

void gpio6_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio6_clear(void);
void gpio6_set(void);
void gpio6_toggle(void);

void gpio7_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio7_clear(void);
void gpio7_set(void);
void gpio7_toggle(void);

void gpio18_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio18_clear(void);
void gpio18_set(void);
void gpio18_toggle(void);

void gpio19_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio19_clear(void);
void gpio19_set(void);
void gpio19_toggle(void);

void gpio20_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio20_clear(void);
void gpio20_set(void);
void gpio20_toggle(void);

void gpio21_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio21_clear(void);
void gpio21_set(void);
void gpio21_toggle(void);

void gpio23_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio23_clear(void);
void gpio23_set(void);
void gpio23_toggle(void);

void gpio24_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio24_clear(void);
void gpio24_set(void);
void gpio24_toggle(void);

void gpio25_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio25_clear(void);
void gpio25_set(void);
void gpio25_toggle(void);

void gpio26_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio26_clear(void);
void gpio26_set(void);
void gpio26_toggle(void);

void gpio27_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio27_clear(void);
void gpio27_set(void);
void gpio27_toggle(void);

void gpio30_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio30_clear(void);
void gpio30_set(void);
void gpio30_toggle(void);

void gpio31_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio31_clear(void);
void gpio31_set(void);
void gpio31_toggle(void);

void gpio50_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio50_clear(void);
void gpio50_set(void);
void gpio50_toggle(void);

void gpio51_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio51_clear(void);
void gpio51_set(void);
void gpio51_toggle(void);

void gpio53_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio53_clear(void);
void gpio53_set(void);
void gpio53_toggle(void);

void gpio63_init(int direction, int pin_function, int sync_mode, short pullup);
void gpio63_clear(void);
void gpio63_set(void);
void gpio63_toggle(void);

#endif
