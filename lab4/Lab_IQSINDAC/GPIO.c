#include "GPIO.h"

//------------------------------GPIO0--------------------------------

void gpio0_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;
	GpioCtrlRegs.GPAPUD.bit.GPIO0 = pullup;	
	GpioCtrlRegs.GPAQSEL1.bit.GPIO0 = sync_mode;    
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = pin_function; 
	GpioCtrlRegs.GPADIR.bit.GPIO0 = direction;
	EDIS;
}

void gpio0_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
}

void gpio0_set(void) {
	GpioDataRegs.GPASET.bit.GPIO0 = 1;
}

void gpio0_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO0 = 1;
}

//------------------------------GPIO1--------------------------------

void gpio1_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAPUD.bit.GPIO1 = pullup;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO1 = sync_mode;    
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = pin_function; 
	GpioCtrlRegs.GPADIR.bit.GPIO1 = direction;
	EDIS;
}

void gpio1_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
}

void gpio1_set(void) {
	GpioDataRegs.GPASET.bit.GPIO1 = 1;
}

void gpio1_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1;
}

//------------------------------GPIO2--------------------------------
void gpio2_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAPUD.bit.GPIO2 = pullup;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO2 = sync_mode;    
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = pin_function; 
	GpioCtrlRegs.GPADIR.bit.GPIO2 = direction;
	EDIS;
}

void gpio2_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
}

void gpio2_set(void) {
	GpioDataRegs.GPASET.bit.GPIO2 = 1;
}

void gpio2_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO2 = 1;
}

//------------------------------GPIO3--------------------------------
void gpio3_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAPUD.bit.GPIO3 = pullup;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = sync_mode;    
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = pin_function; 
	GpioCtrlRegs.GPADIR.bit.GPIO3 = direction;
	EDIS;
}

void gpio3_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
}

void gpio3_set(void) {
	GpioDataRegs.GPASET.bit.GPIO3 = 1;
}

void gpio3_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO3 = 1;
}

//------------------------------GPIO4--------------------------------
void gpio4_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAPUD.bit.GPIO4 = pullup;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO4 = sync_mode;    
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = pin_function; 
	GpioCtrlRegs.GPADIR.bit.GPIO4 = direction;
	EDIS;
}

void gpio4_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
}

void gpio4_set(void) {
	GpioDataRegs.GPASET.bit.GPIO4 = 1;
}

void gpio4_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO4 = 1;
}

//------------------------------GPIO5--------------------------------
void gpio5_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAPUD.bit.GPIO5 = pullup;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = sync_mode;    
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = pin_function; 
	GpioCtrlRegs.GPADIR.bit.GPIO5 = direction;
	EDIS;
}

void gpio5_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
}

void gpio5_set(void) {
	GpioDataRegs.GPASET.bit.GPIO5 = 1;
}

void gpio5_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO5 = 1;
}
//------------------------------GPIO6--------------------------------
void gpio6_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAPUD.bit.GPIO6 = pullup;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO6 = sync_mode;    
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = pin_function; 
	GpioCtrlRegs.GPADIR.bit.GPIO6 = direction;
	EDIS;
}

void gpio6_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
}

void gpio6_set(void) {
	GpioDataRegs.GPASET.bit.GPIO6 = 1;
}

void gpio6_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO6 = 1;
}
//------------------------------GPIO7--------------------------------
void gpio7_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAPUD.bit.GPIO7 = pullup;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = sync_mode;    
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = pin_function; 
	GpioCtrlRegs.GPADIR.bit.GPIO7 = direction;
	EDIS;
}

void gpio7_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
}

void gpio7_set(void) {
	GpioDataRegs.GPASET.bit.GPIO7 = 1;
}

void gpio7_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO7 = 1;
}
//------------------------------GPIO18--------------------------------

void gpio18_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = sync_mode; 
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = pin_function;
	GpioCtrlRegs.GPADIR.bit.GPIO18 = direction;     
	EDIS;
}

void gpio18_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;
}

void gpio18_set(void) {
	GpioDataRegs.GPASET.bit.GPIO18 = 1;
}

void gpio18_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO18 = 1;
}

//------------------------------GPIO19--------------------------------

void gpio19_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = sync_mode; 
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = pin_function;
	GpioCtrlRegs.GPADIR.bit.GPIO19 = direction;     
	EDIS;
}

void gpio19_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;
}

void gpio19_set(void) {
	GpioDataRegs.GPASET.bit.GPIO19 = 1;
}

void gpio19_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO19 = 1;
}

//------------------------------GPIO20--------------------------------

void gpio20_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO20 = sync_mode; 
	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = pin_function;
	GpioCtrlRegs.GPADIR.bit.GPIO20 = direction;     
	EDIS;
}

void gpio20_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO20 = 1;
}

void gpio20_set(void) {
	GpioDataRegs.GPASET.bit.GPIO20 = 1;
}

void gpio20_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO20 = 1;
}

//------------------------------GPIO21--------------------------------

void gpio21_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = sync_mode; 
	GpioCtrlRegs.GPAMUX2.bit.GPIO21 = pin_function;
	GpioCtrlRegs.GPADIR.bit.GPIO21 = direction;     
	EDIS;
}

void gpio21_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO21 = 1;
}

void gpio21_set(void) {
	GpioDataRegs.GPASET.bit.GPIO21 = 1;
}

void gpio21_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO21 = 1;
}

//------------------------------GPIO23--------------------------------

void gpio23_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = sync_mode; 
	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = pin_function;
	GpioCtrlRegs.GPADIR.bit.GPIO23 = direction;     
	EDIS;
}

void gpio23_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO23 = 1;
}

