// Code for terminal input and output

#include <TM4C129.h>
#include <stdbool.h>

// Defining used modules and pins
#define UART_PORT_0 (1 << 0)
#define UART_PIN_0_1 ((1 << 0) | (1 << 1))
#define UART_MODULE_0 (1 << 0)
#define RX_BUFFER_SIZE 64

// Global variables
volatile char c;
volatile char rxBuffer[RX_BUFFER_SIZE];
volatile int rxHead = 0;
volatile int rxTail = 0;

// Function prototypes
void setup_uart(void);
void write_char(char data);
void write_string(const char *str);
void write_int(uint32_t num);
int read_char(void);

// Function for initialising the GPIO and UART peripherals for utilisation
void setup_uart(void)
{
	const uint32_t pctlUART0Mask = 0xFF;
	const uint32_t pctlPA01Mask = 0x11;
	const uint32_t ctlMask = 0x301;
	const uint32_t ibrdMask = 8;
	const uint32_t fbrdMask = 44;
	const uint32_t lcrhMask = 0x60;
	const uint32_t ccMask = 0;

	// Setup GPIO peripheral for UART0
	SYSCTL->RCGCGPIO |= UART_PIN_0_1;
	while ((SYSCTL->PRGPIO & UART_PIN_0_1) == 0);

	GPIOA_AHB->AFSEL |= UART_PIN_0_1;
	GPIOA_AHB->PCTL &= ~pctlUART0Mask;
	GPIOA_AHB->PCTL |= pctlPA01Mask;
	GPIOA_AHB->DEN |= UART_PIN_0_1;
	GPIOA_AHB->AMSEL &= ~UART_PIN_0_1;

	// Setup UART0 peripheral
	SYSCTL->RCGCUART |= UART_MODULE_0;
	while ((SYSCTL->PRUART & UART_MODULE_0) == 0);

	UART0->CTL &= ~ctlMask;
	UART0->IBRD = ibrdMask;
	UART0->FBRD = fbrdMask;
	UART0->LCRH = lcrhMask;
	UART0->CC = ccMask;
	UART0->CTL |= ctlMask;

	UART0->IM |= (1 << 4);
	UART0->IM |= (1 << 5); // TXIM (optional)
	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_SetPriority(UART0_IRQn, 2); // IRQ 5 is UART0_IRQn
}

// Handler function for UART0 which runs on interrupts
void UART0_Handler(void)
{
	const uint32_t rxMask = (1 << 4);
	const uint32_t txMask = (1 << 5);

	if (UART0->MIS & rxMask) { // RX interrupt
		char received = UART0->DR & 0xFF;

		int nextHead = (rxHead + 1) % RX_BUFFER_SIZE;
		if (nextHead != rxTail) { // Buffer not full
			rxBuffer[rxHead] = received;
			rxHead = nextHead;
		}

		UART0->ICR |= rxMask; // Clear RX interrupt
	}

	if (UART0->MIS & txMask) {
		UART0->ICR |= txMask; // Clear TX interrupt
	}
}

// Function for reading user input char values in the terminal
int read_char(void)
{
	char data;

	if (rxHead == rxTail) return -1; // No data
	data = rxBuffer[rxTail];
	rxTail = (rxTail + 1) % RX_BUFFER_SIZE;

	return data;
}

// Function for writing character values to the terminal
void write_char(void)
{
	const uint32_t frUART0Mask = (1 << 5);

	while (UART0->FR & frUART0Mask);
	UART0->DR = data;
}

// Function for writing string values to the terminal
void write_string(const char *str)
{
	while (*str) write_char(*str++);
}


// Function for writing integers to the terminal
void write_int(uint32_t num)
{
	char buffer[10];
	int i = 0;

	if (num == 0)
		write_char('0');

	while (num > 0) {
		buffer[i++] = '0' + (num % 10);
		num /= 10;
	}

	while (i > 0)
		write_char(buffer[--i]);
}
