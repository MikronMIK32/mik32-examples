#ifndef HEADER_TESTTOOLS_H
#define HEADER_TESTTOOLS_H

#include <mcu32_memory_map.h>
#include <uart.h>

/*
* uart_divider = 32000000 / baudrate
*/
void initUART(UART_TypeDef *uart, uint32_t uart_divider);

void writeByte(UART_TypeDef *uart, uint8_t byte);

void writeLine(UART_TypeDef *uart, uint8_t *line);

void writeNum(UART_TypeDef *uart, int num, int base);

uint8_t readByte(UART_TypeDef *uart);

void delay(uint16_t ms);

#endif