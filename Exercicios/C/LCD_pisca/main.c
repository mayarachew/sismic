#include <msp430.h> 
#include <stdint.h>
#include "libs/i2c.h"
#include "libs/timer.h"

// Deixa o LCD piscando

void main()
   {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	i2cInit();

	uint8_t lcdAddr = 0x3F;

	if(i2cSendByte(lcdAddr, 0)) // Descobrindo o endereco
	    lcdAddr = 0x27;

	while(1)
	{
	    i2cSendByte(lcdAddr, 0);
	    wait(1, sec);

	    i2cSendByte(lcdAddr, BIT3);
	    wait(1, sec);
	}
}
