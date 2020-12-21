#ifndef _SPI_H
#define _SPI_H

#include <stdint.h>

// Pinos da interface SPI
#define STE     6,0         // Slave transmit enable (chip/slave select)
#define MISO    6,1         // Master in slave out
#define MOSI    6,2         // Master out slave in
#define SCLK    6,3         // Serial clock

#define TCLK2   50,us       // Metade do periodo de clock

// Bit-banging
uint8_t spiTransferBB(uint8_t byte, uint8_t CPOL, uint8_t CPHA);
// CPOL -> valor da linha de clock quando nao ha comunicacao
// CPHA -> define o significado das subidas se descidas do clock

#endif //_SPI_H
