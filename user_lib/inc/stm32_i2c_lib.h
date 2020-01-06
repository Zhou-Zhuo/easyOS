#ifndef STM32_I2C_LIB_H
#define STM32_I2C_LIB_H

int16_t I2C_read_byte(uint8_t slave_addr,uint8_t mem_addr);
uint8_t I2C_write_byte(uint8_t slave_addr,uint8_t mem_addr,uint8_t data);

#endif
