// Code for buzzer

#include "TM4C129.h"

#define BUZZER_PWM_PORT_F (1 << 5)			// Port F
#define BUZZER_PWM_PIN_2 (1 << 2)			// Pin 2
#define BUZZER_PWM_MODULE_0 (1 << 0)

// Function prototypes
void setup_buzzer_pwm(void);

void setup_buzzer_pwm(void) {
    // Timing and mode constraints
    const uint32_t pctlMask = (0xF << 8);
    const uint32_t pwmFuncEnable = (0x6 << 8);
    const uint32_t pwmDisable = 0;
    const uint32_t pwmEnable = 1;
    const uint32_t pwmPeriodTicks = 19999;
    const uint32_t pwmDutyCycle = 100;

    // Waveform generation
    const uint32_t setHighOnLoad = (0x3 << 6);
    const uint32_t setLowOnMatch = (0x2 << 2);

	SYSCTL->RCGCGPIO |= BUZZER_PWM_PORT_F;
	while ((SYSCTL->PRGPIO & BUZZER_PWM_PORT_F) == 0);

    // GPIO setup for PWM
	// Buzzer PF2
	GPIOF_AHB->AFSEL |= BUZZER_PWM_PIN_2;
    GPIOF_AHB->PCTL &= ~pctlMask;
    GPIOF_AHB->PCTL |= pwmFuncEnable;
    GPIOF_AHB->DEN |= BUZZER_PWM_PIN_2;

	SYSCTL->RCGCPWM |= BUZZER_PWM_MODULE_0;
	while ((SYSCTL->PRPWM & BUZZER_PWM_MODULE_0) == 0);

	PWM0->_1_CTL = disableCTL;
    PWM0->_1_LOAD = pwmPeriodTicks;
    PWM0->_1_CMPA = pwmDutyCycle;
    PWM0->_1_GENA |= setLowOnMatch | setHighOnLoad;
    PWM0->_1_CTL = pwmEnable;
}
