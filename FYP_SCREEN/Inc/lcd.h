/*
 * temp.h
 *
 *  Created on: Apr 24, 2026
 *      Author: Blake
 */

#ifndef LCD_H
#define LCD_H

#include "main.h"

extern const uint8_t font5x7[][5];

void LCD_Init(void);
void LCD_Clear(void);
void LCD_Command(uint8_t cmd);
void LCD_Data(uint8_t data);
void LCD_SetCursor(uint8_t page, uint8_t column);
void LCD_Char(char c);

#endif /* LCD_H_ */
