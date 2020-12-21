#include <msp430.h>
#include <stdint.h>
#include "libs/spi.h"
#include "libs/gpio.h"
#include "libs/timer.h"

uint8_t spiTransferBB(uint8_t byte, uint8_t CPOL, uint8_t CPHA)
{
    // Configuracao dos pinos
    pinMode(STE,  output);
    pinMode(MOSI, output);
    pinMode(SCLK, output);
    pinMode(MISO, input );

    // Mascara de comunicacao para filtrar um bit por vez
    uint8_t mask = BIT7;
    uint8_t reply = 0;                      // Variavel para guardar as respostas

    digitalWrite(STE, 1);

    if(CPHA)
        CPOL = !CPOL;

    while(mask)
    {
        digitalWrite(MOSI, byte & mask);    // Envia o bit apontado pela mascara
        if( (!CPHA) || (mask |= BIT7) )     // BIT7 eh o MSB
        {
            digitalWrite(SCLK, !CPOL);      // Bate o clock (flanco de subida)
        }

        wait(TCLK2);

        if(digitalRead(MISO))               // Le o valor do MISO
            reply |= mask;                  // e coloco na variavel de retorno

        digitalWrite(SCLK, CPOL);           // Bate o clock (flanco de descida)

        wait(TCLK2);

        mask >>= 1;                         // Pega o proximo bit (BIT7, BIT6, BIT5,...)
    }

    if(CPHA)
        digitalWrite(SCLK, !CPOL);          // Leva a linha para o estado ocioso 0

    digitalWrite(STE, 0);

    return reply;
}
