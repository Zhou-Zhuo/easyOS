#include "user_lib.h"

int do_clear(int argc, char *argv[])
{
	printf("\f");
	return 1;
}

INCOM_SH_CMD(clear, do_clear,
		"clear - Clear screen.\r\n");

extern const incom_sh_t *__incom_sh_cmd_start;
extern const incom_sh_t *__incom_sh_cmd_end;

int do_help(int argc, char *argv[])
{
	const incom_sh_t *incom_cmd;

	if(argc > 1) {
		print_usage();
		return 0;
	}

	if(argc == 0) {
		for (incom_cmd = __incom_sh_cmd_start; incom_cmd < __incom_sh_cmd_end; incom_cmd ++) {
			printf(incom_cmd->usage);
		}
		return 1;
	}
	else if(argc == 1) {
		for (incom_cmd = __incom_sh_cmd_start; incom_cmd < __incom_sh_cmd_end; incom_cmd ++) {
			if(string_cmp(argv[0], incom_cmd->name)) {
				printf(incom_cmd->usage);
				return 1;
			}
		}
		printf("help: Command not found.\r\n");
		return 0;
	}

	return 1;

}

INCOM_SH_CMD(help, do_help,
		"help [cmd] - List usage of some command.\r\n");

int do_iocon(int argc, char *argv[])
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_TypeDef* port;
	uint16_t pin = 0;
	GPIOMode_TypeDef mode;
	uint32_t rcc;
	uint8_t i = 0;

	if(argc != 3) 
		goto USAGE_ERR;

	if(string_cmp(argv[0], "A")) {
		port = GPIOA;
		rcc = RCC_APB2Periph_GPIOA;
	}
	else if(string_cmp(argv[0], "B")) {
		port = GPIOB;
		rcc = RCC_APB2Periph_GPIOB;
	}
	else if(string_cmp(argv[0], "C")) {
		port = GPIOC;
		rcc = RCC_APB2Periph_GPIOC;
	}
	else if(string_cmp(argv[0], "D")) {
		port = GPIOD;
		rcc = RCC_APB2Periph_GPIOD;
	}
	else if(string_cmp(argv[0], "E")) {
		port = GPIOE;
		rcc = RCC_APB2Periph_GPIOE;
	}
	else {
		goto USAGE_ERR;
	}

	for(i = 0; i < 2 && argv[1][i]; i++) 
		pin = argv[1][i] - '0' + 10*pin;
	if(pin < 0 || pin > 15 || argv[1][i] != '\0') 
		goto USAGE_ERR;

	if(string_cmp(argv[2], "AI")) 
		mode = GPIO_Mode_AIN;
	else if(string_cmp(argv[2], "AO")) 
		mode = GPIO_Mode_AF_OD;
	else if(string_cmp(argv[2], "AP")) 
		mode = GPIO_Mode_AF_PP;
	else if(string_cmp(argv[2], "IF")) 
		mode = GPIO_Mode_IN_FLOATING;
	else if(string_cmp(argv[2], "OP")) 
		mode = GPIO_Mode_Out_PP;
	else if(string_cmp(argv[2], "OO")) 
		mode = GPIO_Mode_Out_OD;
	else if(string_cmp(argv[2], "ID")) 
		mode = GPIO_Mode_IPD;
	else if(string_cmp(argv[2], "IU")) 
		mode = GPIO_Mode_IPU;
	else 
		goto USAGE_ERR;

	RCC_APB2PeriphClockCmd(rcc, ENABLE);
	GPIO_InitStructure.GPIO_Pin = 1 << pin;
	GPIO_InitStructure.GPIO_Mode = mode;			 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(port, &GPIO_InitStructure);					 

	return 1;

USAGE_ERR:
	print_usage();
	return 0;

}

INCOM_SH_CMD(iocon, do_iocon, 
		"iocon [port] [pin] [mode] - Config a GPIO pin.\r\n\
	port - A/B/C/D/E\r\n\
	pin  - 0~15\r\n\
	mode - AI/AO/AP/IF/OP/OO/ID/IU\r\n");

