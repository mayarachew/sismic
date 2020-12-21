#include <msp430.h>
#include <stdint.h>
#include "libs/spi.h"

// Escravo SPI

#define USCI_SPI_RXIFG     0x02
#define USCI_SPI_TXIFG     0x04

void configEscravo()
{
    /* Configuração dos LEDs e botões */

    P1OUT &= ~BIT0;                         // Configura o LED vermelho (P1.0)
    P1DIR |=  BIT0;                         // como saida

    P4OUT &= ~BIT7;                         // Configura o LED verde (P4.7)
    P4DIR |=  BIT7;                         // como saida

    P2DIR &= ~BIT1;                         // Configura SW1 (P2.1)
    P2REN |=  BIT1;                         // como entrada
    P2OUT |=  BIT1;                         // com resistor de pull up

    P1DIR &= ~BIT1;                         // Configura SW2 (P1.1)
    P1REN |=  BIT1;                         // como entrada
    P1OUT |=  BIT1;                         // com resistor de pull up

    /* Configuração da interface como escravo */

    UCB0CTLW0  = UCSWRST;                   // Reseta a interface
    UCB0CTLW0 |= 0x2100;                    // 3-wire Slave SPI, MSB-first
    P3SEL     |= BIT2 | BIT1 | BIT0;        // CLK  -> P3.2
                                            // MISO -> P3.1
                                            // MOSI -> P3.0
    UCB0CTLW0 &= ~UCSWRST;                  // Libera a interface
    UCB0IE     =  UCTXIE | UCRXIE;          // Habilita as interrupcoes locais

    /* Configuração do timer para geração da PWM */

    TA0CTL    = MC__UP | TASSEL__ACLK | TAIE | TACLR;
                                            // ACLK @2^15 Hz

    TA0CCR0   = 110 - 1;                    // Limiar maximo de 298 Hz

    TA0CCR1   = 1;                          // Limiar minimo LED vermelho
    TA0CCR2   = 1;                          // Limiar minimo LED verde

    TA0CCTL1 = CCIE;                        // Habilita interrupcoes no canal 1
    TA0CCTL2 = CCIE;                        // Habilita interrupcoes no canal 2
}

/* Interrupções */

#pragma vector = USCI_B0_VECTOR
__interrupt void SPI_SLAVE_ISR()
{
    switch (UCB0IV) {
        case USCI_SPI_RXIFG:                // RX -> Precisa ler o buffer de recepcao
            switch (UCB0RXBUF & 0xF0) {     // LED verde
                case 0x00:
                    TA0CCR2 = 1;            // Indicador para duty cycle = 0%
                    break;

                case 0x10:
                    TA0CCR2 = 27.5 - 1;     // Duty cycle = 25%
                    break;

                case 0x20:
                    TA0CCR2 = 55 - 1;       // Duty cycle = 50%
                    break;

                case 0x30:
                    TA0CCR2 = 82.5 - 1;     // Duty cycle = 75%
                    break;

                case 0x40:
                    TA0CCR2 = 109;          // Duty cycle = 100%
                    break;

                default:
                    break;
            }

            switch (UCB0RXBUF & 0x0F) {     // LED vermelho
                case 0x00:
                    TA0CCR1 = 1;            // Indicador para duty cycle = 0%
                    break;

                case 0x01:
                    TA0CCR1 = 27.5 - 1;     // Duty cycle = 25%
                    break;

                case 0x02:
                    TA0CCR1 = 55 - 1;       // Duty cycle = 50%
                    break;

                case 0x03:
                    TA0CCR1 = 82.5 - 1;     // Duty cycle = 75%
                    break;

                case 0x04:
                    TA0CCR1 = 109;          // Duty cycle = 100%
                    break;

                default:
                    break;
            }
            break;

        case USCI_SPI_TXIFG:                // TX -> Precisa escrever no buffer de transmissao
            if(UCB0RXBUF == 0xFF)           // Se o mestre pedir o estado dos botoes
            {
                UCB0TXBUF = 0x00;
                if(P2IN & BIT1)             // SW1 nao pressionado
                    UCB0TXBUF += 0x01;
                if(P1IN & BIT1)             // SW2 nao pressionado
                    UCB0TXBUF += 0x10;
            }
            break;

        default:
            break;
    }
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TA0_CCRN_ISR()
{
    switch (TA0IV) {
        case 2:
            // CCR1
            P1OUT &= ~BIT0;                 // Apaga o LED vermelho em CCR1
            break;

        case 4:
            // CCR2
            P4OUT &= ~BIT7;                 // Apaga o LED verde em CCR2
            break;

        case 14:
            // CCR0
            if(TA0CCR1 != 1)                // Se o LED vermelho nao estiver apagado
                P1OUT |= BIT0;              // Liga o LED
            if(TA0CCR2 != 1)                // Se o LED verde nao estiver apagado
                P4OUT |= BIT7;              // Liga o LED
            break;

        default:
            break;
    }
}


