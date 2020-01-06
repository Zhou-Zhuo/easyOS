/*
 * @file	incomplete_shell.c
 * @author	zz
 * @version	0.0
 */

#ifndef IMCOMPLETE_SHELL_H
#define IMCOMPLETE_SHELL_H

#include <stdint.h>
#include "user_lib.h"

#define SECTION(x) __attribute__ ((section(x)))

typedef struct {
	char *name;
	int (*cmd)(int, char *[]);
	char *usage;
} incom_sh_t;

#define INCOM_SH_CMD(name,cmd,usage) \
	incom_sh_t __incom_sh_cmd##name SECTION(".__incom_sh_cmd") = \
	{#name, cmd, usage }


#define INCOM_SH_PROMPT "incomsh> "	

void incom_mainloop();
void data_reset();
int read_line();
int run_cmd();
int find_cmd();
int lexan();
int match(int t);
void print_usage();
uint8_t string_cmp();

#endif
