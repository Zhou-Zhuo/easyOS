#include "stm32f10x_conf.h"

int16_t I2C_read_byte(uint8_t slave_addr,uint8_t mem_addr)
{
	uint8_t ch;
	uint16_t i = 0;

	/* 等待总线空闲 */
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	/* 发送START条件 */
	I2C_GenerateSTART(I2C1,ENABLE);
	/* 等待I2C接口变为主模式(EV5) */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
		if(i ++ == 10000) {
			I2C_GenerateSTOP(I2C1, ENABLE);
			return -1;
		}
	/* 发送7位从设备地址,发送模式 */
	I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Transmitter);
	/* 等待接口进入发送模式(EV6) */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		if(i ++ == 10000) {
			I2C_GenerateSTOP(I2C1, ENABLE);
			return -1;
		}
	/* 重新设置以清除EV6 */
  	I2C_Cmd(I2C1, ENABLE); 
	/* 发送读地址 */
  	I2C_SendData(I2C1, mem_addr);  
	/* EV8 */ 
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) 
		if(i ++ == 10000) {
			I2C_GenerateSTOP(I2C1, ENABLE);
			return -2;
		}
	/* 重新发送START */
  	I2C_GenerateSTART(I2C1, ENABLE); 
	/* EV5 */
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
		if(i ++ == 10000) {
			I2C_GenerateSTOP(I2C1, ENABLE);
			return -3;
		}
	/* 接收模式 */
  	I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Receiver);
	/* EV6 */
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		if(i ++ == 10000) {
			I2C_GenerateSTOP(I2C1, ENABLE);
			return -4;
		}
    I2C_AcknowledgeConfig(I2C1, DISABLE);
//STOP:
	/* STOP条件 */
    I2C_GenerateSTOP(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
		if(i ++ == 10000) {
//			I2C_GenerateSTOP(I2C1, ENABLE);
			return -5;
		}
    ch = I2C_ReceiveData(I2C1);
   
  	I2C_AcknowledgeConfig(I2C1, ENABLE);

	
	return ch;
		
}

int16_t I2C_write_byte(uint8_t slave_addr,uint8_t mem_addr,uint8_t data)
{
	uint16_t i = 0;
	/* 产生起始条件 */
	I2C_GenerateSTART(I2C1,ENABLE); 
	/* 等待ACK */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	if(i ++ == 10000) {
		data = -1;
		goto STOP;
	}
	/* 向设备发送设备地址 */
	I2C_Send7bitAddress(I2C1,slave_addr,I2C_Direction_Transmitter); 
	/* 等待ACK */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	if(i ++ == 10000) {
		data = -1;
		goto STOP;
	}
	/* 寄存器地址 */
	I2C_SendData(I2C1, mem_addr); 
	/* 等待ACK */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) 
	if(i ++ == 10000) {
		data = -1;
		goto STOP;
	}
	/* 发送数据 */
	I2C_SendData(I2C1, data);
	/* 发送完成 */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) 
	if(i ++ == 10000) {
		data = -1;
		goto STOP;
	}
STOP:
	/* 产生结束信号 */
	I2C_GenerateSTOP(I2C1, ENABLE);
	return data;
}

void I2C1_EV_IRQHandler()
{
	
}
