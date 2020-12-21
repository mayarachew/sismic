#include <msp430.h> 
#include <stdint.h>
#include "libs/spi.h"
#include "libs/timer.h"

// Comunicacao escravo SPI

uint8_t myAddr = 0x42;
uint8_t byteCounter;

enum modes { echo_addr         = 0x10,
             echo_getMsgSize   = 0x11,
             echo_ignoreBytes  = 0x12,
             slave_getMsgSize  = 0x20,
             slave_task        = 0x21
            } mode;

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

	mode = echo_addr;
}

#pragma vector = USCI_B0_VECTOR
__interrupt void SPI_SLAVE_ISR()
{
    switch (UCB0IV) {
        case 0x02:                          // RX -> Precisa ler o buffer de recepcao
            switch (mode) {
                // Modo echo
                case echo_addr:
                    if(UCB0RXBUF == myAddr)
                        mode = slave_getMsgSize;
                    else
                        mode = echo_getMsgSize;
                    break;

                case echo_getMsgSize:
                    byteCounter = UCB0RXBUF;
                    mode = echo_ignoreBytes;
                    break;

                case echo_ignoreBytes:
                    if(--byteCounter == 0)
                        mode = echo_addr;
                    break;

                case slave_getMsgSize:
                    byteCounter = UCB0RXBUF;
                    mode = slave_task;
                    break;

                // Modo escravo
                case slave_task:
                    if(UCB0RXBUF == 0xCA)
                        P1OUT |=  BIT0;
                    if(UCB0RXBUF == 0xFE)
                        P1OUT &= ~BIT0;
                    break;

                default:
                    break;
            }

        case 0x04:                          // TX -> Precisa escrever no buffer de transmissao
            // Modo echo
            if( (mode & 0xF0) == 0x10 )
                UCB0TXBUF = UCB0RXBUF;
            else
                UCB0TXBUF = 0x5A;
            // Modo escravo
            break;

        default:
            break;
    }
}
