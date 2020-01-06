#ifndef __TIMER_H
#define __TIMER_H

#include "user_lib.h"

typedef struct {
	uint16_t arr;
	float val;
	__IO uint16_t *ccr; //point to CCR register
	uint32_t freq;
} PWM;

void TIM3_PWM_Init(u16 arr,u16 psc);
void PWMstruct_init(PWM *pwm, uint16_t arr, uint32_t freq, float val, __IO uint16_t *ccr);


#endif
