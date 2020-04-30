/**********************************************************************

    > File Name: setting_page.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月23日 星期四 20时45分22秒

***********************************************************************/

#include <stdlib.h>

#include "config.h"
#include "page_manager.h"
#include "display_manager.h"
#include "picture/picture_parser.h"
#include "render.h"
#include "input_manager.h"

static void setting_page_run();
static int setting_page_get_input_event(page_layout_t *page_layout, input_event_t *input_event);
static int setting_page_prepare();	

static layout_t setting_page_icon_layout[] = 
{
	{0, 0, 0, 0, "select_fold.bmp"},
	{0, 0, 0, 0, "interval.bmp"},
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, NULL},
};

static page_layout_t setting_page_layout = 
{
	.max_total_bytes	= 0,
	.layout				= setting_page_icon_layout,	
};


static page_action_t setting_page_action = 
{
	.name = "setting",

	.run				= setting_page_run,
	.get_input_event	= setting_page_get_input_event,
	.prepare			= setting_page_prepare,
};

static void calc_setting_page_layout(page_layout_t *page_layout)
{
	layout_t *layout;

	int x_res;
	int y_res;
	int bpp;

	int height;
	int width;
	int start_y;

	int temp_total_bytes;

	int i;
	
	layout = page_layout->layout;
	get_display_resolution(&x_res, &y_res, &bpp);
	page_layout->bpp = bpp;	

	
	/*   
	 *    ----------------------
	 *                           1/2 * iHeight
	 *          select_fold.bmp  iHeight
	 *                           1/2 * iHeight
	 *          interval.bmp     iHeight
	 *                           1/2 * iHeight
	 *          return.bmp       iHeight
	 *                           1/2 * iHeight
	 *    ----------------------
	 */

	height	= y_res / 5;
	width	= height;
	start_y= height / 2;

	/* select_fold.bmp */
	layout[0].top_left_y = start_y;
	layout[0].top_left_x = (x_res - width * 2) / 2;
	layout[0].bot_right_y= layout[0].top_left_y + height - 1;
	layout[0].bot_right_x= layout[0].top_left_x + width * 2 - 1;

	/* interval.bmp */
	layout[1].top_left_y = layout[0].bot_right_y + height / 2 + 1;
	layout[1].top_left_x = (x_res - width * 2) / 2;
	layout[1].bot_right_y= layout[1].top_left_y + height - 1;
	layout[1].bot_right_x= layout[1].top_left_x + width * 2 - 1;

	/* return.bmp */
	layout[2].top_left_y = layout[1].bot_right_y + height / 2 + 1;
	layout[2].top_left_x = (x_res - width) / 2;
	layout[2].bot_right_y= layout[2].top_left_y + height - 1;
	layout[2].bot_right_x= layout[2].top_left_x + width - 1;

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

static void show_setting_page(page_layout_t	*page_layout)
{
	video_mem_t *video_mem;
	layout_t *layout = page_layout->layout;
	int ret;

	/* 1.	获得显存 */
	video_mem = get_video_mem(get_new_id("setting"), 1);
	if (video_mem == NULL)
	{
		DEBUG_PRINTF("can't get video mem for main page\n");
		return ;
	}

	/* 2.	描画数据     */
	if (layout[0].bot_right_x == 0)
	{
		calc_setting_page_layout(page_layout);
	}

	ret = generate_page(page_layout, video_mem);

	/* 3.	刷新至设备 */
	flush_video_mem_to_dev(video_mem);

	/* 4.	释放显存 */
	put_video_mem(video_mem);	
}

static int setting_page_get_input_event(page_layout_t *page_layout, input_event_t *input_event)
{
	return generic_get_input_event(page_layout, input_event);
}

static void setting_page_run(void)
{
	input_event_t input_event;
	int index;

	int pressed = 0;
	int pre_index = -1;
	/* 1.	显示界面 */
	show_setting_page(&setting_page_layout);

	/* 2.	启动prepare线程 */

	/* 3.	处理input事件 */
	while (1)
	{
		index = setting_page_get_input_event(&setting_page_layout, &input_event);

		if (input_event.pressure == 0)
		{
			if (pressed)
			{
				release_button(&setting_page_icon_layout[pre_index]);
				pressed = 0;

				if (index == pre_index)		/* 按下与抬起在一个按钮上 */
				{
					switch (pre_index)
					{
						case 0:		/* select_fold */
						{
							break;
						}

						case 1:		/* interval */
						{
							get_page_action("interval")->run();
							show_setting_page(&setting_page_layout);
							break;
						}

						case 2:		/* return */
						{
							return;
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
					press_button(&setting_page_icon_layout[pre_index]);
				}
			}

		}
	}
}

static int setting_page_prepare(void)
{
	
	return 0;
}

int setting_page_init(void)
{
	return register_page_action(&setting_page_action);
}


