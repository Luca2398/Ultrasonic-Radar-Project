#include "TM4C129.h"

#define BUZZER_PWM_PORT (1 << 5)			// Port F
#define BUZZER_PWM_PIN (1 << 2)				// Pin 2
#define BUZZER_PWM_MODULE (1 << 0)

void setup_Buzzer_PWM(void);

void setup_Buzzer_PWM(void) {
	SYSCTL->RCGCGPIO |= BUZZER_PWM_PORT;
	while ((SYSCTL->PRGPIO & BUZZER_PWM_PORT) == 0);

	// Buzzer PF2
	GPIOF_AHB->AFSEL |= (1 << 2);
  GPIOF_AHB->PCTL &= ~(0xF << 8);
  GPIOF_AHB->PCTL |= (0x6 << 8);
  GPIOF_AHB->DEN |= (1 << 2);
		
	SYSCTL->RCGCPWM |= BUZZER_PWM_MODULE;
	while ((SYSCTL->PRPWM & BUZZER_PWM_MODULE) == 0);

	PWM0->_1_CTL = 0;
  PWM0->_1_LOAD = 19999;
  PWM0->_1_CMPA = 100;
  PWM0->_1_GENA |= (0x2 << 2) | (0x3 << 6);
  PWM0->_1_CTL = 1;
}