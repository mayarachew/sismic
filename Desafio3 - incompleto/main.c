// Mayara Chew Marinho
// 18/0025210

#include <msp430.h> 
#include <stdint.h>
#include "libs/uart.h"
#include "libs/i2c.h"
#include "libs/adc.h"

// Montagem ---------------------------------------------

// JOYSTICK:
// GND em GND
// +5V em 3V3
// VRx no P6.0
// VRy no P6.1

// ------------------------------------------------------

// Nao foi possivel visualizar o funcionamento do uart
// por limitacoes tecnicas, dessa forma, o codigo foi
// feito com base na teoria vista

volatile uint16_t dadox = 0, dadoy = 0, dadoz = 0, dadow = 0;
char data;

uint8_t hexaConversor(uint8_t n, uint16_t dado)
{
    volatile uint16_t h_num;

    h_num = (dado >> n) & 0x0F; // Filtra os bits

    if(h_num < 0x0A){           // Converte para hexadecimal
        h_num += 0x30;
    }
    else{
        h_num += 0x37;
    }

    return h_num;               // Retorna o valor em hexadecimal
}

void imprimeHexa(uint16_t dado) // Converte e imprime o valor em hexa
{
    volatile char h_num;

    h_num = hexaConversor(12, dado);
    uartPrint(h_num);

    h_num = hexaConversor(8, dado);
    uartPrint(h_num);

    h_num = hexaConversor(4, dado);
    uartPrint(h_num);

    h_num = hexaConversor(0, dado);
    uartPrint(h_num);
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;        // stop watchdog timer

    P6DIR &= ~BIT2;                  // P6.2 = switch do joystick
    P6REN |=  BIT2;                  // com resistor
    P6OUT |=  BIT2;                  // de pull up

    i2cInit();                       // Configura I2C

    adcInit();                       // Configura ADC

    ADC12CTL0 |=  ADC12ENC;          // Habilita o conversor
    ADC12CTL0 &= ~ADC12SC;           // Gera um flanco de subida em SC
    ADC12CTL0 |=  ADC12SC;

    while(!(ADC12IFG & ADC12IFG1));
    dadox = ADC12MEM0;               // Tensão do canal 0
    dadoy = ADC12MEM1;               // Tensão do canal 1
    dadoz = ADC12MEM2;               // Tensão do canal 2
    dadow = ADC12MEM3;               // Tensão do canal 3

    __enable_interrupt();            // Habilita interrupcoes

    uartInit();                      // Configura UART

    __delay_cycles(10000);

    uartPrint("\033[2J\r");

    while(1)
    {
        uartPrint("Oscilento: versao 0.1 \n\r");
        uartPrint("Selecione a entrada: [0:3], 't' p/ todas \n\r");

        uartPrint("Qual modo de amostragem? \n\r");

        uartPrint("0: Manual - pressione 'a' p/ amostrar \n\r");
        uartPrint("1: Periodico - 16 amostras por segundo \n\r");

        uartPrint("Pressione 's' a qualquer momento para sair \n\r");

        while(uartGet(&data));

        if(data == 's')
        {
            return 0;
        }

        __delay_cycles(10000);
    }

}
