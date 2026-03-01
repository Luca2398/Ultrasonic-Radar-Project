#include "TM4C129.h"
#include <stdbool.h>

#define SERVO_PWM_PORT (1 << 5)			// Port F
#define SERVO_PWM_PIN (1 << 1)			// Pin 1
#define SERVO_PWM_MODULE (1 << 0)

volatile double DC_MAX = 2000;
volatile double DC_MIN = 1000;

void Delay_MS(int ms) {
    for (int i = 0; i < ms * 16000; i++); // ~1ms delay at 16 MHz
}

void Setup_Servo_PWM(void) {
    SYSCTL->RCGCGPIO |= SERVO_PWM_PORT;
    while ((SYSCTL->PRGPIO & SERVO_PWM_PORT) == 0);

    // Motor PF1
    GPIOF_AHB->AFSEL |= SERVO_PWM_PIN;
    GPIOF_AHB->PCTL &= ~(0xF << 4);
    GPIOF_AHB->PCTL |= (0x6 << 4);
    GPIOF_AHB->DEN |= SERVO_PWM_PIN;
			
    SYSCTL->RCGCPWM |= SERVO_PWM_MODULE;
    while ((SYSCTL->PRPWM & SERVO_PWM_MODULE) == 0);

    PWM0->CC = 0x103;
    PWM0->_0_CTL = 0;
    PWM0->_0_LOAD = 19999;
    PWM0->_0_CMPB = DC_MIN;
    PWM0->_0_GENB |= (0x2 << 2) | (0x3 << 10);
    PWM0->_0_CTL = 1;
    PWM0->ENABLE |= (1 << 1);
}
