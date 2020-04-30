/**********************************************************************

    > File Name: input_manager.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月21日 星期二 14时35分07秒

***********************************************************************/

#if defined(SELECT)
#include <sys/select.h>
#endif

#include "config.h"
#include "input_manager.h"

static input_operation_t *input_operation_head;
static input_event_t	input_event_global;

#if defined(THREAD)

static pthread_mutex_t	mutex 	= PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t	conv	= PTHREAD_COND_INITIALIZER;

#endif

int register_input_operation(input_operation_t *input_opr)
{
	input_operation_t *temp;

	//	如果链表上无设备
	if (!input_operation_head)
	{
		input_operation_head = input_opr;
		input_operation_head->next = NULL;
	}
	else	//	链表上有设备
	{
		temp = input_operation_head;

		while (temp->next)
			temp = temp->next;

		temp->next = input_opr;
		input_opr->next = NULL;
	}

	return 0;

}

void show_all_input_operation(void)

{
	input_operation_t	*temp = input_operation_head;
	int	i = 0;

	puts("input device : ");
	while (temp)
	{
		printf("%02d %s\n", i++, temp->name);
		temp = temp->next;
	}

}

input_operation_t *get_input_operation(char *name)
{
	input_operation_t *temp = input_operation_head;

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

#if defined(THREAD)
static void *input_event_worker(void *arg)
{
	input_event_t input_event_temp;

	int (*get_input_event_function)(input_event_t *input_event);
	get_input_event_function = (int (*)(input_event_t *))arg;

	while (1)
	{
		if (get_input_event_function(&input_event_temp) == 0)
		{
			pthread_mutex_lock(&mutex);
			input_event_global = input_event_temp;

			pthread_cond_signal(&conv);

			pthread_mutex_unlock(&mutex);
		}
	}

	return NULL;
}
#endif


int all_input_devices_init(void)
{
	input_operation_t *temp = input_operation_head;
	int ret = -1;

#if defined(THREAD)
	while (temp)
	{
		if (temp->device_init() == 0)
		{
			pthread_create(&temp->thread_id, NULL, input_event_worker, temp->get_input_event);
			ret = 0;
		}
		temp = temp->next;
	}
#endif
	return ret;
}

int get_input_event(input_event_t *input_event)
{
#if defined(THREAD)
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&conv, &mutex);

	*input_event = input_event_global;
	pthread_mutex_unlock(&mutex);
#endif

	return 0;
}

int input_manager_init(void)
{
	int ret = 0;

	ret = stdin_init();
	ret |= ts_init();

	return ret;
}


