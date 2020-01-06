#include "stm32f10x_conf.h"

int16_t I2C_read_byte(uint8_t slave_addr,uint8_t mem_addr)
{
	uint8_t ch;
	uint16_t i = 0;

	/* �ȴ����߿��� */
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	/* ����START���� */
	I2C_GenerateSTART(I2C1,ENABLE);
	/* �ȴ�I2C�ӿڱ�Ϊ��ģʽ(EV5) */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
		if(i ++ == 10000) {
			I2C_GenerateSTOP(I2C1, ENABLE);
			return -1;
		}
	/* ����7λ���豸��ַ,����ģʽ */
	I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Transmitter);
	/* �ȴ��ӿڽ��뷢��ģʽ(EV6) */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		if(i ++ == 10000) {
			I2C_GenerateSTOP(I2C1, ENABLE);
			return -1;
		}
	/* �������������EV6 */
  	I2C_Cmd(I2C1, ENABLE); 
	/* ���Ͷ���ַ */
  	I2C_SendData(I2C1, mem_addr);  
	/* EV8 */ 
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) 
		if(i ++ == 10000) {
			I2C_GenerateSTOP(I2C1, ENABLE);
			return -2;
		}
	/* ���·���START */
  	I2C_GenerateSTART(I2C1, ENABLE); 
	/* EV5 */
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
		if(i ++ == 10000) {
			I2C_GenerateSTOP(I2C1, ENABLE);
			return -3;
		}
	/* ����ģʽ */
  	I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Receiver);
	/* EV6 */
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		if(i ++ == 10000) {
			I2C_GenerateSTOP(I2C1, ENABLE);
			return -4;
		}
    I2C_AcknowledgeConfig(I2C1, DISABLE);
//STOP:
	/* STOP���� */
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
	/* ������ʼ���� */
	I2C_GenerateSTART(I2C1,ENABLE); 
	/* �ȴ�ACK */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	if(i ++ == 10000) {
		data = -1;
		goto STOP;
	}
	/* ���豸�����豸��ַ */
	I2C_Send7bitAddress(I2C1,slave_addr,I2C_Direction_Transmitter); 
	/* �ȴ�ACK */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	if(i ++ == 10000) {
		data = -1;
		goto STOP;
	}
	/* �Ĵ�����ַ */
	I2C_SendData(I2C1, mem_addr); 
	/* �ȴ�ACK */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) 
	if(i ++ == 10000) {
		data = -1;
		goto STOP;
	}
	/* �������� */
	I2C_SendData(I2C1, data);
	/* ������� */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) 
	if(i ++ == 10000) {
		data = -1;
		goto STOP;
	}
STOP:
	/* ���������ź� */
	I2C_GenerateSTOP(I2C1, ENABLE);
	return data;
}

void I2C1_EV_IRQHandler()
{
	
}
