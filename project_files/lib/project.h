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
	extern void setup_uart(void);
	extern int read_char(void);
	extern void write_char(char data);
	extern void write_string(const char *str);
	extern void write_int(uint32_t num);

	// Buzzer functions
	extern void setup_buzzer_pwm(void);

	// Servo motor functions
	extern void delay_ms(int ms);
	extern void setup_servo_pwm(void);
	//extern void Automatic_Mode(void);

	// Ultrasonic sensor functions
	extern void sensor_trig(void);
	extern void sensor_echo(void);
	extern void get_sensor_data(void);

	// Ultrasonic sensor variables
	extern double distance;
	extern volatile uint16_t pulseWidth;
	extern volatile uint16_t risingEdgeTime;
	extern volatile uint16_t fallingEdgeTime;
	extern volatile double dcMax;
	extern volatile double dcMin;

#endif /* PROJ */

