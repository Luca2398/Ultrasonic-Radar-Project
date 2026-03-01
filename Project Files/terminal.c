#include <TM4C129.h>
#include <stdbool.h>

#define UART_PORT (1 << 0)
#define UART_PIN ((1 << 0) | (1 << 1))
#define UART_MODULE (1 << 0)

#define RX_BUFFER_SIZE 64

volatile char rxBuffer[RX_BUFFER_SIZE];
volatile int rxHead = 0;
volatile int rxTail = 0;


volatile char c;

void Setup_UART(void);
int Read_Char(void);
void Write_Char(char data);
void Write_String(const char *str);
void Write_Int(uint32_t num);

void Setup_UART(void) {
	SYSCTL->RCGCGPIO |= UART_PIN;
	while ((SYSCTL->PRGPIO & UART_PIN) == 0);
	
	GPIOA_AHB->AFSEL |= UART_PIN;
	GPIOA_AHB->PCTL &= ~0xFF;
	GPIOA_AHB->PCTL |= 0x11;
	GPIOA_AHB->DEN |= UART_PIN;
	GPIOA_AHB->AMSEL &= ~UART_PIN;
	
	SYSCTL->RCGCUART |= UART_MODULE;
	while ((SYSCTL->PRUART & UART_MODULE) == 0);
	
	UART0->CTL &= ~0x301;
	UART0->IBRD = 8;
	UART0->FBRD = 44;
	UART0->LCRH = 0x60;
	UART0->CC = 0;
	UART0->CTL |= 0x301;
	
	UART0->IM |= (1 << 4); 
	UART0->IM |= (1 << 5); // TXIM (optional)
	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_SetPriority(UART0_IRQn, 2); // IRQ 5 is UART0_IRQn
}

void UART0_Handler(void) {
	if (UART0->MIS & (1 << 4)) { // RX interrupt
		char received = UART0->DR & 0xFF;

		int nextHead = (rxHead + 1) % RX_BUFFER_SIZE;
		if (nextHead != rxTail) { // Buffer not full
			rxBuffer[rxHead] = received;
			rxHead = nextHead;
		}

		UART0->ICR |= (1 << 4); // Clear RX interrupt
	}

	if (UART0->MIS & (1 << 5)) {
		UART0->ICR |= (1 << 5); // Clear TX interrupt
	}
}

int Read_Char(void) {
    if (rxHead == rxTail) return -1; // No data
    char data = rxBuffer[rxTail];
    rxTail = (rxTail + 1) % RX_BUFFER_SIZE;
    return data;
}

void Write_Char(char data) {
	while (UART0->FR & (1 << 5));
	UART0->DR = data;
}

void Write_String(const char *str) {
	while (*str) Write_Char(*str++);
}

void Write_Int(uint32_t num) {
	char buffer[10];
	int i = 0;
	
	if (num == 0)
		Write_Char('0');
	
	while (num > 0) {
		buffer[i++] = '0' + (num % 10);
		num /= 10;
	}
	
	while (i > 0)
		Write_Char(buffer[--i]);
}