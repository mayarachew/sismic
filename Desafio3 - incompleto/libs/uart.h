#ifndef _UART_H
#define _UART_H

#include <msp430.h>
#include <stdint.h>

#define RX 4,5
#define TX 4,4

void uartInit();

void uartPrint(char * str);
void uartSend(uint8_t byte);
uint8_t uartGet(char * data);

#endif //_UART_H
