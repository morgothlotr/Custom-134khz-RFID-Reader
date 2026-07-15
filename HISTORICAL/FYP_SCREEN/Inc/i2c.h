/*
 * i2c.h
 *
 *  Created on: Apr 24, 2026
 *      Author: Blake
 */

#ifndef I2C_H
#define I2C_H

#include "main.h"

void lcd_init(I2C_HandleTypeDef *hi2c);
void lcd_clear(void);
void lcd_put_cur(uint8_t row, uint8_t col);
void lcd_send_string(char *str);

#endif /* I2C_H_ */
