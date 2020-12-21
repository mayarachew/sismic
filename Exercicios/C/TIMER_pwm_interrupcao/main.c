#include <msp430.h> 

// Gerar PWM por software usando interrupcoes

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	    // stop watchdog timer
	P1DIR |= BIT0;

	TA0CTL   = MC__UP | TASSEL__ACLK | TAIE;
	TA0CCR0  = 0x0080 - 1;
	TA0CCR1  = 0x0010 - 1;
	TA0CCTL1 = CCIE;                // Habilita o CCIE do canal 1

	// Setar o bit GIE dentro de SR
	__enable_interrupt();

	while(1)
	{

	}
	
	return 0;
}


#pragma vector = TIMER0_A1_VECTOR   // Valor da entrada na tabela
                                    // Obrigatorio para interrupcoes
__interrupt void TA0_CCRN_ISR()
{
    switch (TA0IV) {
        case 2:
            // CCR1 ...
            P1OUT ^= BIT0;         // Zera o bit quando chega em CCR1
            volatile unsigned int x = 60000;

                while(x > 0)
                {
                    x--;
                }
            break;
        case 14:
            // Overflow ...
            P1OUT |= BIT0;          // Seta o bit quando chega em overflow
            break;
        default:
            break;
    }
}

