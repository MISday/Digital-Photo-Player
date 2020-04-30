/**********************************************************************

    > File Name: explore_page.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月23日 星期四 20时43分23秒

***********************************************************************/

#include "config.h"
#include "paga_manager.h"

static int explore_page_run();
static int explore_page_get_input_event();
static int explore_page_prepare();	

static page_action_t explore_page_action = 
{
	.name = "explore",
	
	.run				= explore_page_run,
	.get_input_event	= explore_page_get_input_event,
	.prepare			= explore_page_prepare,

	
};

static int explore_page_run()
{
	return 0;
}

static int explore_page_get_input_event()
{
	return 0;
}

static int explore_page_prepare()
{
	return 0;
}


int explore_page_init(void)
{
	return register_page_action(&explore_page_action);
}



