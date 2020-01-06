#ifndef STM32_CONFIGURATION_H
#define STM32_CONFIGURATION_H

#define STM32_CONFIG_PRINTF_USART USART2

#include <stm32f10x.h>
#include <stm32f10x_conf.h>


#define USART_Baudrate 115200
#define I2C2_ADDRESS7 (0x0A<<1) /*Shift right 1 Bit, because the last bit is the R/W-Bit*/
#define I2C_CLOCK 400000

void Disable_Interrupts(void);
void Enable_Interrupts(void);

void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART1_Configuration(void);
void USART2_Configuration(void);

#endif /*STM32_CONFIGURATION_H */
