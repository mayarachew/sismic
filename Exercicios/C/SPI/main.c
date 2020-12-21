#include <msp430.h> 
#include <stdint.h>
#include "libs/spi.h"
#include "libs/gpio.h"
#include "libs/timer.h"

// Implementar o protocolo SPI em bit-banging

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	uint8_t byte = 0xFF;
	digitalWrite(SCLK, 0);

	while(byte)
	{
	    spiTransferBB(byte--, 0, 0);
	}

    __low_power_mode_0();
}
