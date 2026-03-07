// Code for ultrasonic sensor

#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include "ES_Lib.h"

#define SENSOR_TRIG_TIMER_1 (1 << 1)
#define SENSOR_ECHO_TIMER_2 (1 << 2)
#define SENSOR_TRIG_PORT_12 (1 << 12)
#define SENSOR_TRIG_PIN_2 (1 << 2)
#define SENSOR_ECHO_PORT_0 (1 << 0)
#define SENSOR_ECHO_PIN_4 (1 << 4)

volatile double distance;
volatile uint16_t pulseWidth;
volatile uint16_t risingEdgeTime;
volatile uint16_t fallingEdgeTime;

// Function prototypes
void sensor_trig(void);
void sensor_echo(void);
void get_sensor_data(void);
void sensor_input(void);
void sensor_output(void);

void sensor_trig(void)
{
	SYSCTL->RCGCGPIO |= SENSOR_TRIG_PORT_12;
	while ((SYSCTL->PRGPIO & SENSOR_TRIG_PORT_12) == 0);

	GPION->DEN |= SENSOR_TRIG_PIN_2;
	GPION->DIR |= SENSOR_TRIG_PIN_2;
	GPION->DATA &= ~SENSOR_TRIG_PIN_2;

	SYSCTL->RCGCTIMER |= SENSOR_TRIG_TIMER_1;
	while ((SYSCTL->PRTIMER & SENSOR_TRIG_TIMER_1) == 0);
}

// Function for initialising the one shot signal required by the ultrasonic sensor
void sensor_input(void)
{
	const uint32_t enableBit = 0x1;
	const uint32_t config16Bit = 0x4;
	const uint32_t modeMask = 0x3;
	const uint32_t oneShotMode = 0x1;
	const uint32_t loadVal = 1;
	const uint32_t prescaleVal = 159;
	const uint32_t clearInt = 0x1;

	// Initalisation
	TIMER1->CTL &= ~enableBit;
	TIMER1->CFG = config16Bit;
	TIMER1->TAMR &= ~modeMask;
	TIMER1->TAMR = oneShotMode;
	TIMER1->TAILR = loadVal;
	TIMER1->TAPR = prescaleVal;
	TIMER1->ICR |= clearInt;
}

// Function for obtaining measured data from sensor
void sensor_output(void)
{
	// Constant variables
	const uint32_t enableBit = 0x1;
	const uint32_t config16Bit = 0x4;
	const uint32_t edgeCaptureMode = 0x7;
	const uint32_t maxLoadVal = 0xFFFF;
	const uint32_t eventMask = 0x0C;
	const uint32_t risingEdge = 0x00;
	const uint32_t captureInt = (1 << 2);
	const uint32_t timerEnable = 0x01;
	const uint32_t readSensor = (1 << 2);
	const uint32_t edgeFalling = 0x4;

	// Initialisation
	TIMER2->CTL &= ~enableBit;
	TIMER2->CFG = config16Bit;
	TIMER2->TAMR = edgeCaptureMode;
	TIMER2->TAILR = maxLoadVal;

	// Start searching for rising edge
	TIMER2->CTL &= ~eventMask;
	TIMER2->CTL |= risingEdge;
	TIMER2->ICR = captureInt;
	TIMER2->CTL |= timerEnable;

	// Capture rising edge
	while ((TIMER2->RIS & readSensor) == 0);
	risingEdgeTime = TIMER2->TAR;
	TIMER2->ICR = captureInt;

	// Flip to falling edge
	TIMER2->CTL &= ~eventMask;
	TIMER2->CTL |= edgeFalling;

	// Capture falling edge
	while ((TIMER2->RIS & readSensor) == 0);
	fallingEdgeTime = TIMER2->TAR;
	TIMER2->ICR = captureInt;
}

void get_sensor_data(void)
{
	const uint32_t sensorTrigPin = (1 << 2);
	const uint32_t timerAEnable = (1 << 0);
	const uint32_t timerOutInt = (1 << 0);
	const uint32_t pwmOutput2Enable = (1 << 2);

	// Obtain ultrasonic sensor data using function
	sensor_input();

	GPION->DATA |= sensorTrigPin;
	TIMER1->CTL |= timerAEnable;
	// Check if timer A is enabled
	while ((TIMER1->RIS & timerAEnable) == 0);
	TIMER1->ICR = timerOutInt;
	GPION->DATA &= ~sensorTrigPin;

	sensor_output();

	// Calculate pulse width (pulse high duration time)
	if (fallingEdgeTime > risingEdgeTime)
		pulseWidth = fallingEdgeTime - risingEdgeTime;
	else
		pulseWidth = risingEdgeTime - fallingEdgeTime;

	// Calculation to determine distance to detected object from pulse width returned from ultrasonic sensor
	distance = ((double)pulseWidth * 34300.0) / 32000000.0;
	// Print determined distance to terminal
	ES_printf("Distance: %i cm\n", (int)distance);

	if (distance < 5.0) {
		PWM0->ENABLE |= pwmOutput2Enable;
		msDelay(500);
		PWM0->ENABLE &= ~pwmOutput2Enable;
	}

	// Add delay to prevent interference
	msDelay(300);
}
