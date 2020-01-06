/*
 * @file	stm32_usart_lib.h
 * @author	zz
 * @version	0.0
 */

#ifndef STM32_USART_LIB_H
#define STM32_USART_LIB_H

#include "user_lib.h"

uint8_t usart_putc(uint8_t ch);
uint8_t usart_getc();
void com1_init();

#endif

