#ifndef _UART_H
#define _UART_H

#include <stdint.h>

#define RX 4,5
#define TX 4,4

void uartInit();

void uartSend(uint8_t byte);
uint8_t uartGet(char * data);
void uartPrint(char * str);

void uartBang(uint8_t port, uint8_t pin, uint8_t byte);
uint8_t uartGang(uint8_t port, uint8_t pin);

#endif //_UART_H
