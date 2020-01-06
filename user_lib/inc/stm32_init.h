/* 
 * @file	stm32_init.h
 * @author	zz
 * @version 0.0
 */

#ifndef STM32_INIT_H
#define STM32_INIT_H

void board_init();
void RCC_init();
void NVIC_init();
void GPIO_init();
void USART_init();
void I2C_init();
void SysTick_init();

#endif
