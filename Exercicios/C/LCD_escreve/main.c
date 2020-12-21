#include <msp430.h>
#include <stdint.h>
#include "libs/i2c.h"
#include "libs/timer.h"
#include "libs/lcd.h"

// Utilizar o LCD c/ o conversor serial

// Montagem:
// SCL no P3.1
// SDA no P3.0
// GND em GND
// VCC em 5V

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    i2cInit();
    lcdInit();

    while(1)
    {
        lcdWriteByte('H', 1);
        lcdWriteByte('e', 1);
        lcdWriteByte('l', 1);
        lcdWriteByte('l', 1);
        lcdWriteByte('o', 1);
        lcdWriteByte(' ', 1);

        print("World!");

        __low_power_mode_0();
    }
}
