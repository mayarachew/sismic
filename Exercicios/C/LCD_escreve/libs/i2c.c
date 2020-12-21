#include <msp430.h>
#include <stdint.h>
#include "i2c.h"
#include "gpio.h"
#include "timer.h"

#define SDA 2,4             // SDA -> P2.5
#define SCL 2,5             // SCL -> P2.4

#define TCLK4 25,us         // TCLK/4 -> Frequencia de 10kHz

// I2C via USCI ----------------------------------------------

void i2cInit()
{   // Usa a interface B1 com os pinos P3.0 e P3.1
    UCB0CTLW0  = UCSWRST;                   // Reseta a interface
    UCB0CTLW0 |= 0x0F00 | UCSSEL__SMCLK;    // Configura como mestre

    // Frequencias para escrever no LCD
    //UCB0BRW    = 100;                       // Freq = 1M / 100 = 10k
                                            // LCD escreve de uma vez
    UCB0BRW    = 2000;                      // LCD escreve mais devagar

    P3SEL     |= (BIT0 | BIT1);             // Usa os pinos P3.{0,1}
    UCB0CTLW0 &= ~UCSWRST;                  // Libera o RST
}

uint8_t i2cSend(uint8_t addr, uint8_t * data, uint8_t nBytes)
{
    UCB0I2CSA = addr;                       // Configura o endereco
    UCB0CTLW0 |= UCTXSTT | UCTR;            // Transmite o endereco

    while(!(UCB0IFG & UCTXIFG));            // Espera o start acontecer
    UCB0TXBUF = * data++;                   // Escreve o primeiro byte para
    nBytes--;                               // destravar o ciclo de ACK

    // Verifica se o escravo responde (ACK) ou nao (NACK)

    while(UCB0CTLW0 & UCTXSTT);             // Aguarda ACK/NACK
    if(UCB0IFG & UCNACKIFG)                 // Se o escravo nao responder (NACK)
    {
        UCB0CTLW0 |= UCTXSTP;               // Se o escravo nao respondeu
        while(UCB0CTLW0 & UCTXSTP);         // eu peco um stop e espero o stop
        return 1;                           // ser enviado antes de retornar 1
    }

    while(nBytes--)                         // Se o escravo responder (ACK),
    {                                       // continuar a comunicacao enviando
        while(!(UCB0IFG & UCTXIFG));        // os proximos bytes ate acabarem
        UCB0TXBUF = * data++;               // os bytes acabarem
    }

    while(!(UCB0IFG & UCTXIFG));            // Espera o ultimo byte ser enviado
    UCB0CTLW0 |= UCTXSTP;                   // e eu peco um stop

    while(UCB0CTLW0 & UCTXSTP);             // Espera o stop ser enviado

    return 0;                               // Retorno = 0 significa sucesso
}

uint8_t i2cSendByte(uint8_t addr, uint8_t byte)
{
    return i2cSend(addr, &byte, 1);
}

uint8_t i2cGetByte(uint8_t addr)
{
    uint8_t byte;
    i2cGet(addr, &byte, 1);
    return byte;
}

//############################################################
// Bit-banging
// Camada fisica ---------------------------------------------

void i2cRelease(uint8_t port, uint8_t pin)
{
    // Botar pino no modo de entrada (flutuante)
    // Pino em estado de alta impedancia, dreno aberto
    pinMode(port, pin, inPullUp);
}

void i2cPullLow(uint8_t port, uint8_t pin)
{
    digitalWrite(port, pin, 0); // Botar o pino como saida
    pinMode(port, pin, output); // Puxa a linha para 0V
}

// Acoes de 1 bit --------------------------------------------

void i2cStart()
{
    // SCL vale 1, SDA vai pra 0
    // Espera um periodo do Clock
    wait(TCLK4);
    wait(TCLK4);
    i2cPullLow(SDA);
    wait(TCLK4);
    wait(TCLK4);
}

void i2cStop()
{
    i2cRelease(SDA);            // SCL vale 1, SDA vai pra 1
    wait(TCLK4);                // Espera um periodo do CLK/4
}

void i2cSendBit(uint8_t bitValue)
{
    if(bitValue)
        i2cRelease(SDA);        // Setar SDA
    else
        i2cPullLow(SDA);        // Zerar SDA

    wait(TCLK4);

    i2cRelease(SCL);            // Bate o Clock

    wait(TCLK4);
    wait(TCLK4);

    i2cPullLow(SCL);            // Termina de bater o Clock

    wait(TCLK4);
}

uint8_t i2cGetBit()
{
    uint8_t sample;

    i2cRelease(SDA);            // Solta SDA para o escravo
    wait(TCLK4);                // escrever na linha

    i2cRelease(SCL);            // Bate o Clock
    wait(TCLK4);

    sample = digitalRead(SDA);  // Amostra SDA na metade do Clock
    wait(TCLK4);

    i2cPullLow(SCL);            // Termina de bater o Clock
    wait(TCLK4);

    return sample;
}

// Acoes multibit --------------------------------------------

uint8_t i2cSendByteBB(uint8_t byte)
{
    uint8_t mask = BIT7;        // Mascara para filtrar os bits

    while(mask)
    {
        i2cSendBit(byte & mask);// Envia do bit mais significativo
        mask >>= 1;             // ate o bit menos significativo
    }

    return i2cGetBit();         // Ciclo de acknowledge
}

uint8_t i2cGetByteBB(uint8_t lastByte)
{
    uint8_t mask = BIT7;        // Mascara para filtrar os bits
    uint8_t receivedByte = 0;

    receivedByte = 0;

    while(mask)
    {
        if(i2cGetBit())
            receivedByte |= mask; // Byte recebe a mascara

        mask >>= 1;             // Ate o menos significativo
    }

    i2cSendBit(lastByte);       // Ciclo de acknowledge

    return receivedByte;
}

// Protocolo completo ----------------------------------------

void i2cSendBB(uint8_t addr, uint8_t * data, uint8_t nBytes)
{
    i2cStart();

    if(i2cSendByteBB(addr << 1 | 0)) // Manda o endereco e o bit de Write
    {
        i2cStop();                   // Caso o escravo nao responda
        return;
    }

    while(nBytes--)               // Enquanto tiverem bytes
    {
        i2cSendByteBB(*data++);   // Pegar o valor de data e
                                  // avancar o ponteiro
    }

    i2cStop();
}

void i2cGetBB (uint8_t addr, uint8_t * data, uint8_t nBytes)
{
    i2cStart();

    if(i2cSendByteBB(addr << 1 | 1)) // Manda o endereco e o bit de Read
       {
           i2cStop();                // Caso o escravo nao responda
           return;
       }

       while(nBytes--)               // Enquanto tiverem bytes
       {
           *data++ = i2cGetByteBB(!nBytes);
                                     // Quando nByte for 0, termina a comunicacao
       }
                                     // Recebeu todos os Bytes
    i2cStop();
}

