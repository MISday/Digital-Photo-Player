/**********************************************************************

    > File Name: touchscreen.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月21日 星期二 14时31分17秒

***********************************************************************/

#include <stdlib.h>
#include <tslib.h>

#include "config.h"
#include "input_manager.h"
#include "display_manager.h"

#define TOUCH_SCREEN_PATH "/dev/input/event2"

static struct tsdev *ts_dev;
static int			xres;
static int			yres;

static int ts_device_init(void);
static int ts_device_exit(void);
static int ts_get_input_event(input_event_t *input_event);

static input_operation_t ts_input_operation = 
{
	.name = "touchscreen",

	.device_init = ts_device_init,
	.device_exit = ts_device_exit,
	.get_input_event = ts_get_input_event,	
};

static int ts_device_init(void)
{
	char *ts_name = NULL;
	int bpp;

	if ((ts_name = getenv("TSLIB_TSDEVICE")) != NULL)
	{
		ts_dev = ts_open(ts_name, 1);
	}
	else
	{
		ts_dev = ts_open(TOUCH_SCREEN_PATH, 1);
	}

	if (!ts_dev)
	{
		DEBUG_PRINTF("ts_open error!\n");
		return -1;
	}

	if (ts_config(ts_dev))
	{
		DEBUG_PRINTF("ts_config error!\n");
		return -1;
	}


	if (get_display_resolution(&xres, &yres, &bpp) != 0)
	{
		DEBUG_PRINTF("get_display_resolution error!\n");
		return -1;
	}

	return 0;
}

static int ts_device_exit(void)
{
	return 0;
}

static int ts_get_input_event(input_event_t *input_event)
{
	struct ts_sample sample;
	struct ts_sample sample_pre;
	int ret = 0;

	while (1)
	{
		ret = ts_read(ts_dev, &sample, 1);		
		if (ret == 1)
		{
			/* 如果x,y,pressure都没变，可以返回上一次的值 */
			input_event->time		= sample.tv;
			input_event->type		= INPUT_TYPE_TOUCHSCREEN;
			input_event->x 			= sample.x;
			input_event->y			= sample.y;
			input_event->pressure	= sample.pressure;

			return 0;
		}
		else
		{
			return -1;
		}

	}

}



int ts_init(void)
{
	return register_input_operation(&ts_input_operation);
}

