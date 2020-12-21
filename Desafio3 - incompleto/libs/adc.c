#include <msp430.h>
#include <stdint.h>

void adcInit(void)
{
    ADC12CTL0 &= ~ADC12ENC;          // Zera para configuraçao

    P6SEL |=  (BIT0 | BIT1 | BIT2 | BIT3); // P6.0 e P6.1, P6.2, P6.3 no ADC

    ADC12CTL0 = ADC12SHT0_3      |   // Batidas de clock
                ADC12MSC         |   // Multiplas conversoes por trigger
                ADC12ON;             // Liga o quantizador

    ADC12CTL1 = ADC12CSTARTADD_0 |   // MEM0
                ADC12SHS_0       |   // Trigger: bit SC
                ADC12SHP         |   // Timer interno
                ADC12SSEL_0      |
                ADC12CONSEQ_1;       // Modo sequencia

    ADC12CTL2  = ADC12TCOFF      |   // Desliga o sensor de temperatura
                 ADC12RES_1;         // 10 bits de resolucao

    ADC12MCTL0 = ADC12INCH_0;        // (P6.0) A0 = canal 0

    ADC12MCTL1 = ADC12INCH_1;        // (P6.1) A1 = canal 1

    ADC12MCTL2 = ADC12INCH_2;        // (P6.2) A1 = canal 2

    ADC12MCTL3 = ADC12INCH_3     |   // (P6.3) A1 = canal 3
                 ADC12EOS;           // Fim de sequencia

    ADC12IFG = 0;                    // Zera a flag

    ADC12CTL0 |= ADC12ENC;           // Fim da configuracao

    ADC12IE = ADC12IE1;
}
