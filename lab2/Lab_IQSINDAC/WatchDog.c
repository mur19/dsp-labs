#include "DSP2833x_Device.h"     // DSP280x Headerfile Include File

// This function disables the watchdog timer.

void disable_watchdog(void) {
    EALLOW;
    SysCtrlRegs.WDCR= 0x0068;
    EDIS;
}

// This function resets the watchdog timer.
// Enable this function for using ServiceDog in the application 

void service_watchdog(void) {
    EALLOW;
    SysCtrlRegs.WDKEY = 0x0055;
    SysCtrlRegs.WDKEY = 0x00AA;
    EDIS;
}
