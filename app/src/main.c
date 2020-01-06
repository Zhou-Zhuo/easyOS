#include <stm32f10x_conf.h>
#include <stm32_configuration.h>
#include <stddef.h>
#include <init.h>
#include <stm32_init.h>
#include <task.h>
#include <scheduler.h>
#include <mailbox.h>
#include <stdbool.h>

#define TASK1_STKSIZE 256
stack_t task1_stk[TASK1_STKSIZE];

#define TASK2_STKSIZE 256
stack_t task2_stk[TASK2_STKSIZE];

#define TASK3_STKSIZE 256
stack_t task3_stk[TASK3_STKSIZE];

static void task1(void);

eid_t mbox1;
eid_t mbox_usart_dr;

static void task3(void)
{
	void *data;

	if(mailbox_pend(mbox1, &data, 0) == 0)
		printf("T3 get mbox1: %s\r\n", (char *)data);
	else 
		printf("T3 mbox1 timeout.\r\n");
	while(1) {
//		printf("T3 ID:%d\r\n", get_current_id());
		sleep_ticks(10);
	}
}

static void task2(void)
{
	tid_t tid;
	eid_t eid;
	uint8_t i;

	task_delete(1);

	for(i = 0; i < 9; i ++)
		mailbox_create(&eid);

	mailbox_create(&mbox1);

//	for(i = 0; i < 9; i ++)
//		mailbox_delete(i);

	task_create(&tid, task3, 14, &task3_stk[TASK3_STKSIZE-1]);

	task_create(&tid, task1, 16, &task1_stk[TASK1_STKSIZE-1]);

//	task_delete(2);
	task_delete(get_current_id());

	while(1) {
	//	printf("T2 ID:%d\r\n", get_current_id());
		sleep_ticks(8);
	}
}

char hello[]="Hello, task3!\r\n";

static void task1(void)
{
//	mailbox_post(mbox1, (void *)hello);

	while(1) {
//		printf("T1 ID:%d\r\n", get_current_id());
		sleep_ticks(3);
	}
}

#define TASK_INCOMSH_STKSIZE 1024
stack_t task_incomsh_stk[TASK_INCOMSH_STKSIZE];

static void task_incomsh()
{
	incom_mainloop();
	while(1);
}

#define TASK_INIT_STKSIZE 512
stack_t task_init_stk[TASK_INIT_STKSIZE];

void disable_all_interrupts();

static void task_init(void)
{
	//TODO: 加上统计CPU使用率
//	uint16_t i = 0;
	tid_t tid;

	SysTick_init();   //to invent interrupt happens right after we create 
	//tasks, SysTick_init() should be placed here rather than in 
	//board_init()
	printf("task_init running.\r\n");
	mailbox_create(&mbox_usart_dr);
	task_create(&tid, task1, 10, &task1_stk[TASK1_STKSIZE-1]);
	task_create(&tid, task2, 13, &task2_stk[TASK2_STKSIZE-1]);
	task_create(&tid, task_incomsh, 20, &task_incomsh_stk[TASK_INCOMSH_STKSIZE-1]);

	while(1) {
//		while(!((i+=4)/4))
//			printf(".");	
	}
}

void usart_test()
{
	int i0 = 0,
		i1 = 0,
		i2 = 0,
		i3 = 0;
	while(!(USART1->SR & 0x40))
		i0 ++;
	USART1->DR = 'T';
	while(!(USART1->SR & 0x40))
		i1 ++;
	USART1->DR = 'E';
	while(!(USART1->SR & 0x40))
		i2 ++;
	USART1->DR = 'S';
	while(!(USART1->SR & 0x40))
		i3 ++;
	USART1->DR = 'T';

	printf("\r\ni0 = %d\r\ni1 = %d\r\ni2 = %d\r\ni3 = %d\r\n",
		   	i0, i1, i2, i3);
}

void sys_init();
void startup();

int main()
{
	tid_t tid;

	
	disable_all_interrupts();
	board_init();
	serial_init();
	console_init();
	sys_init();
	task_create(&tid, task_init, LOWIST_PRIO, 
		&task_init_stk[TASK_INIT_STKSIZE-1]);  //task_init is also the idle 
											//task
	usart_test();
	startup();
	incom_mainloop();

	while(1);
}



