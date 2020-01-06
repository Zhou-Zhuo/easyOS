
.syntax unified

.equ NVIC_ICSR,      0xe000ed04       
.equ PENDSVSET,      0x10000000        //PENDSV bit mask
.equ EXC_RETURN_PSP, 0xfffffffd
.extern high_ready_task
.extern os_ready

.globl disable_all_interrupts
.globl first_switch
.globl cpu_sr_save
.globl cpu_sr_restore
.globl PendSV_Handler
.globl context_switch

.cpu cortex-m3
.fpu softvfp
.thumb

.text

disable_all_interrupts:
	cpsid i
	bx    lr

cpu_sr_save:
	mrs   r0, primask
	cpsid i
	bx    lr

cpu_sr_restore:
	msr   primask, r0    //如果发生嵌套临界区结构也不怕内层exit时退出临界区
	bx    lr

.type PendSV_Handler,%function
PendSV_Handler:
	mrs   r3, primask
	cpsid i
	mrs   r0,  psp
	ldr   r1,  =current_task      //load address of current_task(just the pointer) into r1
	cbz   r0,  no_task_before     //first time switch, no task before
	stmdb r0!, {r4-r11}
	ldr   r2,  [r1]               //load address of current_task struct into r2, also address of current_task->sp
	str   r0,  [r2]               //store new sp into current task struct
no_task_before:
	ldr   r0,  =high_ready_task
	ldr   r0,  [r0]               
	str   r0,  [r1]               //current_task = high_ready_task
	ldr   r0,  [r0]               //get sp
	ldmia r0!, {r4-r11}
	msr   psp, r0 
	ldr   lr,  =EXC_RETURN_PSP
	msr   primask, r3
	bx    lr
	
first_switch:
	ldr r0,  =NVIC_ICSR
	ldr r1,  =PENDSVSET
	str r1,  [r0]       //set PendSV

	ldr r0,  =os_ready
	mov r1,  1
	str r1,  [r0]       //os_ready = True

	mov r0,  0
	msr psp, r0
	cpsie i
1:
	b 1b                //hang here, nerver return 

context_switch:
	ldr r0, =NVIC_ICSR
	ldr r1, =PENDSVSET
	str r1, [r0]
	bx  lr         //此处必须返回，且在CM3中将产生咬尾中断，中断调用点直接被PendSV继承！PendSV优先级最低，必定等此中断返回后才执行




