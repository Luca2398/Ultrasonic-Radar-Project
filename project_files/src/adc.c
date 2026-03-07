// Code for analog to digital converter required by potentiometer

#include "TM4C129.h"

#define ADC_PORT_4 (1 << 4)
#define ADC_INPUT_PIN_3 (1 << 3)
#define ADC_MODULE_0 (1 << 0)

void setup_adc(void);
uint32_t read_adc(void);

void setup_adc(void)
{
    uint32_t

	SYSCTL->RCGCGPIO |= ADC_PORT;
	while ((SYSCTL->PRGPIO & ADC_PORT) == 0);

	GPIOE_AHB->AFSEL |= ADC_INPUT_PIN_3;
	GPIOE_AHB->AMSEL |= ADC_INPUT_PIN_3;
	GPIOE_AHB->DEN &= ~ADC_INPUT_PIN_3;
	GPIOE_AHB->DIR &= ~ADC_INPUT_PIN_3;

	SYSCTL->RCGCADC |= ADC_MODULE_0;
	while ((SYSCTL->PRADC & ADC_MODULE_0) == 0);

	ADC0->PC = 0x7;
	ADC0->ACTSS &= ~(1 << 3);
    ADC0->EMUX &= ~(0xF << 12);
    ADC0->SSMUX3 = 0x0;
    ADC0->SSCTL3 = (1 << 1) | (1 << 2);
    ADC0->ACTSS |= (1 << 3);
}

uint32_t read_adc(void)
{
	ADC0->PSSI |= (1 << 3);
	while ((ADC0->RIS & (1 << 3)) == 0);
	uint32_t result = ADC0->SSFIFO3 & 0xFFF;
	ADC0->ISC = (1 << 3);

	return result;
}
