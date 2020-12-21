#include <msp430.h> 
#include <stdint.h>
#include "libs/spi.h"
#include "libs/timer.h"

// Comunicacao escravo SPI

void configEscravo()
{
	UCB0CTLW0  = UCSWRST;                   // Reseta a interface
	UCB0CTLW0 |= 0x2100;                    // 3-wire Slave SPI, MSB-first
	P3SEL     |= BIT2 | BIT1 | BIT0;        // CLK  -> P3.2
	                                        // MISO -> P3.1
	                                        // MOSI -> P3.0
	UCB0CTLW0 &= ~UCSWRST;                  // Libera a interface

	UCB0IE     = UCTXIE | UCRXIE;           // Habilita as interrupcoes locais

	P1OUT &= ~BIT0;                         // Configura o LED vermelho
	P1DIR |=  BIT0;                         // como saida
}

#pragma vector = USCI_B0_VECTOR
__interrupt void SPI_SLAVE_ISR()
{
    switch (UCB0IV) {
        case 0x02:                          // RX -> Precisa ler o buffer de recepcao
            if(UCB0RXBUF == 0xCA)
                P1OUT |=   BIT0;
            if(UCB0RXBUF == 0xFE)
                P1OUT &=  ~BIT0;
            break;

        case 0x04:                          // TX -> Precisa escrever no buffer de transmissao
            UCB0TXBUF = 0xA5;
            break;

        default:
            break;
    }
}
