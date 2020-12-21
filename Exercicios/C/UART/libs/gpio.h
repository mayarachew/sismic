#ifndef _GPIO_H     // protecao contra declaracoes multiplas
#define _GPIO_H

enum pinModes {input, output, inPullUp, inPullDown};

void   digitalWrite(uint8_t port, uint8_t bit, uint8_t value);
// Escreve o valor em determinado pino

uint8_t digitalRead(uint8_t port, uint8_t bit);
// Le o valor em determinado pino

void        pinMode(uint8_t port, uint8_t bit, enum pinModes mode);
// Configura o pino

#endif // _GPIO_H
