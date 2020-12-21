#include <msp430.h>
#include <stdint.h>
#include "gpio.h"

void digitalWrite(uint8_t port, uint8_t bit, uint8_t value)
{
    uint8_t mask = 1 << bit;    // bit = 0 => mask = 1

    uint8_t * POUT = (uint8_t *) (0x200 + 0x002               // Endereco base
                                  + ( (port-1)/2 ) * 0x20     // Endereco das portas
                                  + ( (port+1)%2 )            // Endereco de PxOUT
                                 );
    if(value)
        *POUT |=  mask;
    else
        *POUT &= ~mask;
}


uint8_t digitalRead(uint8_t port, uint8_t bit)
{
    uint8_t mask = 1 << bit;    // bit = 0 => mask = 1

    // Retorna 0 se o pino estiver em LOW
    // Retorna um numero != 0 se o pino estiver em HIGH

    uint8_t * PIN = (uint8_t *) (0x200                       // Endereco base
                                 + ( (port-1)/2 ) * 0x20     // Endereco das portas
                                 + ( (port+1)%2 )            // Endereco de PxIN
                                );

    return *PIN & mask;         // Conteudo apontado por PIN e mask
}


void pinMode(uint8_t port, uint8_t bit, enum pinModes mode)
{
    // bit = numero do bit que quer modificar

    uint8_t mask = 1 << bit;    // bit = 0 => mask = 1

    P4OUT |= BIT7;

    switch (port) {
        case 1:
            if(mode == output)
                P1DIR |= mask;
            if(mode == input)
            {
                P1DIR &= ~mask; // direcao
                P1REN &= ~mask; // resistor
                P1OUT &= ~mask; // saida
            }
            if(mode == inPullUp)
            {
                P1DIR &= ~mask;
                P1REN |=  mask;
                P1OUT |=  mask;
            }
            if(mode == inPullDown)
            {
                P1DIR &= ~mask;
                P1REN |=  mask;
                P1OUT &= ~mask;
            }
            break;

        case 2:
            if(mode == output)
                P2DIR |= mask;
            if(mode == input)
            {
                P2DIR &= ~mask;
                P2REN &= ~mask;
                P2OUT &= ~mask;
            }
            if(mode == inPullUp)
            {
                P2DIR &= ~mask;
                P2REN |=  mask;
                P2OUT |=  mask;
            }
            if(mode == inPullDown)
            {
                P2DIR &= ~mask;
                P2REN |=  mask;
                P2OUT &= ~mask;
            }
            break;

        case 4:
            if(mode == output)
                P4DIR |= mask;
            if(mode == input)
            {
                P4DIR &= ~mask;
                P4REN &= ~mask;
                P4OUT &= ~mask;
            }
            if(mode == inPullUp)
            {
                P4DIR &= ~mask;
                P4REN |=  mask;
                P4OUT |=  mask;
            }
            if(mode == inPullDown)
            {
                P4DIR &= ~mask;
                P4REN |=  mask;
                P4OUT &= ~mask;
            }
            break;

        default:
            break;
    }
}
