#include <msp430.h> 
#include <stdint.h>
#include "libs/spi.h"
#include "libs/timer.h"

// Comunicacao mestre SPI

void configEscravo();

void main()
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	UCA0CTLW0  = UCSWRST;                   // Reseta a interface
	UCA0CTLW0 |= 0x2900 | UCSSEL__SMCLK;    // 3-wire Master SPI, MSB-first
	UCA0BRW    = 10;                        // freq. clk = 1MHz/10 = 100kHz
	P2SEL     |= BIT7;                      // CLK  -> P2.7
	P3SEL     |= BIT3 | BIT4;               // MISO -> P3.4
	                                        // MOSI -> P3.3 //
	UCA0CTLW0 &= ~UCSWRST;                  // Libera a interface

	configEscravo();

	__enable_interrupt();

	// Para testar, conectar com o jumper:
	// CLK  -> P2.7 (mestre) com P3.2 (escravo)
    // MISO -> P3.4 (mestre) com P3.1 (escravo)
    // MOSI -> P3.3 (mestre) com P3.0 (escravo)

	while(1)
	{
        spiTransfer(0xCA);
        wait(1,sec);
	    spiTransfer(0xFE);
        wait(1,sec);
	}

	// Para testar somente o mestre, conectar
	// o MISO com o MOSI (P4.2 com P4.1)
	// e ver o resultado nos registradores

//	 uint8_t byte = 0xFF;
//	 while(1)
//	 {
//	      spiTransfer(0x00);
//	 }
}
