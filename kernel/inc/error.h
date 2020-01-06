#ifndef ERROR_H
#define ERROR_H

#define ERR_NONE 0

//task errors:
#define ERR_NO_SUCH_PRIO -1
#define ERR_NO_MORE_TCB -2
#define ERR_NO_SUCH_TASK -3
#define ERR_SYS_TASK -4

//event errors;
#define ERR_NO_SUCH_EVENT -1
#define ERR_NO_MORE_ECB -2
#define ERR_EV_TYPE -3
#define ERR_EV_BUSY -4
#define ERR_EV_TIMEOUT -5

#endif
