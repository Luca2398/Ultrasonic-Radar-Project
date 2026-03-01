#ifndef PROJ
#define PROJ

	#include <TM4C129.h>
	#include "ES_Lib.h"
	#include "LCD_Display.h"

	#include <stdlib.h>
	#include <stdint.h>
	#include <stdbool.h>
	#include <float.h>

	// UART functions
	extern void Setup_UART(void);
	extern int Read_Char(void);
	extern void Write_Char(char data);
	extern void Write_String(const char *str);
	extern void Write_Int(uint32_t num);


	// Buzzer functions
	extern void setup_Buzzer_PWM(void);


	// Potentiometer functions
	extern void Setup_ADC(void);
	extern uint32_t Read_ADC(void);
	

	// Servo motor functions
	extern void Delay_MS(int ms);
	extern void Setup_Servo_PWM(void);
	//extern void Automatic_Mode(void);
	
	
	// Ultrasonic sensor functions
	extern void Sensor_Trig(void);
	extern void Sensor_Echo(void);
	extern void Get_Sensor_Data(void);
	// Ultrasonic sensor variables
	extern double DISTANCE;
	extern volatile uint16_t PULSE_WIDTH;
	extern volatile uint16_t RISING_EDGE_TIME;
	extern volatile uint16_t FALLING_EDGE_TIME;
	extern volatile double DC_MAX;
	extern volatile double DC_MIN;

#endif /* PROJ */

