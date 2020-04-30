/**********************************************************************

    > File Name: page_manager.h

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月23日 星期四 20时47分34秒

***********************************************************************/

#ifndef __PAGE_MANAGER_H__
#define __PAGE_MANAGER_H__

#include "input_manager.h"
#include "display_manager.h"

typedef struct _layout
{
	int top_left_x;
	int top_left_y;
	int bot_right_x;
	int bot_right_y;

	char *icon_name;
} layout_t;

typedef struct _page_layout {
	int bpp;
	int max_total_bytes;
	layout_t *layout;
} page_layout_t;

typedef struct _page_action
{
	char *name;

	void (*run)();
	int (*get_input_event)(page_layout_t *page_layout, input_event_t *input_event);
	int (*prepare)();

	struct _page_action *next;
} page_action_t;

int register_page_action(page_action_t *disp_opr);

void show_all_page_action(void);

page_action_t *get_page_action(char *name);

int page_manager_init(void);

int main_page_init(void);
int explore_page_init(void);
int auto_page_init(void);
int browse_page_init(void);
int setting_page_init(void);
int interval_page_init(void);

int generate_page(page_layout_t *page_layout, video_mem_t *video_mem);
int generic_get_input_event(page_layout_t *page_layout, input_event_t *input_event);

int get_new_id(char *name);

#endif /* __PAGE_MANAGER_H__ */

