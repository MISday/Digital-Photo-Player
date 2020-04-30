/**********************************************************************

    > File Name: stdin.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月16日 星期四 22时37分30秒

***********************************************************************/

#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <stdlib.h>


#include "config.h"
#include "input_manager.h"

static int stdin_device_init(void);
static int stdin_device_exit(void);
static int stdin_get_input_event(input_event_t *input_event);

static input_operation_t stdin_input_operation = 
{
	.name = "stdin",

	.device_init = stdin_device_init,
	.device_exit = stdin_device_exit,
	.get_input_event = stdin_get_input_event,
};

static int stdin_device_init(void)
{
	struct termios tty_state;

	//	获取terminal状态
	tcgetattr(STDIN_FILENO, &tty_state);

	//	关闭典型模式
	tty_state.c_lflag &= ~ICANON;

	//	输入读取的最小数量
	tty_state.c_cc[VMIN] = 1;	//	有一个数据就立即返回

	//	设置terminal状态
	tcsetattr(STDIN_FILENO, TCSANOW, &tty_state);

#if defined(SELECT)
	stdin_input_operation.fd = STDIN_FILENO;
#endif

	return 0;
}

static int stdin_device_exit(void)
{
	struct termios tty_state;

	tcgetattr(STDIN_FILENO, &tty_state);

	//	开典型模式
	tty_state.c_lflag |= ICANON;

	tcsetattr(STDIN_FILENO, TCSANOW, &tty_state);

	return 0;
}

/* 利用stdin */
static int stdin_get_input_event(input_event_t *input_event)
{
	fd_set read_fds;
	struct timeval tv;
	char c;

	FD_ZERO(&read_fds);
	FD_SET(STDIN_FILENO, &read_fds);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	select(STDIN_FILENO+1, &read_fds, NULL, NULL, &tv);

	if (FD_ISSET(STDIN_FILENO, &read_fds))
	{
		input_event->type = INPUT_TYPE_STDIN;
		gettimeofday(&input_event->time, NULL);

		c = fgetc(stdin);

#if 0
		if (c == 'u')
		{
			input_event->value = INPUT_VALUE_UP;
		}
		else if (c == 'n')
		{
			input_event->value = INPUT_VALUE_DOWN;
		}
		else if (c == 'q')
		{
			input_event->value = INPUT_VALUE_EXIT;
		}
		else
		{
			input_event->value = INPUT_VALUE_UNKNOWN;
		}
#endif
		return 0;
	}
	else
		return -1;

}


int stdin_init(void)
{
	return register_input_operation(&stdin_input_operation);
}
