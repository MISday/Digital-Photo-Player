/**********************************************************************

    > File Name: debug_manager.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月21日 星期二 14时23分05秒

***********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "config.h"
#include "debug_manager.h"

static debug_operation_t *debug_operation_head;
static int debug_level_limit = 8;

int register_debug_operation(debug_operation_t *debug_opr)

{
	debug_operation_t *temp;

	//	如果链表上无设备
	if (!debug_operation_head)
	{
		debug_operation_head = debug_opr;
		debug_operation_head->next = NULL;
	}
	else	//	链表上有设备
	{
		temp = debug_operation_head;

		while (temp->next)
			temp = temp->next;

		temp->next = debug_opr;
		debug_opr->next = NULL;
	}

	return 0;
}

void show_all_debug_operation(void)

{
	debug_operation_t	*temp = debug_operation_head;
	int	i = 0;

	puts("display device : ");
	while (temp)
	{
		printf("%02d %s\n", i++, temp->name);
		temp = temp->next;
	}
	
}

debug_operation_t *get_debug_operation(char *name)
{
	debug_operation_t *temp = debug_operation_head;

	while (temp)
	{
		if (STRCMP(temp->name, ==, name))
		{
			return temp;
		}
		temp = temp->next;
	}

	return NULL;
}

/* str = "dbglevel=6" */
int set_debug_level(char *str)
{
	debug_level_limit = str[9] - '0';
	return 0;
}

/*
 * stdout=0			   : 关闭stdout打印
 * stdout=1			   : 打开stdout打印
 * netprint=0		   : 关闭netprint打印
 * netprint=1		   : 打开netprint打印
 */
int set_debug_device(char *str)
{
	char *str_temp;
	char str_name[128];
	debug_operation_t *temp;

	str_temp = strchr(str, '=');
	if (!str_temp)
		return -1;
	else
	{
		strncpy(str_name, str, str_temp-str);	
		str_name[str_temp-str] = '\0';
		temp = get_debug_operation(str_name);
		if (!temp)
			return -1;
		
		if (str_temp[1] == '0')
			temp->flag = 0;
		else
			temp->flag = 1;

		return 0;
	}
	
}



int debug_printf(const char *format, ...)
{
	char temp_buf[1024];
	va_list arg;
	int num;	
	char *temp;
	int dbglevel = DEFAULT_DBGLEVEL;
	debug_operation_t *debug_opr_temp = debug_operation_head;

	va_start(arg, format);
	num = vsprintf(temp_buf, format, arg);
	va_end(arg);

	temp_buf[num] = '\0';

	temp = temp_buf;

	if ((temp_buf[0] == '<') && (temp_buf[2] == '>'))
	{
		dbglevel = temp_buf[1] - '0';
		if (dbglevel >= 0 && dbglevel <= 9)
		{
			temp = temp_buf + 3;
		}
		else
		{
			dbglevel = DEFAULT_DBGLEVEL;
		}
	}

	if (dbglevel > debug_level_limit)
	{
		return -1;
	}

	while (debug_opr_temp)
	{
		if (debug_opr_temp->flag)
			debug_opr_temp->debug_print(temp);

		debug_opr_temp = debug_opr_temp->next;
	}

	return 0;
}

int all_debug_devices_init(void)
{
	debug_operation_t *temp = debug_operation_head;
	int ret = -1;

	while (temp)
	{
		if (temp->flag && temp->debug_init)
		{
			ret = temp->debug_init();
		}
		temp = temp->next;
	}

	return ret;
}

int debug_manager_init(void)
{
	int		err;

	err = stdout_init();
	err |= netprint_init();

	return err;
}


