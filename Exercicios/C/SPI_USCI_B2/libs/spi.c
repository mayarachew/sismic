#include <msp430.h>
#include <stdint.h>
#include "libs/spi.h"
#include "libs/gpio.h"
#include "libs/timer.h"

uint8_t spiTransfer(uint8_t byte)
{
    while(!(UCA0IFG & UCTXIFG));    // Espera para poder enviar
    UCA0TXBUF = byte;               // Envia o byte e zera a flag
                                    // Ja da as 8 batidas do clock

    while(!(UCA0IFG & UCRXIFG));    // Espera o escravo enviar a resposta
    return UCA0RXBUF;               // Le o byte recebido e zera a flag
}
