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
    UCA1CTL0  = 0;
    UCA1CTL1 |= UCSSEL__SMCLK;    // D = 1048576 / 19200 / 16 = 3,41
    UCA1BRW   = 2;                // BRW = 2
    UCA1MCTL  = UCOS16 | UCBRF_4; // BRF = 4
    P4SEL    |= BIT4   | BIT5;    // Pinos P4.4 e P4.5

    UCA1CTL1 &= ~UCSWRST;         // Libera o RST

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

void uartBang(uint8_t port, uint8_t bit, uint8_t byte)
{
    pinMode(port, bit, output);

    TA0CTL  = TASSEL__ACLK | MC__UP;   // Configura interface
    TA0CCR0 = 27 - 1;                  // para frequencia de 1200Hz

    digitalWrite(port, bit, 0);        // Envia o bit de start

    while(!(TA0CCTL0 & CCIFG));        // Espera um periodo de bit
    TA0CCTL0 &= ~CCIFG;

    uint8_t mask = BIT0;

    while(mask)
    {
        digitalWrite(port, bit, byte & mask); // Envia bit apos bit

        while(!(TA0CCTL0 & CCIFG));     // Espera um periodo de bit
        TA0CCTL0 &= ~CCIFG;             // Zera a flag

        mask <<= 1;                     // Desloca a mascara
    }

    digitalWrite(port, bit, 1);

    while(!(TA0CCTL0 & CCIFG));         // Espera um periodo de bit
    TA0CCTL0 &= ~CCIFG;

    TA0CTL = 0;                         // Para o timer
}

uint8_t uartGang(uint8_t port, uint8_t bit)
{
    pinMode(port, bit, input);          // Bit de entrada

    while(digitalRead(port, bit));      // Espera o bit de start

    TA0CTL  = TASSEL__SMCLK | MC__UP;    // Espera TB/2
    TA0CCR0 = 27 - 1;                   // baudrate = 1200

    while(!(TA0CCTL0 & CCIFG));
    TA0CCTL0 &= ~CCIFG;

    if(digitalRead(port, bit))
        return 0;

    TA0CCR0 = 55 - 1;

    uint8_t mask = BIT0;
    uint8_t reply = 0;

    while(mask)
    {
        while(!(TA0CCTL0 & CCIFG));
        TA0CCTL0 &= ~CCIFG;

        if(digitalRead(port, bit))
            reply |= mask;

        mask <<= 1;
    }

    while(!(TA0CCTL0 & CCIFG));
    TA0CCTL0 &= ~CCIFG;

    if(digitalRead(port, bit))
        return reply;
    else
        return 0;
}

#pragma vector = USCI_A1_VECTOR
__interrupt void UART_ISR()
{
    rxBuff[rxWr] = UCA1RXBUF;

    rxWr = (rxWr + 1) & 0x0F;

    UCA1TXBUF = UCA1RXBUF;         // Registro de transmissao
}                                  // recebe do registro de recepcao
