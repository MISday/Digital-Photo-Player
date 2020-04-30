/**********************************************************************

    > File Name: input_manager.h

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月21日 星期二 14时23分38秒

***********************************************************************/

#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include <sys/time.h>
#include <pthread.h>


#define INPUT_TYPE_STDIN        0
#define INPUT_TYPE_TOUCHSCREEN  1

#define INPUT_VALUE_UP          0   
#define INPUT_VALUE_DOWN        1
#define INPUT_VALUE_EXIT        2
#define INPUT_VALUE_UNKNOWN     -1 

typedef struct _input_event
{
	struct timeval time;
	int type;
	int	x;
	int y;
	int key;
	int pressure;
} input_event_t;

typedef struct _input_operation
{
	char *name;
	
#if defined(SELECT)
	int	fd;
#endif

#if defined(THREAD)
	pthread_t thread_id;
#endif

	int (*device_init)(void);
	int (*device_exit)(void);
	int (*get_input_event)(input_event_t *input_event);

	struct _input_operation *next;
} input_operation_t;

int register_input_operation(input_operation_t *input_opr);

void show_all_input_operation(void);

input_operation_t *get_input_operation(char *name);

int get_input_event(input_event_t *input_event);

int all_input_devices_init(void);

int input_manager_init(void);

int stdin_init(void);
int ts_init(void);

#endif /* __INPUT_MANAGER_H__ */

