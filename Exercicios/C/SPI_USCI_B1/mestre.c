#include <msp430.h> 
#include <stdint.h>
#include "libs/spi.h"
#include "libs/timer.h"

// Comunicacao mestre SPI

void configEscravo();
void daisyChain(uint8_t addr, uint8_t * data, uint8_t nBytes);

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

//        uint8_t LEDstate = 0xCA;
//        daisyChain(0x42, &LEDstate, 1);
//
//        wait(1, sec);
//
//        LEDstate = 0xFE;
//        daisyChain(0x42, &LEDstate, 1);
//
//        wait(1, sec);
	}
}

void daisyChain(uint8_t addr, uint8_t * data, uint8_t nBytes)
{
    spiTransfer(addr);
    spiTransfer(nBytes);
    while(nBytes--)
        spiTransfer(* data++);
}
