# Ultrasonic-Radar-Project
Assignment submission for 2303ENG embedded systems course.
The project consists of a radar system that is controlled by a Tiva TM4C1294NCPDT microcontroller which uses multiple sensors including an ultrasonic sensor to create a map of the surrounding environment.

<img src="https://github.com/user-attachments/assets/cc7227a6-610a-4ada-be43-ddb6e014c250" width="500" />

Image of completed ultrasonic radar.


## Components
The ultrasonic radar uses the following components:
 - SRF04 ultrasonic sensor
 - Servo motor
 - Piezo buzzer
 - Potentiometer
 - SPI driven colour LCD
 - LED (red)


## Specification
 - The ultrasonic radar has two mapping modes, which includes automatic, which automatically scans the surrounding environment by incrementally cycling around the range of the servo motor continuously until manually stopped, and manual mode, where the user can set a specified angle in degrees (within the available range on the servo motor) for the servo motor to get a scan in the set direction.
 - Ultrasonic sensor detects and measures distances to surrounding objects.
