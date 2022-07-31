#include "test-tools.h"
#include <stdlib.h>

void initUART(UART_TypeDef *uart, uint32_t uart_divider) {
    uart->DIVIDER = uart_divider; //19200 baud
	uart->CONTROL1 = UART_CONTROL1_TE_M | UART_CONTROL1_RE_M 
		| UART_CONTROL1_UE_M; //TX, RX, Enable
    
    while (!(uart->FLAGS & UART_FLAGS_TEACK_M)
		&& !(uart->FLAGS & UART_FLAGS_REACK_M));
}

void writeByte(UART_TypeDef *uart, uint8_t byte) {
	uart->TXDATA = byte;
	while (!(uart->FLAGS & UART_FLAGS_TC_M));
}

void writeLine(UART_TypeDef *uart, uint8_t *line) {
	for (int i = 0; line[i] != '\0'; i++) {
		writeByte(uart, line[i]);
	}
}

void writeNum(UART_TypeDef *uart, int num, int base) {
    char buf[12];
    itoa(num, buf, base);
	for (int i = 0; buf[i] != '\0'; i++) {
		writeByte(uart, buf[i]);
	}
}

uint8_t readByte(UART_TypeDef *uart) {
	while (!(uart->FLAGS & UART_FLAGS_RXNE_M));
	return uart->RXDATA;
}

const uint32_t tickInMs = 3200;

void delay(uint16_t ms) {
	for (volatile uint32_t i = 0; i < (tickInMs * ms); i++);
}
