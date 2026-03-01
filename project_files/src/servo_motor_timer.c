#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define PORT_N (1 << 12)
#define PIN_0 (1 << 0)
#define LOAD_VAL 16000000 / 1000 - 1

volatile int counter;
volatile int dutyCycle = 50;
volatile bool led_on;

void init_TIMER(void)
{
	SysTick->CTRL = 0x0;
	SysTick->LOAD = LOAD_VAL;
	SysTick->VAL = 0;
	SysTick->CTRL = 0x7;
}

void configure_PORT_N(void)
{
	SYSCTL->RCGCGPIO |= PORT_N;
	while ((SYSCTL->PRGPIO & PORT_N) != PORT_N);
	
	GPION->DEN |= PIN_0;
	GPION->DIR |= PIN_0;
	GPION->DATA &= ~PIN_0;
}

void D2_on(void)
{
	GPION->DATA |= PIN_0;
}

void D2_off(void)
{
	GPION->DATA &= ~PIN_0;
}

void Set_Duty_Cycle(int percent) {
	dutyCycle = percent;
}

void SysTick_Handler(void)
{
	counter++;
	if (counter <= (dutyCycle*10)) {
		D2_on();

	}
	else  {
		D2_off();
	}
	
	if (counter == 1000){
		counter = 0;
	}
}

// ************* main function ***********************
int main(void)
{
	ES_Serial(0,"115200,8,N,1") ;
	
	configure_PORT_N();
	init_TIMER();
	
	// message to check if the serail connection is working
	ES_printf("\n****************************************\n") ;
	ES_printf("\n Laboratory 4 \n") ;
	ES_printf("\n****************************************\n") ;
	
	__enable_irq();
	
	// loop forever and enter and print a decimal value.
	// readDec() needed for Part B
	while(true) 
		{
		int d=readDec() ;
		Set_Duty_Cycle(d);
		ES_printf("\ndec = %i\n",d) ;
		}
	
	return 0;
}


