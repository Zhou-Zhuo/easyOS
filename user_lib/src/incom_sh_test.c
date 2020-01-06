#include "user_lib.h"

int do_hello(int argc, char *argv[])
{
	if(argc == 0) {
		printf("Hello, Incomsh!\r\n");
	}
	else if(argc == 1) {
		printf("Hello, %s!\r\n", argv[0]);
	}
	else if(argc == 2) {
		printf("Hello, %s and %s!\r\n", argv[0], argv[1]);
	}
	else {
		print_usage();
		return 0;
	}

	return 1;

} 

INCOM_SH_CMD(hello, do_hello, 
		"hello [arg1] [arg2] - Say hello , 2 arguments most.\r\n"); 

