#include <msp430.h>
#include <stdint.h>
#include "timer.h"

void wait(uint16_t time, timeUnit_t unit)
{
    if(unit == us)                  // Microssegundo
    {
        TA2CTL = TASSEL__SMCLK |    // Configura o passo para 1us
                 MC__UP        |    // Modo UP, ja que o usuario
                 TACLR;             // vai configurar o teto de contagem

        TA2CCR0 = time - 1;         // Tempo exato

        while(!(TA2CCTL0 & CCIFG));
        TA2CCTL0 &= ~CCIFG;

        TA2CTL = MC__STOP;          // Para o timer

        return;
    }
    if(unit == sec)                     // Microssegundo
    {
        TA2CTL = TASSEL__ACLK  |    // Configura o passo
                 MC__UP        |    // Modo UP, ja que o usuario
                 TACLR;             // vai configurar o teto de contagem

        TA2CCR0 = (time * (0x8000 - 1));

        while(!(TA2CCTL0 & CCIFG));
        TA2CCTL0 &= ~CCIFG;

        TA2CTL = MC__STOP;          // Para o timer

        return;
    }
}