int do_ioset(int argc, char *argv[])
{
	GPIO_TypeDef* port;
	uint16_t pin = 0;
	uint8_t i = 0;

	if(argc != 3) 
		goto USAGE_ERR;

	if(string_cmp(argv[0], "A")) 
		port = GPIOA;
	else if(string_cmp(argv[0], "B")) 
		port = GPIOB;
	else if(string_cmp(argv[0], "C")) 
		port = GPIOC;
	else if(string_cmp(argv[0], "D")) 
		port = GPIOD;
	else if(string_cmp(argv[0], "E")) 
		port = GPIOE;
	else 
		goto USAGE_ERR;

	for(i = 0; i < 2 && argv[1][i]; i++) 
		pin = argv[1][i] - '0' + 10*pin;
	if(pin < 0 || pin > 15 || argv[1][i] != '\0') 
		goto USAGE_ERR;

	if(string_cmp(argv[2], "0"))
		GPIO_ResetBits(port, 1 << pin);
	else if(string_cmp(argv[2], "1"))
		GPIO_SetBits(port, 1 << pin);
	else 
		goto USAGE_ERR;

	return 1;

USAGE_ERR:
	print_usage();
	return 0;
}

INCOM_SH_CMD(ioset, do_ioset, 
		"ioset [port] [pin] [value] - Set or reset a GPIO pin.\r\n\
	port  - A/B/C/D/E\r\n\
	pin   - 0~15\r\n\
	value - 0/1\r\n");

int do_led(int argc, char *argv[])
{
	if(argc != 1) 
		goto USAGE_ERR;

	if(string_cmp(argv[0], "on")) 
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
	else if(string_cmp(argv[0], "off")) 
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	else 
		goto USAGE_ERR;

	return 1;

USAGE_ERR:
	print_usage();
	return 0;
}

INCOM_SH_CMD(led, do_led, 
		"led [on/off] - Turn on/off the led.\r\n");

extern PWM* pwmF;
extern PWM* pwmB;
extern PWM* pwmL;
extern PWM* pwmR;

int do_pwmf(int argc, char* argv[])
{
	uint32_t freq = 0;
	uint16_t psc = 0;
	uint16_t arr = 0;
	uint8_t i = 0;

	if(argc == 0) {
		printf("%dHz\r\n", pwmF->freq);
		return 1;
	}
	if(argc != 1) 
		goto USAGE_ERR;
	
	for(i = 0; argv[0][i]; i ++) 
		freq = 10*freq + argv[0][i] - '0';
	
	if(freq > 10000000) 
		goto USAGE_ERR;
	else if(freq > 100000) 
		psc = 0;
	else if(freq > 100)
		psc = 99;
	else if(freq >= 10)
		psc = 999;
	else 
		goto USAGE_ERR;

	arr = 72000000 / ((psc + 1) * freq) - 1;

	TIM3->ARR = arr;
	TIM3->PSC = psc;
	*(pwmF->ccr) = (uint16_t)((float)arr * pwmF->val);
	*(pwmB->ccr) = (uint16_t)((float)arr * pwmB->val);
	*(pwmL->ccr) = (uint16_t)((float)arr * pwmL->val);
	*(pwmR->ccr) = (uint16_t)((float)arr * pwmR->val);

	pwmF->arr = arr;
	pwmF->freq = freq;

	return 1;

USAGE_ERR:
	print_usage();
	return 0;

}	

INCOM_SH_CMD(pwmf, do_pwmf, 
		"pwmf [freq] - Set pwm frequency.\r\n\
	freq - 10~10000000\r\n");
/*
int do_pwmv(int argc, char* argv[])
{
	float val;
	uint16_t cmp = 0;
	uint8_t i = 0; 

	if(argc == 0) {
		printf("%d%\r\n", (uint8_t)(pwm1.val * 100));
		return 1;
	}
	if(argc != 1) 
		goto USAGE_ERR;

	for(i = 0; argv[0][i]; i ++) 
		cmp = 10 * cmp + argv[0][i] - '0';

	if(cmp < 0 || cmp > 100) 
		goto USAGE_ERR;

	pwm1.val = (float)cmp / 100.0;

	TIM3->CCR2 = pwm1.arr * pwm1.val;

	return 1;

USAGE_ERR:
	print_usage();
	return 0;

}

INCOM_SH_CMD(pwmv, do_pwmv, 
		"pwmv [val] - Set pwm value.\r\n\
	val - 0~100\r\n");
*/

