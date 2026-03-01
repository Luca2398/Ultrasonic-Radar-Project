#include "project.h"


#define ESCAPE 0x1B

volatile bool manual_mode = true;


int16_t Automatic_Mode(int16_t x_coords) {
	static uint16_t pulse_width = 500;
	static int direction = 1;

	// Get ultrasonic sensor measurements
	Get_Sensor_Data();

	// Update direction at boundaries
	if (pulse_width >= DC_MAX)
		direction = -1;
	else if (pulse_width <= DC_MIN)
		direction = 1;

	// Adjust pulse width
	pulse_width += direction * 10;

	// Update PWM
	PWM0->_0_CTL = 0;
	PWM0->_0_CMPB = pulse_width;
	PWM0->_0_CTL = 1;
	
	drawPixel(x_coords, (int16_t)DISTANCE, ILI9341_CYAN);
	
	//Write_Int(x_coords);
	x_coords += 1;
	return x_coords;
}

int main(void) {
	ES_Serial(0, "115200,8,N,1");
	int16_t x_coords = 0;

	// Setup LCD
	initSPI() ;	// These contain the SPI functions (See LCD_Display.h)
	initLCD() ; // This function turns on and initialises the LCD
	setRotation(2);
  clearScreen();
	setCharConfig(ILI9341_DARKGREEN,1,1,ILI9341_BLACK,1);
	moveCursor(20,80);
	drawString("Ultrasonic Radar",16);
	// Setup UART
	Setup_UART();
	
	// Setup ultrasonic sensor
	Sensor_Trig();
	Sensor_Echo();
	
	// Setup servo motor
	Setup_Servo_PWM();
	
	// Setup buzzer
	setup_Buzzer_PWM();
	
	// Setup potentiometer
	Setup_ADC();

	Write_String("System Ready.\r\n");

	__enable_irq();

	bool auto_mode = false;

	while (1) {
		int input = Read_Char();
		char angle_str[5];
		uint32_t idx = 0;
		uint16_t pulse_width;
		
		if (input != -1) {  // If a key was pressed
			
			if (input == ESCAPE) {
				Write_String("KEYPRESS: esc\r\n");
				PWM0->ENABLE |= (1 << 2);
				msDelay(1000);
				PWM0->ENABLE &= ~(1 << 2);
				auto_mode = false; // Exit auto mode
			}
			
			else if (input == 'A' || input == 'a') {
				Write_String("KEYPRESS: a -> Entering Auto Mode\r\n");
				auto_mode = true;
			}
			
			else if (input == '<') {
				Write_String("KEYPRESS: <\r\n");
				Write_String("Min: ");
				
				while (1) {
					input = Read_Char();
					if (input == -1) continue;

					if (input == '\r' || input == '\n') {
						angle_str[idx] = '\0';  // Null-terminate
						break;
					}

					if (input >= '0' && input <= '9' && idx < sizeof(angle_str) - 1) {
						angle_str[idx++] = input;
						Write_Char(input);  // Echo typed char
					}
				}
				
				Write_String("\r\n");

				int angle = atoi(angle_str);  // Convert to int

				if (angle < 0) angle = 0;
				if (angle > 180) angle = 180;
				
				DC_MIN = ((double)angle / 180.0) * (DC_MAX - DC_MIN) + DC_MIN;
				
				Write_String("Min angle set to: ");
				Write_Int(angle);
				Write_String(" degrees\r\n");
				
				// Update PWM
				PWM0->_0_CTL = 0;
				PWM0->_0_CMPB = DC_MIN;
				PWM0->_0_CTL = 1;
				
				auto_mode = false;
			}
			
			else if (input == '>') {
				Write_String("KEYPRESS: >\r\n");
				Write_String("Max: ");
		
				while (1) {
					input = Read_Char();
					if (input == -1) continue;

					if (input == '\r' || input == '\n') {
						angle_str[idx] = '\0';  // Null-terminate
						break;
					}

					if (input >= '0' && input <= '9' && idx < sizeof(angle_str) - 1) {
						angle_str[idx++] = input;
						Write_Char(input);  // Echo typed char
					}
				}
				
				Write_String("\r\n");

				int angle = atoi(angle_str);  // Convert to int

				if (angle < 0) angle = 0;
				if (angle > 180) angle = 180;
				
				DC_MAX = ((double)angle / 180.0) * (DC_MAX - DC_MIN) + DC_MIN;
				
				Write_String("Max angle set to: ");
				Write_Int(angle);
				Write_String(" degrees\r\n");
				
				// Update PWM
				PWM0->_0_CTL = 0;
				PWM0->_0_CMPB = DC_MAX;
				PWM0->_0_CTL = 1;
				
				auto_mode = false;
			}
			
			/*else if (input == 'M' || input == 'm') {
				Write_String("KEYPRESS: m -> Manual Mode\r\n");
				Write_String("Angle: ");
					
				uint32_t adc_value = Read_ADC();  // Convert to int
				
				int angle = ((adc_value / 4095) * 180.0);
				pulse_width = angle * (DC_MAX - DC_MIN) + DC_MIN;
				
				Write_String("Servo motor angle set to: ");
				Write_Int(angle);
				Write_String(" degrees\r\n");
				
				// Update PWM
				PWM0->_0_CTL = 0;
				PWM0->_0_CMPB = pulse_width;
				PWM0->_0_CTL = 1;
				
				auto_mode = false;
			}*/
			
			else if (input == 'S' || input == 's') {
				Write_String("KEYPRESS: s -> Set Angle\r\n");
				Write_String("Angle: ");

				while (1) {
					input = Read_Char();
					if (input == -1) continue;
					
					if (input == '\r' || input == '\n') {
						angle_str[idx] = '\0';  // Null-terminate
						break;
					}

					if (input >= '0' && input <= '9' && idx < sizeof(angle_str) - 1) {
						angle_str[idx++] = input;
						Write_Char(input);  // Echo typed char
					}
				}
				
				Write_String("\r\n");

				int angle = atoi(angle_str);  // Convert to int

				if (angle < 0) angle = 0;
				if (angle > 180) angle = 180;
				
				pulse_width = ((double)angle / 180.0) * (DC_MAX - DC_MIN) + DC_MIN;
				
				Write_String("Servo motor angle set to: ");
				Write_Int(angle);
				Write_String(" degrees\r\n");
				
				// Update PWM
				PWM0->_0_CTL = 0;
				PWM0->_0_CMPB = pulse_width;
				PWM0->_0_CTL = 1;
				
				auto_mode = false;
			}
		}

		if (auto_mode) {
			x_coords = Automatic_Mode(x_coords);  // Keep sweeping
			msDelay(100);
		}
	}

	return 0;
}
