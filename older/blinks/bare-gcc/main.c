#include "lpc17xx.h"

volatile int g_LoopDummy;

void startup_delay() {}

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
