#include <msp430.h> 
#include <stdint.h>
#include "libs/adc.h"

// Conversao unica por interrupcao

volatile uint16_t adcResult;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Led
	P1DIR |= BIT0;

	P6SEL |=  BIT0 | BIT1;      // P6.1 e P6.0 estão no ADC [x/y]
	P6DIR &= ~BIT2;             // P6.2 e o switch do joystick
	P6REN |=  BIT2;             // com resistor
	P6OUT |=  BIT2;             // de pull up

	// Analog read ----
	ADC12CTL0 &= ~ADC12ENC;          // Precisa zerar para comecar a configuracao

	REFCTL0    =  0;                 // Desliga o modo de referencia

    ADC12CTL0  =  ADC12SHT0_2 |      // 26 batidas de clock
//                  ADC12MSC    |      // Multiplas conversoes por trigger
                  ADC12ON;           // Liga o quantizador

    ADC12CTL1  =  ADC12CSTARTADD_0 | // MEMO
                  ADC12SHS_0       | // Trigger: bit SC
                  ADC12SHP         | // Timer interno
                  ADC12SSEL_0      | // Clock de 5MHz
                  ADC12CONSEQ_0;     // Single convertion

    ADC12CTL2  =  ADC12TCOFF       | // Desliga o sensor de temperatura
                  ADC12RES_2;        // 12 bits de resolucao

    ADC12MCTL0 =  0;                 // Pino P6.0

    // ----------------

    ADC12IE    =  BIT0;

    __enable_interrupt();

    // Timer
    TA0CTL   = TASSEL__SMCLK | MC__UP;
    TA0CCR0  = 1000;
    TA0CCTL0 = CCIE;

	while(1)
	{
	    if(adcResult > 0x800)
	        P1OUT |=  BIT0;
	    else
	        P1OUT &= ~BIT0;
	}

}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_CCR0_ISR()
{
    ADC12CTL0 |=  ADC12ENC;      // Habilita o conversor
    ADC12CTL0 &= ~ADC12SC;       // Gera um flanco de subida em SC
    ADC12CTL0 |=  ADC12SC;
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC_ISR()
{
    adcResult = ADC12MEM0;
}
