// Code for servo motor setup

#include "TM4C129.h"
#include <stdbool.h>

#define SERVO_PWM_PORT_F (1 << 5)			// Port F
#define SERVO_PWM_PIN_1 (1 << 1)			// Pin 1
#define SERVO_PWM_MODULE_0 (1 << 0)

volatile double dcMax = 2000;
volatile double dcMin = 1000;

void setup_servo_pwm(void);
void createDelay(int ms);

void createDelay(int ms) {
    const int cyclesPerMs = 16000;
    for (int iterationCounter = 0; iterationCounter < ms * cyclesPerMs; iterationCounter++); // ~1ms delay at 16 MHz
}

void setup_servo_pwm(void) {
    const uint32_t pctlMask = (0xF << 4);
    const uint32_t pwmFuncEnable = (0x6 << 4);
    const uint32_t pwmClkDiv64 = 0x103;
    const uint32_t pwmDisable = 0;
    const uint32_t pwmEnable = 1;
    const uint32_t pwmPeriodTicks = 19999;
    const uint32_t pwmOutputBEnable = (1 << 1);

    const uint32_t setHighOnLoad = (0x3 << 10);
    const uint32_t setLowOnMatch = (0x2 << 2);

    SYSCTL->RCGCGPIO |= SERVO_PWM_PORT_F;
    while ((SYSCTL->PRGPIO & SERVO_PWM_PORT_F) == 0);

    // Motor PF1
    GPIOF_AHB->AFSEL |= SERVO_PWM_PIN_1;
    GPIOF_AHB->PCTL &= ~pctlMask;
    GPIOF_AHB->PCTL |= pwmFuncEnable;
    GPIOF_AHB->DEN |= SERVO_PWM_PIN_1;

    SYSCTL->RCGCPWM |= SERVO_PWM_MODULE_0;
    while ((SYSCTL->PRPWM & SERVO_PWM_MODULE_0) == 0);

    PWM0->CC = pwmClkDiv64;
    PWM0->_0_CTL = pwmDisable;
    PWM0->_0_LOAD = pwmPeriodTicks;
    PWM0->_0_CMPB = dcMin;
    PWM0->_0_GENB |= setLowOnMatch | setHighOnLoad;
    PWM0->_0_CTL = pwmEnable;
    PWM0->ENABLE |= pwmOutputBEnable;
}
