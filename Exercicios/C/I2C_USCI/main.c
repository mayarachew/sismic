#include <msp430.h>
#include <stdint.h>
#include "libs/timer.h"
#include "libs/i2c.h"
#include "libs/gpio.h"

// Implementar comunicacao I2C usando as interfaces dedicadas
// --> Usando interrupcao (escravo)
// --> Usando poolling (mestre)

uint8_t echo;

int main()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // 1) Setar o bit de RST
    // 2) Inicializar os registros de configuracao
    // 3) Configurar os pinos
    // 4) Limpar o RST
    // 5) Habilitar interrupcoes

    // Configurar um escravo por interrupcao

    UCB1CTL1  =   UCSWRST;           // Seta o bit RST e zera os demais campos

    UCB1CTL0  =   UCMODE_3 | UCSYNC; // Escravo i2c
    UCB1I2COA =   UCGCEN   | 10;     // de endereco 10
    P4SEL    |=  (BIT1     | BIT2);  // P4.1 e P4.2 funcionando como i2c
    P4DIR    &= ~(BIT1     | BIT2);
    P4REN    |=  (BIT1     | BIT2);  // Resistor de pull up
    P4OUT    |=  (BIT1     | BIT2);

    UCB1CTL1 &=   UCSWRST;           // Zera o bit RST

    UCB1IE    = UCRXIE | UCTXIE;     // UCRXIE acontece quando o buffer de recepcao
                                     // estiver cheio e for necessario esvaziar

                                     // UCTXIE acontece quando for necessario transmitir algo

    __enable_interrupt();            // Habilita interrupcao

    uint8_t data[] = {0xCA, 0xFE};

    while(1)
    {
        i2cSendBB(11, data, 2);
        i2cSendBB(10, data, 2);
        i2cGetBB (10, data, 1);
    }
}

#pragma vector = USCI_B1_VECTOR
__interrupt void I2C_ISR2()         // Interrupcao agrupada
{
    switch (UCB1IV) {
        case USCI_I2C_UCRXIFG:      // Quando recebe uma interrupcao de RX
            echo = UCB1RXBUF;       // Recebe o byte do buffer e zera a flag de RX
            break;                  // Esvazia o buffer de recepcao

        case USCI_I2C_UCTXIFG:      // Quando recebr uma interrupcao de TX
            UCB1TXBUF = echo;       // Envia o ultimo byte recebido e zera a flag de TX
            break;

        default:
            break;
    }
}