int do_chpwm(int argc, char* argv[])
{
	char ch = 0;
	PWM *pwm = NULL;
	float val;

	if(argc != 1) 
		goto USAGE_ERR;
	
	if(string_cmp(argv[0], "A")) 
		goto CHPWM_A;
	if(string_cmp(argv[0], "F")) 
		pwm = pwmF;
	else if(string_cmp(argv[0], "B")) 
		pwm = pwmB;
	else if(string_cmp(argv[0], "L")) 
		pwm = pwmL;
	else if(string_cmp(argv[0], "R")) 
		pwm = pwmR;
	else 
		goto USAGE_ERR;


	printf("Press 'A'/'S' to change pwm value, 'Z' to set it to 0, 'X' to exit.\r\n");
	printf("\r%d% ", (uint8_t)(pwm->val * 100));

	while(1) {
		ch = getc();
		switch(ch) {
			case 'a':
			case 'A':
				if(pwm->val <= 0.99) {
					pwm->val += 0.01;
					*(pwm->ccr) = pwm->arr * pwm->val;
				}
				val = (float)(*(pwm->ccr))/(float)pwm->arr;
				printf("\r%d% ", (uint8_t)(val * 100));
				break;
			case 's':
			case 'S':
				if(pwm->val >= 0.01) {
					pwm->val -= 0.01;
					*(pwm->ccr) = pwm->arr * pwm->val;
				}
				val = (float)(*(pwm->ccr))/(float)pwm->arr;
				printf("\r%d% ", (uint8_t)(val * 100));
				break;
			case 'z':
			case 'Z':
				pwm->val = 0.0;
				*(pwm->ccr) = pwm->arr * pwm->val;
				printf("\r%d% ", (uint8_t)(pwm->val * 100));
				break;
			case 'x':
			case 'X':
				printf("\r\n");
				return 1;
			default : break;
		}
	}

USAGE_ERR:
	print_usage();
	return 0;

CHPWM_A:

	printf("Press 'A'/'S' to change pwm value, 'Z' to set it to 0, 'X' to exit.\r\n");
	printf("\r%d% ", (uint8_t)(pwm->val * 100));
	while(1) {
			ch = getc();
			switch(ch) {
				case 'a':
				case 'A':
					if(pwmF->val <= 0.99) {
						pwmF->val += 0.01;
						*(pwmF->ccr) = pwmF->arr * pwmF->val;
						*(pwmB->ccr) = pwmF->arr * pwmF->val;
						*(pwmL->ccr) = pwmF->arr * pwmF->val;
						*(pwmR->ccr) = pwmF->arr * pwmF->val;
					}
					printf("\r%d% ", (uint8_t)(pwmF->val * 100));
					break;
				case 's':
				case 'S':
					if(pwmF->val >= 0.01) {
						pwmF->val -= 0.01;
						*(pwmF->ccr) = pwmF->arr * pwmF->val;
						*(pwmB->ccr) = pwmF->arr * pwmF->val;
						*(pwmL->ccr) = pwmF->arr * pwmF->val;
						*(pwmR->ccr) = pwmF->arr * pwmF->val;
					}
					printf("\r%d% ", (uint8_t)(pwmF->val * 100));
					break;
				case 'z':
				case 'Z':
					pwmF->val = 0.0;
					pwmB->val = 0.0;
					pwmL->val = 0.0;
					pwmR->val = 0.0;
					*(pwmF->ccr) = pwmF->arr * pwmF->val;
					*(pwmB->ccr) = pwmF->arr * pwmF->val;
					*(pwmL->ccr) = pwmF->arr * pwmF->val;
					*(pwmR->ccr) = pwmF->arr * pwmF->val;
					printf("\r%d% ", (uint8_t)(pwm->val * 100));
					break;
				case 'x':
				case 'X':
					printf("\r\n");
					return 1;
				default : break;
			}	
		}

}

INCOM_SH_CMD(chpwm, do_chpwm, 
		"chpwm [pwmX] - Change pwm value.\r\n\
	pwmX - F/B/L/R/A\r\n");

int do_reset(int argc, char* argv[])
{
	if(argc)
		goto USAGE_ERR;

	__set_FAULTMASK(1);
	NVIC_SystemReset();

USAGE_ERR:
	print_usage();
	return 0;
}

INCOM_SH_CMD(reset, do_reset, 
		"reset - Reset the chip.\r\n");
