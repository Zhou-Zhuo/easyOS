/*
 * @file	stm32_usart_lib.c
 * @author	zz
 * @version	0.0
 */

#include "stm32_usart_lib.h" 
#include <event.h>
#include <mailbox.h>

//static __inline void usart_putc(uint8_t ch);

serial_t com1;

static __inline int8_t com1_open(uint16_t oflag);
static __inline size_t com1_write(void *buff, size_t len);
static __inline size_t com1_read(void *buff, size_t len);

void com1_init()
{
	com1.open = com1_open;
	com1.read = com1_read;
	com1.write = com1_write;
}

static __inline int8_t com1_open(uint16_t oflag)
{
	com1.oflag = oflag;
	return 0;
}

static __inline size_t com1_write(void *buffin, size_t len)
{
	size_t i;
	uint8_t *buff = (uint8_t *)buffin;

	for(i = 0; i < len; i ++) {
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, *buff ++);
	}

	return i;
}

extern eid_t mbox_usart_dr;
static __inline size_t com1_read(void *buffin, size_t len)
{
	size_t i;
	uint8_t *buff = NULL;
	
	for(i = 0; i < len; i ++) {
		mailbox_pend(mbox_usart_dr, (void **)&buff , 0);
		*(uint8_t *)buffin ++ = *buff;
	}

	return i;

}

uint8_t usart_tx_index = 0;
uint8_t usart_tx_buff[64];
void usart_DMA_tx(uint8_t *buff, uint8_t nr);
volatile extern uint8_t DMA_busy;

/*static __inline void usart_putc(uint8_t ch)
{
#ifdef USART_USE_DMA
	if(ch == '\0') {
		if(usart_tx_index == 0)
			return ;
		while(DMA_busy);
		usart_DMA_tx(usart_tx_buff, usart_tx_index);
		usart_tx_index = 0;
		return ;
	}
	usart_tx_buff[usart_tx_index ++] = ch;

#else 
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
	USART_SendData(USART1, ch);
#endif
}*/

void usart_DMA_tx(uint8_t *buff, uint8_t nr)
{
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA1_Channel4->CMAR = (uint32_t)buff;
	DMA_SetCurrDataCounter(DMA1_Channel4, nr);
	DMA_Cmd(DMA1_Channel4, ENABLE);
	DMA_busy = 1;
}

