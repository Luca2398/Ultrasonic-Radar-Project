#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include "ES_Lib.h"

#define SENSOR_TRIG_TIMER (1 << 1)
#define SENSOR_ECHO_TIMER (1 << 2)
#define SENSOR_TRIG_PORT (1 << 12)
#define SENSOR_TRIG_PIN (1 << 2)
#define SENSOR_ECHO_PORT (1 << 0)
#define SENSOR_ECHO_PIN (1 << 4)

volatile double DISTANCE;
volatile uint16_t PULSE_WIDTH;
volatile uint16_t RISING_EDGE_TIME;
volatile uint16_t FALLING_EDGE_TIME;

// Function definitions
void Sensor_Trig(void);
void Sensor_Echo(void);
void Get_Sensor_Data(void);

void Sensor_Trig(void) {
	SYSCTL->RCGCGPIO |= SENSOR_TRIG_PORT;
	while ((SYSCTL->PRGPIO & SENSOR_TRIG_PORT) == 0);
	
	GPION->DEN |= SENSOR_TRIG_PIN;
	GPION->DIR |= SENSOR_TRIG_PIN;
	GPION->DATA &= ~SENSOR_TRIG_PIN;

	SYSCTL->RCGCTIMER |= SENSOR_TRIG_TIMER;
	while ((SYSCTL->PRTIMER & SENSOR_TRIG_TIMER) == 0);
}

void Sensor_Input(void) {
	TIMER1->CTL &= ~0x1;
	TIMER1->CFG = 0x4;
	TIMER1->TAMR &= ~0x3;
	TIMER1->TAMR = 0x1;
	TIMER1->TAILR = 1;
	TIMER1->TAPR = 159;
	TIMER1->ICR |= 0x1;
}

void Sensor_Echo(void) {
	SYSCTL->RCGCGPIO |= SENSOR_ECHO_PORT;
	while ((SYSCTL->PRGPIO & SENSOR_ECHO_PORT) == 0);
	
	GPIOA_AHB->AFSEL |= SENSOR_ECHO_PIN;
	GPIOA_AHB->PCTL &= ~(0xF << (4 * 4));
	GPIOA_AHB->PCTL |= (3 << (4 * 4));
	GPIOA_AHB->DEN |= SENSOR_ECHO_PIN;
	GPIOA_AHB->DIR &= ~SENSOR_ECHO_PIN;
	
	SYSCTL->RCGCTIMER |= SENSOR_ECHO_TIMER;
	while ((SYSCTL->PRTIMER & SENSOR_ECHO_TIMER) == 0);
}

void Sensor_Output(void) {
	TIMER2->CTL &= ~0x1;
	TIMER2->CFG = 0x4;
	TIMER2->TAMR = 0x7;
	TIMER2->TAILR = 0xFFFF;
	TIMER2->CTL &= ~0x0C;
	TIMER2->CTL |= 0x00;
	TIMER2->ICR = (1 << 2);
	TIMER2->CTL |= 0x1;
	
	while ((TIMER2->RIS & (1 << 2)) == 0);
	RISING_EDGE_TIME = TIMER2->TAR;
	TIMER2->ICR = (1 << 2);
	
	TIMER2->CTL |= 0x4;
	
	while ((TIMER2->RIS & (1 << 2)) == 0);
	FALLING_EDGE_TIME = TIMER2->TAR;
	TIMER2->ICR = (1 << 2);
}

void Get_Sensor_Data(void) {
	Sensor_Input();
	
	GPION->DATA |= (1 << 2);
	TIMER1->CTL |= (1 << 0);
	while ((TIMER1->RIS & (1 << 0)) == 0);
	TIMER1->ICR = (1 << 0);  
	GPION->DATA &= ~(1 << 2);
	
	Sensor_Output();
	
	if (FALLING_EDGE_TIME > RISING_EDGE_TIME)
		PULSE_WIDTH = FALLING_EDGE_TIME - RISING_EDGE_TIME;
	else
		PULSE_WIDTH = RISING_EDGE_TIME - FALLING_EDGE_TIME;
	
	DISTANCE = ((double)PULSE_WIDTH * 34300.0) / 32000000.0;
	ES_printf("Distance: %i cm\n", (int)DISTANCE);
	
	if (DISTANCE < 5.0) {
		PWM0->ENABLE |= (1 << 2);
		msDelay(500);
		PWM0->ENABLE &= ~(1 << 2);
	}
	
	msDelay(300);
}