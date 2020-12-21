#include <msp430.h> 
#include <stdint.h>
#include "libs/spi.h"

// Comunicacao Mestre/Escravo SPI

void configEscravo();

uint8_t buttonValue = 0x11;                 // Valor do botoes "Resposta do escravo"
uint8_t comando = 0x00;                     // Comando com a intensidade dos LEDs

// Montagem:
// CLK  -> P2.7 (mestre) com P3.2 (escravo)
// MISO -> P3.4 (mestre) com P3.1 (escravo)
// MOSI -> P3.3 (mestre) com P3.0 (escravo)

void main()
{
    WDTCTL = WDTPW | WDTHOLD;

    /* Configuracao do mestre */

    UCA0CTLW0  = UCSWRST;                   // Reseta a interface
    UCA0CTLW0 |= 0x2900 | UCSSEL__SMCLK;    // 3-wire Master SPI, MSB-first
    UCA0BRW    = 10;                        // freq. clk = 1MHz/10 = 100kHz
    P2SEL     |= BIT7;                      // CLK  -> P2.7
    P3SEL     |= BIT3 | BIT4;               // MISO -> P3.4
                                            // MOSI -> P3.3
    UCA0CTLW0 &= ~UCSWRST;

    /* Configuracao do escravo */

    configEscravo();

    __enable_interrupt();

    /* Loop infinito */

    while(1)
    {
        /*
         * Faz a leitura dos botões do escravo e
         * ajusta a luminosidade dos LEDs do escravo.
         */

        buttonValue = spiTransfer(0xFF);        // Mestre transmite o 0xFF por spi
        buttonValue = spiTransfer(0xFF);
        buttonValue = spiTransfer(0xFF);

        __delay_cycles(125000);                 // Tira rebote

        switch (buttonValue) {
            case 0x00:                          // SW1 e SW2 pressionados - 00000000
                if( (comando & 0xF0) == 0x40 )  // Se o LED verde estiver 100% aceso
                    comando -= 0x40;            // apagar o LED verde
                else comando += 0x10;           // Senao, aumentar a intensidade
                if( (comando & 0x0F) == 0x04 )  // Se o LED vermelho estiver 100% aceso
                    comando -= 0x04;            // apagar o LED vermelho
                else comando += 0x01;           // Senao, aumentar a intensidade
                break;

            case 0x01:                          // SW2 pressionado - 00000001
                if( (comando & 0xF0) == 0x40 )  // Se o LED verde estiver 100% aceso
                    comando -= 0x40;            // apagar o LED verde
                else comando += 0x10;           // Senao, aumentar a intensidade
                break;

            case 0x10:                          // SW1 pressionado - 00010000
                if( (comando & 0x0F) == 0x04 )  // Se o LED vermelho estiver 100% aceso
                    comando -= 0x04;            // apagar o LED vermelho
                else comando += 0x01;           // Senao, aumentar a intensidade
                break;

            case 0x11:                          // SW1 e SW2 nao pressionados - 00010001
                break;

            default:
                break;
        }

        spiTransfer(comando);                   // Transmite o comando atualizado

        __delay_cycles(30000);
    }

}
