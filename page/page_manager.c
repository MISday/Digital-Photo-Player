/**********************************************************************

    > File Name: page_manager.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月23日 星期四 20时36分40秒

***********************************************************************/

#include <stdlib.h>

#include "config.h"
#include "page_manager.h"
#include "display_manager.h"
#include "input_manager.h"
#include "render.h"

static page_action_t *page_action_head;

int register_page_action(page_action_t *page_action)
{
	page_action_t *temp;

	if (!page_action_head)
	{
		page_action_head = page_action;
		page_action_head->next = NULL;
	}
	else
	{
		temp = page_action_head;

		while (temp->next)
			temp = temp->next;

		temp->next = page_action;
		page_action->next = NULL;
	}

	return 0;
}

void show_all_page_action(void)
{
	page_action_t	*temp = page_action_head;
	int	i = 0;

	puts("display device : ");
	while (temp)
	{
		printf("%02d %s\n", i++, temp->name);
		temp = temp->next;
	}
	
}

page_action_t *get_page_action(char *name)
{
	page_action_t *temp = page_action_head;

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

int generate_page(page_layout_t *page_layout, video_mem_t *video_mem)
{
	int ret;
	pixel_datas_t origin_icon_pixel_datas;
	pixel_datas_t icon_pixel_datas;
	layout_t *layout;

	layout = page_layout->layout;

	if (video_mem->picture_state != PS_GENERATED)
	{
		clear_video_mem(video_mem, COLOR_BACKGROUND);

		icon_pixel_datas.bpp		= page_layout->bpp;
		icon_pixel_datas.pixel_data = malloc(page_layout->max_total_bytes); 
		if (icon_pixel_datas.pixel_data == NULL)
		{
			return -1;
		}
	
		while(layout->icon_name)
		{
			ret = get_pixel_datas_for_icon(layout->icon_name, &origin_icon_pixel_datas);
			if (ret)
			{
				DEBUG_PRINTF("get_pixel_datas_for_icon %s error\n", layout->icon_name);
				return -1;
			}

			icon_pixel_datas.height = layout->bot_right_y - layout->top_left_y + 1;
			icon_pixel_datas.width	= layout->bot_right_x - layout->top_left_x + 1;
			icon_pixel_datas.line_bytes = icon_pixel_datas.width * icon_pixel_datas.bpp / 8;
			icon_pixel_datas.total_bytes = icon_pixel_datas.line_bytes * icon_pixel_datas.height;

			picture_zoom(&origin_icon_pixel_datas, &icon_pixel_datas);
			picture_merge(layout->top_left_x, layout->top_left_y, 
							&icon_pixel_datas, &video_mem->pixel_datas);

			free_pixel_datas_from_icon(&origin_icon_pixel_datas);
			layout++;
		}
		free(icon_pixel_datas.pixel_data);
		video_mem->picture_state = PS_GENERATED;
	}

	return 0;
}

int generic_get_input_event(page_layout_t *page_layout, input_event_t *input_event)
{
	input_event_t input_event_origin;
	int i = 0;
	int ret;
	layout_t *layout = page_layout->layout;
	
	/*
	 *	获得原始触摸屏数据
	 *	调用该函数后会使当前线程休眠
	 *	当获得输入数据时，将会唤醒当前线程
	 */
	ret = get_input_event(&input_event_origin);
	if (ret)
	{
		return -1;
	}

	if (input_event_origin.type != INPUT_TYPE_TOUCHSCREEN)
	{	
		return -1;
	}

	*input_event = input_event_origin;

	/* 确定触点的位置 */
	while (layout[i].icon_name)
	{
		if ((input_event_origin.x >= layout[i].top_left_x) && (input_event_origin.x <= layout[i].bot_right_x) && \
			(input_event_origin.y >= layout[i].top_left_y) && (input_event_origin.y <= layout[i].bot_right_y))
		{
			return i;
		}
		else
		{
			i++;
		}
	}

	return -1;	

}

int get_new_id(char *name)
{
	return (int)(name[0]) + (int)(name[1]) + (int)(name[2]) + (int)(name[3]);
}

int page_manager_init(void)
{
	int		ret = 0;

	ret = main_page_init();
	if (ret)
	{
		return -1;
	}

	ret = setting_page_init();
	if (ret)
	{
		return -1;
	}

	ret = interval_page_init();
	if (ret)
	{
		return -1;
	}

	ret = browse_page_init();
	if (ret)
	{
		return -1;
	}	

	return ret;
}


