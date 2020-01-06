/*
 *  @file		stm32_init.c
 *  @author		zz
 *  @version	0.0
 */

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_i2c.h"
#include "misc.h"
//#include "stdarg.h"
#include "stm32_init.h"
#include "macro_config.h"
#include "cpu.h"
#include "config.h"

void Fault_init();

void board_init()
{
	RCC_init();
	Fault_init();
	NVIC_init();
	GPIO_init();
	USART_init();
//	I2C_init();
	//TIM3_PWM_Init(7199, 9);
//	SysTick_init();
}


void RCC_init()
{
	SystemInit(); 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |
							RCC_APB2Periph_GPIOA | 
							RCC_APB2Periph_GPIOB |
							RCC_APB2Periph_AFIO  , 
							ENABLE); 
}


void NVIC_init()
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */  
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  
	
	/* USART irq init */	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			     	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);

	/* PendSV irq init */
	NVIC_InitStructure.NVIC_IRQChannel = PendSV_IRQn;			     	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;	     
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);

	/* SysTick irq init */
	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Fault_init()
{
	SCB->SHCSR |= 1<<18;
	SCB->SHCSR |= 1<<17;
	SCB->SHCSR |= 1<<16;
}

void GPIO_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Pin B5 is used for a led */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 

	/* Pin A9 and A10 are used for usart Tx and Rx */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    	
	GPIO_Init(GPIOA, &GPIO_InitStructure);		    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
}
		

void USART_DMA_Conf();
void USART_init()
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200;						
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			
	USART_InitStructure.USART_Parity = USART_Parity_No;				
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					

	/* Configure USART1 */
	USART_Init(USART1, &USART_InitStructure);							
 
	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                    
//	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);						

#ifdef USART_USE_DMA
//	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

	/* Enable USART1 DMA */
	USART_DMA_Conf();
#endif

	/* Enable the USART1 */
	USART_Cmd(USART1, ENABLE);

	/* Read SR to prevent the 1st bit to be ingnored */
	USART_ClearFlag(USART1, USART_FLAG_TC);
}

#define GPIO_WAIT_LOW(gpio, pin, timeoutcycles)\
  {\
    int i = timeoutcycles;\
    while(GPIO_ReadInputDataBit(gpio, pin) == Bit_RESET && i--);\
  }

#define GPIO_WAIT_HIGH(gpio, pin, timeoutcycles) \
  {\
    int i = timeoutcycles;\
    while(GPIO_ReadInputDataBit(gpio, pin) == Bit_SET && i--);\
  }


void USART_DMA_Conf()
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
    DMA_DeInit(DMA1_Channel4);   //将DMA的通道4寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;  //DMA外设USART1_DR基地址
	//DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
	//DMA_InitStructure.DMA_BufferSize = cndtr;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
	
	/* DMA irq configuration */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;			     	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	     
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);

	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

void i2cdelay(uint16_t i)
{
	while(i--);
}

	
void I2C_bus_reset()
{
	  GPIO_SetBits(GPIOB, GPIO_Pin_7);
  /* Check SDA line to determine if slave is asserting bus and clock out if so */
  while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == Bit_RESET)
  {
    /* Set clock high */
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    /* Wait for any clock stretching to finish. */
    GPIO_WAIT_LOW(GPIOB, GPIO_Pin_6, 10 * 5000);
    i2cdelay(60);

    /* Generate a clock cycle */
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    i2cdelay(60);
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    i2cdelay(60);
  }

  /* Generate a start then stop condition */
  GPIO_SetBits(GPIOB, GPIO_Pin_6);
  i2cdelay(60);
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
  i2cdelay(60);
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
  i2cdelay(60);

  /* Set data and clock high and wait for any clock stretching to finish. */
  GPIO_SetBits(GPIOB, GPIO_Pin_7);
  GPIO_SetBits(GPIOB, GPIO_Pin_6);
  GPIO_WAIT_LOW(GPIOB, GPIO_Pin_6, 10 * 5000);
  /* Wait for data to be high */
  GPIO_WAIT_HIGH(GPIOB, GPIO_Pin_7, 10 * 5000);


}

void I2C_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;

	/* GPIOB clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* Enable the DMA1 clock */
//		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* I2C1 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    /* I2C1 SDA and SCL configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;			//
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_bus_reset();
	/* Enable I2C1 reset state */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
	/* Release I2C1 from reset state */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//	I2C_InitStructure.I2C_OwnAddress1 = 0x28;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_Init(I2C1, &I2C_InitStructure);

//    NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_I2C_PRI;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_I2C_PRI + 1;
//    NVIC_Init(&NVIC_InitStructure);
}

void SysTick_init()
{
	SysTick->LOAD = CPU_FREQ / TICKS_PER_SEC;
	SysTick->VAL = 0;
	SysTick->CTRL &= 0XFFFFFFFB;
	SysTick->CTRL |= 0X3; 	
}
