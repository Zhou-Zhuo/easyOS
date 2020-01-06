/*
 * @file	incomplete_shell.c
 * @author	zz
 * @version	0.0
 */

#include "incomplete_shell.h"

#define LINE_BUFF_SIZE		64
#define MAX_ARGC 			8
#define ARGV_BUFF_SIZE		64
#define HISTORY_BUFF_SIZE	128
#define MAX_HISTORY_NR		32

const incom_sh_t *__incom_sh_cmd_start;
const incom_sh_t *__incom_sh_cmd_end;

extern const int __incom_sh_cmdBase;
extern const int __incom_sh_cmdLimit;

char line_buff[LINE_BUFF_SIZE];

char cmd_history[HISTORY_BUFF_SIZE];
uint8_t history_cmd_indx[MAX_HISTORY_NR];

const incom_sh_t *pcmd_obj; 
int argc;
char *argv[MAX_ARGC];
char *tokenval;
int cmd_return;

uint8_t scan = 0;
int lookahead = 0;

static void section_init();

void incom_mainloop()  
{
	section_init();
	while(1) {
		data_reset();
		read_line();
		run_cmd();
	}
}

static void section_init()
{
	__incom_sh_cmd_start = (incom_sh_t *)&__incom_sh_cmdBase;
	__incom_sh_cmd_end = (incom_sh_t *)&__incom_sh_cmdLimit;
}

void data_reset()
{
	int i;
	scan = 0;
	argc = 0;
	for (i = 0;i < MAX_ARGC; i ++) {
		argv[i] = NULL;
	}
}

void wait10ms()
{
	uint16_t i = 0;
	while (i ++ == 10000);
}

int read_line()
{
	char ch;
	uint8_t state = 0;
	uint8_t cur = 0;

	printf(INCOM_SH_PROMPT);
	while(1) {
		ch = getc();
		switch (state) {
			case 0: 
				if (ch == 27) {
					ch = 0;
					wait10ms();
					ch = getc();
					if (ch == 0) {
						break;
						//ESC
					}
					else if (ch == '[') { 
						state = 1;
						break;
					}
				}
				else if (ch == '\r') { //ENTER
					line_buff[cur] = '\0';
					printf("\r\n");
					return 1;
				}
				else if (ch == 127) { 
					if(cur > 0) {
						cur --;
						putc(8);
					}
					break;//BACKSPACE
				}
				else {    //Normal character
					if (cur == LINE_BUFF_SIZE - 1) 
						break;
					line_buff[cur ++] = ch;
					putc(ch);
					break; 
				}
			case 1:
				switch (ch) {
					case 'A':  //UP
					case 'B':  //DOWN
					case 'C':  //RIGHT
					case 'D':  //LEFT
					default : break;
				};
				state = 0;
				break;
			default : break; //never come here
		}
	}

}

#define WORD 256
#define DONE 257	

int run_cmd()
{
	lookahead = lexan();
	if(lookahead == DONE)
		return 1;
	if(lookahead == WORD) {
		if(!find_cmd()) {
			printf("Error: Unknown Command.\r\n");
			return 0;
		}
		match(WORD);
	}
	while (1) {
		if(lookahead == DONE) {
			cmd_return = (pcmd_obj->cmd)(argc, argv); //DO IT!
			if(!cmd_return)
				printf("Return: %d\r\n", cmd_return);
			return 1;
		}	
		else if(lookahead == WORD) { 
			if(argc > MAX_ARGC - 1) {
				printf("Error: Too many args.\r\n");
				return 0;
			}
			argv[argc ++] = tokenval;
			match(WORD);
		}
	}
}

uint8_t string_cmp(char *str1, char *str2)
{
	while (!(*str1 == '\0' && *str2 == '\0'))
		if (*str1++ != *str2++)
			return 0;
	return 1;
}

int find_cmd()
{
	const incom_sh_t *incom_cmd = 0;
	for (incom_cmd = __incom_sh_cmd_start; incom_cmd < __incom_sh_cmd_end;
		incom_cmd ++) {
		if (string_cmp(tokenval, incom_cmd->name)) {
			pcmd_obj = incom_cmd;
			return 1;
		}
	}		
	return 0;
}


int lexan()
{
	char ch;
	uint8_t head = 0;

	do {
		ch = line_buff[scan ++];
	} while(ch == ' ' || ch == '\t');
	head = -- scan;
	
	if(ch == '\0')
		return DONE;

	while(1) {
		ch = line_buff[scan];
		if (ch == ' ' || ch == '\t') {
			line_buff[scan ++] = '\0';
			tokenval = line_buff + head;
			return WORD;
		}
		else if(ch == '\0') {
		    tokenval = line_buff + head;
			return WORD;
		}	
		else {
			scan ++;
		}
	}
}


int match(int t)
{
	if(lookahead == t) {
		lookahead = lexan();
		return 1;
	}
	else {
		return 0;
	}
}

void print_usage()
{
	printf("Usage:\r\n");
	printf(pcmd_obj->usage);
}
