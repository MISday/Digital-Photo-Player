/**********************************************************************

    > File Name: stdout.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月21日 星期二 14时48分52秒

***********************************************************************/


#include "config.h"
#include "debug_manager.h"

static int stdout_debug_init(void);
static int stdout_debug_exit(void);
static int stdout_debug_print(char *str);

static debug_operation_t stdout_debug_operation = 
{
	.name = "stdout",

	.flag = 1,

	.debug_init		= stdout_debug_init,
	.debug_exit 	= stdout_debug_exit,
	.debug_print	= stdout_debug_print,
};

static int stdout_debug_init(void)
{
	return 0;
}

static int stdout_debug_exit(void)
{
	return 0;
}

static int stdout_debug_print(char *str)
{
	printf("%s", str);
	return strlen(str);
}

int stdout_init(void)
{
	return register_debug_operation(&stdout_debug_operation);
}


