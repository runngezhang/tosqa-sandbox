#include "lpc17xx.h"
#include "uart0.c"

volatile int dummy;

void startup_delay() {}

static void initLed () {
	LPC_PINCON->PINSEL1 &= ~ (3 << 12); // set general GPIO mode 0
	LPC_GPIO0->FIODIR |= 1 << 22; // P0.22 connected to LED2
}

static void toggleLed () {
	LPC_GPIO0->FIOPIN ^= 1 << 22; // Toggle P0.22
}

static void wasteSomeTime (int rate) {
	for (int i = 0 ; i < rate * 10000 && !dummy ; i++) {}
}

int main() 
{
	UART0_Init(57600);
	UART0_PrintString("Hello, world!\n");

	initLed();
	while (1) {
		toggleLed();
		UART0_Sendchar('+');
		wasteSomeTime(500);
	}

	return 0;
}
