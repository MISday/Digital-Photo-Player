/**********************************************************************

    > File Name: auto_page.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月23日 星期四 20时45分14秒

***********************************************************************/

#include "config.h"
#include "page_manager.h"

static int auto_page_run();
static int auto_page_get_input_event();
static int auto_page_prepare();	


static page_action_t auto_page_action = 
{
	.name = "auto",

	.run				= auto_page_run,
	.get_input_event	= auto_page_get_input_event,
	.prepare			= auto_page_prepare,
};

static int auto_page_run(void)
{
	return 0;
}

static int auto_page_get_input_event()
{
	return 0;
}

static int auto_page_prepare()
{
	return 0;
}


int auto_page_init(void)
{
	return register_page_action(&auto_page_action);
}