void gpio23_set(void) {
	GpioDataRegs.GPASET.bit.GPIO23 = 1;
}

void gpio23_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO23 = 1;
}

//------------------------------GPIO24--------------------------------

void gpio24_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = sync_mode; 
	GpioCtrlRegs.GPAMUX2.bit.GPIO24 = pin_function;
	GpioCtrlRegs.GPADIR.bit.GPIO24 = direction;     
	EDIS;
}

void gpio24_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO24 = 1;
}

void gpio24_set(void) {
	GpioDataRegs.GPASET.bit.GPIO24 = 1;
}

void gpio24_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO24 = 1;
}

//------------------------------GPIO25--------------------------------

void gpio25_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = sync_mode; 
	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = pin_function;
	GpioCtrlRegs.GPADIR.bit.GPIO25 = direction;     
	EDIS;
}

void gpio25_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO25 = 1;
}

void gpio25_set(void) {
	GpioDataRegs.GPASET.bit.GPIO25 = 1;
}

void gpio25_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO25 = 1;
}


//------------------------------GPIO26--------------------------------

void gpio26_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = sync_mode; 
	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = pin_function;
	GpioCtrlRegs.GPADIR.bit.GPIO26 = direction;     
	EDIS;
}

void gpio26_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO26 = 1;
}

void gpio26_set(void) {
	GpioDataRegs.GPASET.bit.GPIO26 = 1;
}

void gpio26_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO26 = 1;
}

//------------------------------GPIO27--------------------------------

void gpio27_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO27 = sync_mode; 
	GpioCtrlRegs.GPAMUX2.bit.GPIO27 = pin_function;
	GpioCtrlRegs.GPADIR.bit.GPIO27 = direction;     
	EDIS;
}

void gpio27_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;
}

void gpio27_set(void) {
	GpioDataRegs.GPASET.bit.GPIO27 = 1;
}

void gpio27_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO27 = 1;
}

//------------------------------GPIO30--------------------------------

void gpio30_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO30 = sync_mode; 
	GpioCtrlRegs.GPAMUX2.bit.GPIO30 = pin_function;
	GpioCtrlRegs.GPADIR.bit.GPIO30 = direction;     
	EDIS;
}

void gpio30_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO30 = 1;
}

void gpio30_set(void) {
	GpioDataRegs.GPASET.bit.GPIO30 = 1;
}

void gpio30_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO30 = 1;
}

//------------------------------GPIO31--------------------------------

void gpio31_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO31 = sync_mode; 
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = pin_function;
	GpioCtrlRegs.GPADIR.bit.GPIO31 = direction;     
	EDIS;
}

void gpio31_clear(void) {
	GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;
}

void gpio31_set(void) {
	GpioDataRegs.GPASET.bit.GPIO31 = 1;
}

void gpio31_toggle(void) {
	GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
}

//------------------------------GPIO50--------------------------------

void gpio50_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPBQSEL2.bit.GPIO50 = sync_mode; 
	GpioCtrlRegs.GPBMUX2.bit.GPIO50 = pin_function;
	GpioCtrlRegs.GPBDIR.bit.GPIO50 = direction;     
	EDIS;
}

void gpio50_clear(void) {
	GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;
}

void gpio50_set(void) {
	GpioDataRegs.GPBSET.bit.GPIO50 = 1;
}

void gpio50_toggle(void) {
	GpioDataRegs.GPBTOGGLE.bit.GPIO50 = 1;
}

//------------------------------GPIO51--------------------------------

void gpio51_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPBQSEL2.bit.GPIO51 = sync_mode; 
	GpioCtrlRegs.GPBMUX2.bit.GPIO51 = pin_function;
	GpioCtrlRegs.GPBDIR.bit.GPIO51 = direction;     
	EDIS;
}

void gpio51_clear(void) {
	GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1;
}

void gpio51_set(void) {
	GpioDataRegs.GPBSET.bit.GPIO51 = 1;
}

void gpio51_toggle(void) {
	GpioDataRegs.GPBTOGGLE.bit.GPIO51 = 1;
}

//------------------------------GPIO53--------------------------------

void gpio53_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPBQSEL2.bit.GPIO53 = sync_mode; 
	GpioCtrlRegs.GPBMUX2.bit.GPIO53 = pin_function;
	GpioCtrlRegs.GPBDIR.bit.GPIO53 = direction;     
	EDIS;
}

void gpio53_clear(void) {
	GpioDataRegs.GPBCLEAR.bit.GPIO53 = 1;
}

void gpio53_set(void) {
	GpioDataRegs.GPBSET.bit.GPIO53 = 1;
}

void gpio53_toggle(void) {
	GpioDataRegs.GPBTOGGLE.bit.GPIO53 = 1;
}

//------------------------------GPIO63--------------------------------

void gpio63_init(int direction, int pin_function, int sync_mode, short pullup) {
	EALLOW;	
	GpioCtrlRegs.GPBQSEL2.bit.GPIO63 = sync_mode;   // Asynch input GPIO63 
	GpioCtrlRegs.GPBMUX2.bit.GPIO63 = pin_function;    // Select GPIO63 function
	GpioCtrlRegs.GPBDIR.bit.GPIO63 = direction;     // GPIO63 = output
	EDIS;
}

void gpio63_clear(void) {
	GpioDataRegs.GPBCLEAR.bit.GPIO63 = 1;
}

void gpio63_set(void) {
	GpioDataRegs.GPBSET.bit.GPIO63 = 1;
}

void gpio63_toggle(void) {
	GpioDataRegs.GPBTOGGLE.bit.GPIO63 = 1;
}
