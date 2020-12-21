#ifndef _LCD_H
#define _LCD_H

void lcdInit();
void lcdWriteNibble(uint8_t nibble, uint8_t rs);
void lcdWriteByte  (uint8_t byte,   uint8_t rs);
void print(char * str);

#endif //_LCD_H
