/**********************************************************************

    > File Name: interval_page.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月28日 星期二 16时53分04秒

***********************************************************************/

#include "config.h"
#include "page_manager.h"

#include "display_manager.h"
#include "picture/picture_parser.h"
#include "render.h"
#include "fonts_manager.h"
#include "input_manager.h"

static void interval_page_run();
static int interval_page_get_input_event();
static int interval_page_prepare();	

static layout_t interval_page_icon_layout[] = 
{
	{0, 0, 0, 0, "inc.bmp"},
	{0, 0, 0, 0, "time.bmp"},
	{0, 0, 0, 0, "dec.bmp"},
	{0, 0, 0, 0, "ok.bmp"},
	{0, 0, 0, 0, "cancel.bmp"},
	{0, 0, 0, 0, NULL},
};

static layout_t interval_number_layout;
static int interval_number = 7;

static page_layout_t interval_page_layout = 
{
	.max_total_bytes	= 0,
	.layout				= interval_page_icon_layout,
};

static page_action_t interval_page_action = 
{
	.name = "interval",

	.run				= interval_page_run,
	.get_input_event	= interval_page_get_input_event,
	.prepare			= interval_page_prepare,
};

static void calc_interval_page_layout(page_layout_t *page_layout)
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
	 *	  ----------------------
	 *							1/2 * height
	 *			inc.bmp 		height * 28 / 128	   
	 *		   time.bmp 		height * 72 / 128
	 *			dec.bmp 		height * 28 / 128	   
	 *							1/2 * height
	 *	  ok.bmp	 cancel.bmp 1/2 * height
	 *							1/2 * height
	 *	  ----------------------
	 *    对长x,		宽y的图象进行映射
	 *    为长width,	宽height
	 */
	height	= y_res / 3;
	width	= height;
	start_y	= height / 2;

	/* inc.bmp */
	layout[0].top_left_y 	= start_y;
	layout[0].top_left_x 	= (x_res - width * 52 / 128) / 2;	/* width * 52 / 128表示图象长度 */
	layout[0].bot_right_y	= start_y + height * 28 / 128 - 1;
	layout[0].bot_right_x	= layout[0].top_left_x + width * 52 / 128 - 1;
	
	/* time.bmp */
	layout[1].top_left_y 	= layout[0].bot_right_y + 1;
	layout[1].top_left_x 	= (x_res - width) / 2;	
	layout[1].bot_right_y	= layout[1].top_left_y + height * 72 / 128 - 1;
	layout[1].bot_right_x	= layout[1].top_left_x + width - 1;


	/* dec.bmp */
	layout[2].top_left_y 	= layout[1].bot_right_y + 1;
	layout[2].top_left_x 	= (x_res - width * 52 / 128) / 2;	/* width * 52 / 128表示图象长度 */
	layout[2].bot_right_y	= layout[2].top_left_y + height * 28 / 128 - 1;
	layout[2].bot_right_x	= layout[2].top_left_x + width * 52 / 128 - 1;

	/* ok.bmp */
	layout[3].top_left_y 	= layout[2].bot_right_y + height / 2 + 1;
	layout[3].top_left_x 	= (x_res - width) / 3;	
	layout[3].bot_right_y	= layout[3].top_left_y + height / 2 - 1;
	layout[3].bot_right_x	= layout[3].top_left_x + width / 2 - 1;

	/* cancel.bmp */
	layout[4].top_left_y 	= layout[3].top_left_y;
	layout[4].top_left_x 	= layout[3].top_left_x * 2 + width / 2;	
	layout[4].bot_right_y	= layout[3].bot_right_y;
	layout[4].bot_right_x	= layout[4].top_left_x + width / 2 - 1;

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

	/* 
	 * 计算数字 
	 * 原图为128*72，两个数字的大小为52*40，有所缩放
	 */
	width = layout[1].bot_right_x - layout[1].top_left_x + 1; 
	height= layout[1].bot_right_y - layout[1].top_left_y + 1;

	interval_number_layout.top_left_x = layout[1].top_left_x + (128 - 52)/2 * width / 128;
	interval_number_layout.top_left_y = layout[1].top_left_y + (72 - 40)/2 * height / 72;
	
	interval_number_layout.bot_right_x= layout[1].bot_right_x - (128 - 52)/2 * width / 128 + 1;
	interval_number_layout.bot_right_y= layout[1].bot_right_y - (72 - 40)/2 * height /72 + 1;
}

