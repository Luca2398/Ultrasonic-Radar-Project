// Main project code

#include "project.h"

// Define the escape key so the code can recognise it when input in terminal
#define ESCAPE 0x1B

int16_t automaticMode(int16_t xCoords);

int main(void)
{
	ES_Serial(0, "115200, 8, N, 1");
	int16_t xCoords = 0;
	bool autoMode = false;
	const int16_t horzAlign = 2;
	const int32_t pwmDisable = 0;
	const int32_t pwmEnable = 1;

	// LCD initialisation/control
	initSPI(); // This contains the SPI functions (see LCD_Display.h)
	initLCD(); // This function turns on and initialises the LCD
	setRotation(horzAlign);
	clearScreen();
	setCharConfig(ILI9341_DARKGREEN, 1, 1, ILI9341_BLACK, 1);
	moveCursor(20, 80);
	drawString("Ultrasonic Radar", 16);

	// Setup ultrasonic sensor
	setup_uart();

	// Setup ultrasonic sensor
	sensor_trig();
	sensor_echo();

	// Setup servo motor
	setup_servo_pwm();

	// Setup buzzer
	setup_buzzer_pwm();

	write_string("System Ready.\r\n");

	__enable_irq(); // Enable interrupts

	while (1)
	{
		int input = read_char();
		char angleStr[5];
		uint16_t servoDutyCycle;

		const int pwmOutput2Delay = 1000;
		const uint32_t pwmOutput2 = (1 << 2);
		const uint32_t idx = 0;

		const float minAngle = 0;
		const float maxAngle = 180.0;

		if (input != -1) {  // If a key was pressed

			if (input == ESCAPE) {
				write_string("KEYPRESS: esc\r\n");
				PWM0->ENABLE |= pwmOutput2Enable;
				msDelay(pwmOutput2Delay);
				PWM0->ENABLE &= ~pwmOutput2;
				autoMode = false; // Exit auto mode
			}

			else if (input == 'A' || input == 'a') {
				write_string("KEYPRESS: a -> Entering Auto Mode\r\n");
				autoMode = true;
			}

			else if (input == '<') {
				write_string("KEYPRESS: <\r\n");
				write_string("Min: ");

				while (1) {
					input = read_char();
					if (input == -1)
						continue;

					if (input == '\r' || input == '\n') {
						angleStr[idx] = '\0';  // Null-terminate
						break;
					}

					if (input >= '0' && input <= '9' && idx < sizeof(angleStr) - 1) {
						angleStr[idx++] = input;
						write_char(input);  // Echo typed char
					}
				}

				write_string("\r\n");

				int angle = atoi(angleStr);  // Convert to int

				if (angle < minAngle) angle = minAngle;
				if (angle > maxAngle) angle = maxAngle;

				dcMin = ((double)angle / maxAngle) * (dcMax - dcMin) + dcMin;

				write_string("Min angle set to: ");
				write_int(angle);
				write_string(" degrees\r\n");

				// Update PWM
				PWM0->_0_CTL = pwmDisable;
				PWM0->_0_CMPB = dcMin;
				PWM0->_0_CTL = pwmEnable;

				auto_mode = false;
			}

			else if (input == '>') {
				write_string("KEYPRESS: >\r\n");
				write_string("Max: ");

				while (1) {
					input = read_char();
					if (input == -1) continue;

					if (input == '\r' || input == '\n') {
						angleStr[idx] = '\0';  // Null-terminate
						break;
					}

					if (input >= '0' && input <= '9' && idx < sizeof(angleStr) - 1) {
						angleStr[idx++] = input;
						write_char(input);  // Echo typed char
					}
				}

				write_string("\r\n");

				int angle = atoi(angle_str);  // Convert to int

				if (angle < minAngle) angle = minAngle;
				if (angle > maxAngle) angle = maxAngle;

				dcMax = ((double)angle / maxAngle) * (dcMax - dcMin) + dcMin;

				write_string("Max angle set to: ");
				write_int(angle);
				write_string(" degrees\r\n");

				// Update PWM
				PWM0->_0_CTL = pwmDisable;
				PWM0->_0_CMPB = dcMax;
				PWM0->_0_CTL = pwmEnable;

				auto_mode = false;
			}

			else if (input == 'S' || input == 's') {
				write_string("KEYPRESS: s -> Set Angle\r\n");
				write_string("Angle: ");

				while (1) {
					input = read_char();
					if (input == -1) continue;

					if (input == '\r' || input == '\n') {
						angleStr[idx] = '\0';  // Null-terminate
						break;
					}

					if (input >= '0' && input <= '9' && idx < sizeof(angleStr) - 1) {
						angleStr[idx++] = input;
						write_char(input);  // Echo typed char
					}
				}

				write_string("\r\n");

				int angle = atoi(angleStr);  // Convert to int

				if (angle < minAngle) angle = minAngle;
				if (angle > maxAngle) angle = maxAngle;

				servoDutyCycle = ((double)angle / maxAngle) * (dcMax - dcMin) + dcMin;

				write_string("Servo motor angle set to: ");
				write_int(angle);
				write_string(" degrees\r\n");

				// Update PWM
				PWM0->_0_CTL = pwmDisable;
				PWM0->_0_CMPB = servoDutyCycle;
				PWM0->_0_CTL = pwmEnable;

				autoMode = false;
			}
		}

		if (autoMode) {
			xCoords = automaticMode(xCoords);  // Keep sweeping
			msDelay(100); // 100 miliseconds
		}
	}

	return (0);
}

int16_t automaticMode(int16_t xCoords)
{
	static uint16_t servoDutyCycle = 500;
	static int sweepDirection = 1;
	const uint32_t sweepStepSize = 10;
	const uint32_t pwmDisable = 0;
	const uint32_t pwmEnable = 1;
	const uint32_t anticlockwise = -1;
	const uint32_t clockwise = 1;

	// Get ultrasonic sensor measurements
	get_sensor_data();

	// Update direction at boundaries
	if (servoDutyCycle >= dcMax)
		sweepDirection = anticlockwise;
	else if (servoDutyCycle <= dcMin)
		sweepDirection = clockwise;

	// Adjust pulse width
	servoDutyCycle += sweepDirection * sweepStepSize;

	// Update PWM
	PWM0->_0_CTL = pwmDisable;
	PWM0->_0_CMPB = servoDutyCycle;
	PWM0->_0_CTL = pwmEnable;

	drawPixel(xCoords, (int16_t)distance, ILI9341_CYAN); // Draw a pixel on the lcd on the proportionate position to the measured distance

	xCoords += 1;
	return xCoords;
}

