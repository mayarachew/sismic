#include <msp430.h> 
#include <stdint.h>
#include "libs/timer.h"

#define REDBIT BIT0;

// Exercicio 1 - Fazer o LED piscar a cada 2 segundos
// ACLK ate 2 segundos
// MSCLK microssegundos

void main()
{
	WDTCTL = WDTPW | WDTHOLD;	    // stop watchdog timer

	TA0CTL = TASSEL__ACLK |         // Configura o timer
	         MC__UP       |         // com ref. ACLK @32768Hz
	         TACLR;                 // modo up e comeca do zero

	TA0CCR0 = 0x8000 - 1;           // Define limiar para 1 segundo e
	TA0CCR1 = 0x6000;               // subtrai 1 para nao contar o 0

	P1OUT &= ~REDBIT;               // Configura o LED vermelho
	P1DIR |=  REDBIT;               // como saida

	while(1)
	{
	    while(!(TA0CCTL1 & CCIFG)); // Trava enquanto TAOCCTL1 for 0
	    TA0CCTL1 &= ~CCIFG;         // Nega a FLAG

        P1OUT |=  REDBIT;           // Liga o LED vermelho

	    while(!(TA0CCTL0 & CCIFG)); // Trava enquanto TAOCCTL0 for 0
	    TA0CCTL0 &= ~CCIFG;         // Nega a FLAG

	    P1OUT &= ~REDBIT;           // Desliga o LED vermelho
	    wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
        wait(60000, us);
	}
}