static int generate_interval_page_special_icon(int number, video_mem_t *video_mem)
{
	unsigned int font_size;
	char number_array[3];
	int ret;

	font_size = interval_number_layout.bot_right_y - interval_number_layout.top_left_y;
	set_font_size(font_size);

	if (number > 59)
	{
		return -1;
	}

	snprintf(number_array, 3, "%02d", number);

	ret = merge_string_to_center_of_rect_in_video_mem(&interval_number_layout, (unsigned char *)number_array, video_mem);

	return ret;
}

static void show_interval_page(page_layout_t	*page_layout)
{
	video_mem_t *video_mem;
	layout_t *layout = page_layout->layout;
	int ret;

	/* 1.	获得显存 */
	video_mem = get_video_mem(get_new_id("interval"), 1);
	if (video_mem == NULL)
	{
		DEBUG_PRINTF("can't get video mem for main page\n");
		return ;
	}

	/* 2.	描画数据     */
	if (layout[0].bot_right_x == 0)
	{
		calc_interval_page_layout(page_layout);
	}

	ret = generate_page(page_layout, video_mem);

	ret = generate_interval_page_special_icon(interval_number, video_mem);
	if (ret)
	{
		DEBUG_PRINTF("generate_interval_page_special_icon failed\n");
	}
	
	/* 3.	刷新至设备 */
	flush_video_mem_to_dev(video_mem);

	/* 4.	释放显存 */
	put_video_mem(video_mem);	
}

static int interval_page_get_input_event(page_layout_t *page_layout, input_event_t *input_event)
{
	return generic_get_input_event(page_layout, input_event);
}

static int time_ms_between(struct timeval time_start, struct timeval time_end)
{
	int ms;
	ms = (time_end.tv_sec - time_start.tv_sec) * 1000 + (time_end.tv_usec - time_start.tv_usec) / 1000;
	return ms;
}

static void interval_page_run(void)
{
	input_event_t input_event_pre;
	input_event_t input_event;
	int index;

	int pressed = 0;
	int pre_index = -1;
	int fast = 0;

	int number = interval_number;

	video_mem_t *dev_video_mem;

	dev_video_mem = get_dev_video_mem();
	/* 1.	显示界面 */
	show_interval_page(&interval_page_layout);

	/* 2.	启动prepare线程 */

	/* 3.	处理input事件 */
	while (1)
	{
		index = interval_page_get_input_event(&interval_page_layout, &input_event);
		DEBUG_PRINTF("\n");
		DEBUG_PRINTF("index : %d\n", index);
		if (input_event.pressure == 0)
		{
			/* 松开 */
			if (pressed)
			{
				fast = 0;
			
				release_button(&interval_page_icon_layout[pre_index]);
				pressed = 0;

				if (index == pre_index)		/* 按下与抬起在一个按钮上 */
				{
					switch (pre_index)
					{
						case 0:	/* inc */
						{
							number++;
							if (number == 60)
							{
								number = 0;
							}

							generate_interval_page_special_icon(number, dev_video_mem);
							break;
						}

						case 2:	/* dec */
						{
							number--;
							if (number == -1)
							{
								number = 59;
							}

							generate_interval_page_special_icon(number, dev_video_mem);						
							break;
						}

						case 3:	/* ok */
						{
							interval_number = number;
						
							return ;
							break;
						}

						case 4:	/* cancel */
						{
							return ; 
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
			/* 按下 */
			if (index != -1)
			{
				if (!pressed && (index != 1))		//	未按下目标按钮
				{
					pressed = 1;
					pre_index = index;
					input_event_pre = input_event;
					press_button(&interval_page_icon_layout[pre_index]);
				}

				if ((pre_index == 0) || (pre_index == 2))
				{
	
					if (fast && (time_ms_between(input_event_pre.time, input_event.time) > 50))
					{			
						number = pre_index ? (number - 1) : (number + 1);
						if (number == 60)
						{
							number = 0;
						}		
						else if (number == -1)
						{
							number = 59;
						}
						generate_interval_page_special_icon(number, dev_video_mem);
						input_event_pre = input_event;
					}

					if (time_ms_between(input_event_pre.time, input_event.time) > 2000)
					{
						fast = 1;
						input_event_pre = input_event;
					}
				}
			}

		}
	}
}

static int interval_page_prepare(void)
{
	
	return 0;
}


int interval_page_init(void)
{
	return register_page_action(&interval_page_action);
}



