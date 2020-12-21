#ifndef _I2C_H
#define _I2C_H

#include <stdint.h>

// Interface dedicada

void i2cInit();     // Inicializacao da interface

uint8_t i2cSend(uint8_t addr, uint8_t * data, uint8_t nBytes);
uint8_t i2cGet (uint8_t addr, uint8_t * data, uint8_t nBytes);

uint8_t i2cSendByte(uint8_t addr, uint8_t byte);
uint8_t i2cGetByte (uint8_t addr);

// Bit banging

void i2cSendBB(uint8_t addr, uint8_t * data, uint8_t nBytes);
void i2cGetBB (uint8_t addr, uint8_t * data, uint8_t nBytes);

#endif //_I2C_H
