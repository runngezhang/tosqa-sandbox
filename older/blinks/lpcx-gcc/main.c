/* Blink LED on LXPXpresso 1769 without using the mbed library. */

// upload over serial on Mac:
//  lpc21isp Blink.hex /dev/tty.usbserial-A600K1PM 115200 12000

#include "LPC17xx.h"

volatile int g_LoopDummy;

int main() 
{
    LPC_PINCON->PINSEL1 &= ~ (3 << 12); // set general GPIO mode 0
    LPC_GPIO0->FIODIR |= 1 << 22; // P0.22 connected to LED2
    while(1)
    {
        int i;
        
        LPC_GPIO0->FIOPIN ^= 1 << 22; // Toggle P0.22
        for (i = 0 ; i < 5000000 && !g_LoopDummy ; i++)
        {
        }
    }
    return 0;
}
