#include <msp430.h>
#include <stdint.h>
#include "libs/gpio.h"
#include "libs/uart.h"

uint8_t rxBuff[16];
uint8_t rxWr, rxRd;

void uartInit()
{
    UCA1CTL1 = UCSWRST;           // Habilita o reset para iniciar
                                  // a configuracao
    P4SEL    |= (BIT4 | BIT5);    // P4.4 e P4.5

    UCA1CTL0  = 0;
    UCA1CTL1 |= UCSSEL__SMCLK;    // D = 1048576 / 19200 / 16 = 3,41

    UCA1BRW   = 2;                // BRW = 2

    UCA1MCTL  = UCOS16 | UCBRF_4; // BRF = 4

    UCA1CTL1 &= ~UCSWRST;         // Fim da configuracao

    UCA1IE    = UCRXIE;           // Habilita interrupcao

    __enable_interrupt();
}

void uartSend(uint8_t byte)
{
    while(!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = byte;
}

uint8_t uartGet(char * data)
{
    if(rxWr == rxRd)
    {
        return 1;
    } else
    {
        while(rxRd != rxWr)
        {
            *data++ = rxBuff[rxRd];
            rxRd = (rxRd + 1) & 0x0F;
        }
        return 0;
    }
}

void uartPrint(char * str)
{
    while(*str)
        uartSend(*str++);
}

#pragma vector = USCI_A1_VECTOR
__interrupt void UART_ISR()
{
    rxBuff[rxWr] = UCA1RXBUF;

    rxWr = (rxWr + 1) & 0x0F;

    UCA1TXBUF = UCA1RXBUF;         // Registro de transmissao
}                                  // recebe do registro de recepcao
