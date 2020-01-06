#include "stm32f10x_conf.h"
#include "mpu9250_lib.h"
#include "user_lib.h"

void mpu9520_init()
{
	printf("MPU9250 device ID: %d\r\n", I2C_read_byte(GYRO_ADDRESS, 0x75));
	printf("AK8963 device ID: %d\r\n", I2C_read_byte(MAG_ADDRESS, 0x00));
	
	//解除休眠状态
	I2C_write_byte(GYRO_ADDRESS, PWR_MGMT_1, 0x00); 		
	I2C_write_byte(GYRO_ADDRESS,SMPLRT_DIV, 0x07);
	I2C_write_byte(GYRO_ADDRESS,CONFIG, 0x06);
	//设置GYRO量程，当运动幅度不大时当然是越小越精准
	I2C_write_byte(GYRO_ADDRESS,GYRO_CONFIG, 0x10); 
	I2C_write_byte(GYRO_ADDRESS,ACCEL_CONFIG, 0x06);
	I2C_write_byte(GYRO_ADDRESS,ACCEL_CONFIG2, 0x06);
}

void read_mpu9250_cyro(int16_t *W)
{
	static uint8_t last_buff[6];
	uint8_t buff[6];	
	buff[0] = I2C_read_byte(GYRO_ADDRESS,GYRO_XOUT_L); 
	buff[0] >= 0 ? ((last_buff[0] = buff[0])) : ((buff[0] = last_buff[0])); 
	buff[1] = I2C_read_byte(GYRO_ADDRESS,GYRO_XOUT_H);
	buff[1] >= 0 ? ((last_buff[1] = buff[1])) : (buff[1] = last_buff[1]); 
	W[0] = (buff[1]<<8)|buff[0];
//	W[0]/=32.768; 						   //读取计算X轴数据

	buff[2] = I2C_read_byte(GYRO_ADDRESS,GYRO_YOUT_L);
	buff[2] >= 0 ? (last_buff[2] = buff[2]) : (buff[2] = last_buff[2]); 
	buff[3] = I2C_read_byte(GYRO_ADDRESS,GYRO_YOUT_H);
	buff[3] >= 0 ? (last_buff[3] = buff[3]) : (buff[3] = last_buff[3]); 
	W[1] = (buff[3]<<8)|buff[2];
//	W[1]/=32.768; 						   //读取计算Y轴数据
	buff[4] = I2C_read_byte(GYRO_ADDRESS,GYRO_ZOUT_L);
	buff[4] >= 0 ? (last_buff[4] = buff[4]) : (buff[4] = last_buff[4]); 
	buff[5] = I2C_read_byte(GYRO_ADDRESS,GYRO_ZOUT_H);
	buff[5] >= 0 ? (last_buff[5] = buff[5]) : (buff[5] = last_buff[5]); 
	W[2] = (buff[5]<<8)|buff[4];
//	W[2]/=32.768; 					       //读取计算Z轴数据
 
}

void read_mpu9250_accel(int16_t *A)
{
	static uint8_t last_buff[6];
	uint8_t buff[6];	
	buff[0] = I2C_read_byte(ACCEL_ADDRESS,ACCEL_XOUT_L); 
	buff[0] >= 0 ? (last_buff[0] = buff[0]) : (buff[0] = last_buff[0]); 
	buff[1] = I2C_read_byte(ACCEL_ADDRESS,ACCEL_XOUT_H);
	buff[1] >= 0 ? (last_buff[1] = buff[1]) : (buff[1] = last_buff[1]); 
	A[0] = (buff[1]<<8)|buff[0];
//	A[0]/=164; 						   //读取计算X轴数据

	buff[2] = I2C_read_byte(ACCEL_ADDRESS,ACCEL_YOUT_L);
	buff[2] >= 0 ? (last_buff[2] = buff[2]) : (buff[2] = last_buff[2]); 
	buff[3] = I2C_read_byte(ACCEL_ADDRESS,ACCEL_YOUT_H);
	buff[3] >= 0 ? (last_buff[3] = buff[3]) : (buff[3] = last_buff[3]); 
	A[1] = (buff[3]<<8)|buff[2];
//	A[1]/=164; 						   //读取计算Y轴数据
	buff[4] = I2C_read_byte(ACCEL_ADDRESS,ACCEL_ZOUT_L);
	buff[4] >= 0 ? (last_buff[4] = buff[4]) : (buff[4] = last_buff[4]); 
	buff[5] = I2C_read_byte(ACCEL_ADDRESS,ACCEL_ZOUT_H);
	buff[5] >= 0 ? (last_buff[5] = buff[5]) : (buff[5] = last_buff[5]); 
	A[2] = (buff[5]<<8)|buff[4];
//	A[2]/=164; 					       //读取计算Z轴数据
 
}

void delay(uint32_t ticks)
{
	while(ticks --);
}


void read_mpu9250_mag(int16_t *M)
{ 
	static uint8_t last_buff[6];
	uint8_t buff[6];
	if(I2C_write_byte(GYRO_ADDRESS,0x37,0x02) < 0)
		return;//turn on Bypass Mode 
	delay(1000000);	
	if(I2C_write_byte(MAG_ADDRESS,0x0A,0x01) < 0)
		return;
	delay(1000000);	
	buff[0] = I2C_read_byte (MAG_ADDRESS,MAG_XOUT_L);
	buff[0] >= 0 ? (last_buff[0] = buff[0]) : (buff[0] = last_buff[0]); 
	buff[1] = I2C_read_byte (MAG_ADDRESS,MAG_XOUT_H);
	buff[1] >= 0 ? (last_buff[1] = buff[1]) : (buff[1] = last_buff[1]); 
	M[0] = (buff[1]<<8)|buff[0];

	buff[2] = I2C_read_byte(MAG_ADDRESS,MAG_YOUT_L);
	buff[2] >= 0 ? (last_buff[2] = buff[2]) : (buff[2] = last_buff[2]); 
	buff[3] = I2C_read_byte(MAG_ADDRESS,MAG_YOUT_H);
	buff[3] >= 0 ? (last_buff[3] = buff[3]) : (buff[3] = last_buff[3]); 
	M[1] = (buff[3]<<8)|buff[2];
						   //读取计算Y轴数据
	 
	buff[4] = I2C_read_byte(MAG_ADDRESS,MAG_ZOUT_L);
	buff[4] >= 0 ? (last_buff[4] = buff[4]) : (buff[4] = last_buff[4]); 
	buff[5] = I2C_read_byte(MAG_ADDRESS,MAG_ZOUT_H);
	buff[5] >= 0 ? (last_buff[5] = buff[5]) : (buff[5] = last_buff[5]); 
	M[2] = (buff[5]<<8)|buff[4];
						   //读取计算Z轴数据
}
