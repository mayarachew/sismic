#include <msp430.h>
#include <stdint.h>
#include "libs/i2c.h"
#include "libs/timer.h"
#include "libs/lcd.h"

uint8_t lcdAddr = 0x3F;

#define BT BIT3
#define EN BIT2
#define RW BIT1

void lcdWriteNibble(uint8_t nibble, uint8_t rs)
{                                   // BT   EN   RW  RS
    i2cSendByte(lcdAddr, nibble << 4 | BT | 0  | 0 | rs);
    // Tambem pode inserir um wait aqui para diminuir a velocidade
    i2cSendByte(lcdAddr, nibble << 4 | BT | EN | 0 | rs);
    i2cSendByte(lcdAddr, nibble << 4 | BT | 0  | 0 | rs);
}


void lcdWriteByte(uint8_t byte, uint8_t rs)
{
    // 2 repeticoes
    // Pega a parte mais significativa do byte
    lcdWriteNibble(byte >> 4,   rs); // Desloca os bits para a direita
    lcdWriteNibble(byte & 0x0F, rs); // Filtra os 4 bits menos significativos
}

void print(char * str)
{
    while(*str)
        lcdWriteByte(* str++, 1);
}

void lcdInit()
{
    if(i2cSendByte(lcdAddr, 0)) // Descobrindo o endereco
        lcdAddr = 0x27;

    // Entra em modo 8 bits
    lcdWriteNibble(0x3, 0);
    lcdWriteNibble(0x3, 0);
    lcdWriteNibble(0x3, 0);

    // Entra em modo 4 bits
    lcdWriteNibble(0x2, 0);

    // Liga o cursor e o display sem fazer o cursor piscar
    lcdWriteByte(0x0C,  0);

    // Liga o cursor e o display e faz o cursor piscar
    //lcdWriteByte(0x0F,  0);

    // Comandos: return home and clear display
    lcdWriteByte(0x02,  0);
    lcdWriteByte(0x01,  0);
}
