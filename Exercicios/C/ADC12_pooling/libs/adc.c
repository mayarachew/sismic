#include <msp430.h>
#include <stdint.h>

uint8_t analogRead(uint8_t pin)
{
    P6SEL |= 1 << pin;               // Desloca o pino

    ADC12CTL0 &= ~ADC12ENC;          // Precisa zerar para comecar a configuracao

    ADC12CTL0  =  ADC12SHT0_3 |      // 32 batidas de clock
                  ADC12ON;           // Liga o quantizador

    ADC12CTL1  =  ADC12CSTARTADD_0 | // MEMO
                  ADC12SHS_0       | // Trigger: bit SC
                  ADC12SHP         | // Timer interno
                  ADC12SSEL_0      | // Clock de 5MHz
                  ADC12CONSEQ_0;     // Single convertion

    ADC12CTL2  =  ADC12TCOFF       | // Desliga o sensor de temperatura
                  ADC12RES_2;        // 12 bits de resolucao

    ADC12MCTL0 =  pin & 0x07;        // Configura a entrada com os
                                     // 7 bits menos significativos

    ADC12CTL0 |=  ADC12ENC | ADC12SC;

    while(!(ADC12IFG & BIT0));       // Espera a flag

    return ADC12MEM0;                // Leitura
}
