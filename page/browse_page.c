/**********************************************************************

    > File Name: browse_page.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月23日 星期四 20时43分56秒

***********************************************************************/

#include "config.h"
#include "page_manager.h"


#include "display_manager.h"
#include "picture/picture_parser.h"
#include "render.h"
#include "fonts_manager.h"
#include "input_manager.h"

static int browse_page_run();
static int browse_page_get_input_event(page_layout_t *page_layout, input_event_t *input_event);
static int browse_page_prepare();	

static layout_t menu_icons_layout[5] = 
{
	{0, 0, 0, 0, "up.bmp"},
	{0, 0, 0, 0, "select.bmp"},
	{0, 0, 0, 0, "pre_page.bmp"},
	{0, 0, 0, 0, "next_page.bmp"},
	{0, 0, 0, 0, NULL},	
};

static page_layout_t browse_page_menu_icons_layout = 
{
	.max_total_bytes	= 0,
	.layout				= menu_icons_layout,		
};

static page_action_t browse_page_action = 
{
	.name = "browse",

	.run				= browse_page_run,
	.get_input_event	= browse_page_get_input_event,
	.prepare			= browse_page_prepare,
};

static void calc_browse_page_layout(page_layout_t *page_layout)
{
	layout_t *layout;

	int x_res;
	int y_res;
	int bpp;

	int height;
	int width;

	int temp_total_bytes;

	int i;
	
	layout = page_layout->layout;
	get_display_resolution(&x_res, &y_res, &bpp);
	page_layout->bpp = bpp;	

	
	/*	 iYres/4
	 *	  ----------------------------------
	 *	   up		  
	 *
	 *	  select
	 *
	 *	  pre_page
	 *	
	 *	 next_page
	 *
	 *	  ----------------------------------
	 */
	height = y_res / 4;
	width = height;

	/* up.bmp */
	layout[0].top_left_y = 0;
	layout[0].top_left_x = 0;
	layout[0].bot_right_y= layout[0].top_left_y + height - 1;
	layout[0].bot_right_x= layout[0].top_left_x + width - 1;

	/* select.bmp */
	layout[1].top_left_y = layout[0].bot_right_y + 1;
	layout[1].top_left_x = 0;
	layout[1].bot_right_y= layout[1].top_left_y + height - 1;
	layout[1].bot_right_x= layout[1].top_left_x + width - 1;

	/* pre_page.bmp */
	layout[2].top_left_y = layout[1].bot_right_y + 1;
	layout[2].top_left_x = 0;
	layout[2].bot_right_y= layout[2].top_left_y + height - 1;
	layout[2].bot_right_x= layout[2].top_left_x + width - 1;

	/* next_page.bmp */
	layout[3].top_left_y = layout[2].bot_right_y + 1;
	layout[3].top_left_x = 0;
	layout[3].bot_right_y= layout[3].top_left_y + height - 1;
	layout[3].bot_right_x= layout[3].top_left_x + width - 1;	
	
	i = 0;
	while (layout[i].icon_name)
	{
		temp_total_bytes = (layout[i].bot_right_x - layout[i].top_left_x + 1) *
							(layout[i].bot_right_y - layout[i].top_left_y + 1) * bpp / 8;
		if (page_layout->max_total_bytes < temp_total_bytes)
		{
			page_layout->max_total_bytes = temp_total_bytes;
		}
		i++;
	}

}


static void show_browse_page(page_layout_t	*page_layout)
{
	video_mem_t *video_mem;
	layout_t *layout = page_layout->layout;
	int ret;
	
	/* 1.	获得显存 */
	video_mem = get_video_mem(get_new_id("browse"), 1);
	if (video_mem == NULL)
	{
		DEBUG_PRINTF("can't get video mem for main page\n");
		return ;
	}	

	/* 2.	描画数据 */
	if (layout[0].bot_right_x == 0)
	{
		calc_browse_page_layout(page_layout);
	}
	
	ret = generate_page(page_layout, video_mem);
	if (ret)
	{
		DEBUG_PRINTF("generate_page error\n");
		return;
	}

	/* 3.	刷新至设备 */
	flush_video_mem_to_dev(video_mem);

	/* 4.	释放显存 */
	put_video_mem(video_mem);
}


static int browse_page_get_input_event(page_layout_t *page_layout, input_event_t *input_event)
{
	return generic_get_input_event(page_layout, input_event);
}

static int browse_page_run(void)
{
	input_event_t input_event;
	int index;

	int pressed = 0;
	int pre_index = -1;
	/* 1.	显示界面 */
	show_browse_page(&browse_page_menu_icons_layout);

	/* 2.	启动prepare线程 */

	/* 3.	处理input事件 */
	while (1)
	{
		index = browse_page_get_input_event(&browse_page_menu_icons_layout, &input_event);

		if (input_event.pressure == 0)
		{
			if (pressed)
			{
				release_button(&menu_icons_layout[pre_index]);
				pressed = 0;

				if (index == pre_index)		/* 按下与抬起在一个按钮上 */
				{
					switch (pre_index)
					{
						case 0:		/* up.bmp */
						{
							return;
							break;
						}

						case 1:		/* select.bmp */
						{
							break;
						}

						case 2:		/* pre_page.bmp */
						{
							break;
						}

						case 3:		/* next_page.bmp */
						{
							break;
						}

						default:
						{
							break;
						}
					}
				}
				
				pre_index = -1;
			}
		}
		else
		{
			if (index != -1)
			{
				if (!pressed)		//	未按下
				{
					pressed = 1;
					pre_index = index;
					press_button(&menu_icons_layout[pre_index]);
				}
			}

		}
	}


	return 0;
}

static int browse_page_prepare()
{
	return 0;
}


int browse_page_init(void)
{
	return register_page_action(&browse_page_action);
}


