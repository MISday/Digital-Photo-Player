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
#include "file.h"

/* 图标是一个正方体, "图标+名字"也是一个正方体
 *   --------
 *   |  图  |
 *   |  标  |
 * ------------
 * |   名字   |
 * ------------
 */
#define DIR_FILE_ICON_WIDTH    80
#define DIR_FILE_ICON_HEIGHT   DIR_FILE_ICON_WIDTH
#define DIR_FILE_NAME_HEIGHT   40
#define DIR_FILE_NAME_WIDTH   (DIR_FILE_ICON_HEIGHT + DIR_FILE_NAME_HEIGHT)
#define DIR_FILE_ALL_WIDTH    DIR_FILE_NAME_WIDTH
#define DIR_FILE_ALL_HEIGHT   DIR_FILE_ALL_WIDTH


static void browse_page_run();
static int browse_page_get_input_event(page_layout_t *page_layout, input_event_t *input_event);
static int browse_page_prepare();	


/* 菜单 */
static layout_t menu_icons_layout[5] = 
{
	{0, 0, 0, 0, "up.bmp"},
	{0, 0, 0, 0, "select.bmp"},
	{0, 0, 0, 0, "pre_page.bmp"},
	{0, 0, 0, 0, "next_page.bmp"},
	{0, 0, 0, 0, NULL},	
};


/* 整个菜单界面 */
static page_layout_t browse_page_menu_icons_layout = 
{
	.max_total_bytes	= 0,
	.layout				= menu_icons_layout,		
};

/* 文件目录显示 */
static char *dir_icon_name	=	"fold_closed.bmp";
static char *file_icon_name	=	"file.bmp";
static layout_t *dir_and_file_layout;
static page_layout_t browse_page_dir_and_file_layout = 
{
	.max_total_bytes	= 0,
	
};

static int dir_file_num_per_col;
static int dir_file_num_per_row;

/* 目录内容 */
static dir_content_t **dir_contents;		/* 数组：存有目录下文件与目录的名字 */
static int dir_contents_number;			/* 记录上述数组的大小 */
static int start_index = 0;				/* 确定被现实的第一项是上述数组中的哪一项 */

/* 当前显示目录 */
static char cur_dir[256] = DEFAULT_DIR;


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

static int calc_browse_page_dir_and_files_layout(void)
{
	int x_res;
	int y_res;
	int bpp;

	int top_left_x;
	int top_left_y;
	int bot_right_x;
	int bot_right_y;

	int top_left_x_bak;

	int icon_width;
	int icon_height;

	int number_per_row;
	int number_per_col;

	int delta_x;
	int delta_y;

	int i;
	int j;
	int k = 0;

	get_display_resolution(&x_res, &y_res, &bpp);

	/*	 iYres/4
	 *	  ----------------------------------
	 *	   up      |
	 *             |
	 *    select   |
	 *             |     目录和文件
	 *    pre_page |
	 *             |
	 *   next_page |
	 *             |
	 *	  ----------------------------------
	 */	
	top_left_x	= menu_icons_layout[0].bot_right_x + 1;
	top_left_y	= 0;
	bot_right_x	= x_res - 1;
	bot_right_y = y_res - 1;

	/* 确定一行显示多少"目录或文件"，显示多少行 */
	icon_width = DIR_FILE_ALL_WIDTH;
	icon_height= icon_width;

	/* 计算图标之间间隔 */
	number_per_row = (bot_right_x - top_left_x + 1) / icon_width;
	while(1)
	{
		delta_x = (bot_right_x - top_left_x + 1) - icon_width * number_per_row;
		if ((delta_x / (number_per_row + 1)) < 10)	/* 图标间间隔大于10像素 */
			number_per_row--;
		else
			break;
	}

	number_per_col = (bot_right_y - top_left_y + 1) / icon_height;
	while(1)
	{
		delta_y = (bot_right_y - top_left_y + 1) - icon_height * number_per_col;
		if ((delta_y / (number_per_col + 1)) < 10)
			number_per_col--;
		else
			break;
	}

	/* 图标间间隔 */
	delta_x = delta_x / (number_per_row + 1);
	delta_y = delta_y / (number_per_col + 1);

	dir_file_num_per_col = number_per_col;
	dir_file_num_per_row = number_per_row;

	/*
	 *	可以表示dir_file_num_per_col * dir_file_num_per_row 个目录或文件
	 *	分配两倍+1个layout结构体，最后一个用于存NULL
	 */
	dir_and_file_layout = malloc(sizeof(layout_t) * 2 * (dir_file_num_per_col * dir_file_num_per_row + 1));
	if (dir_and_file_layout == NULL)
	{
		DEBUG_PRINTF("malloc error\n");
		return -1;
	}

	browse_page_dir_and_file_layout.top_left_x = top_left_x;
	browse_page_dir_and_file_layout.top_left_y = top_left_y;
	browse_page_dir_and_file_layout.bot_right_x = bot_right_x;
	browse_page_dir_and_file_layout.bot_right_y = bot_right_y;
	browse_page_dir_and_file_layout.bpp = bpp;
	browse_page_dir_and_file_layout.layout = dir_and_file_layout;
	browse_page_dir_and_file_layout.max_total_bytes = DIR_FILE_ALL_WIDTH * DIR_FILE_ALL_HEIGHT * bpp / 8;
    
    /* 确定图标和名字的位置 
     *
     * 图标是一个正方体, "图标+名字"也是一个正方体
     *   --------
     *   |  图  |
     *   |  标  |
     * ------------
     * |   名字   |
     * ------------
     */
	top_left_x += delta_x;
	top_left_y += delta_y;
	top_left_x_bak = top_left_x;

	for (i = 0; i < number_per_col; i++)
	{
		for (j = 0; j < number_per_row; j++)
		{
			/* 图标 */
			dir_and_file_layout[k].top_left_x	= top_left_x + (DIR_FILE_NAME_WIDTH - DIR_FILE_ICON_WIDTH) / 2;
			dir_and_file_layout[k].top_left_y	= top_left_y;
			dir_and_file_layout[k].bot_right_x	= dir_and_file_layout[k].top_left_x + DIR_FILE_ICON_WIDTH - 1;
			dir_and_file_layout[k].bot_right_y	= top_left_y + DIR_FILE_ICON_HEIGHT - 1;

			/* 名字 */
			dir_and_file_layout[k+1].top_left_x	= top_left_x;
			dir_and_file_layout[k+1].top_left_y = dir_and_file_layout[k].bot_right_y + 1;
			dir_and_file_layout[k+1].bot_right_x= top_left_x + DIR_FILE_NAME_WIDTH + 1;
			dir_and_file_layout[k+1].bot_right_y= dir_and_file_layout[k+1].top_left_y + DIR_FILE_NAME_HEIGHT - 1;

			top_left_x += DIR_FILE_ALL_WIDTH + delta_x;
			k+=2;
		}

		top_left_x = top_left_x_bak;
		top_left_y += DIR_FILE_ALL_HEIGHT + delta_y;
	}

	dir_and_file_layout[k].top_left_x	= 0;
	dir_and_file_layout[k].top_left_y	= 0;
	dir_and_file_layout[k].bot_right_x	= 0;
	dir_and_file_layout[k].bot_right_y	= 0;
	dir_and_file_layout[k].icon_name 	= NULL;

#if 0
	k = 0;

	for (i = 0; i < number_per_col; i++)
	{
		for (j = 0; j < number_per_row; j++)
		{
			DEBUG_PRINTF("k : %d\n", k);
			DEBUG_PRINTF("top_left_x : %d\n", dir_and_file_layout[k].top_left_x);
			DEBUG_PRINTF("top_left_y : %d\n", dir_and_file_layout[k].top_left_y);
			DEBUG_PRINTF("bot_right_x : %d\n", dir_and_file_layout[k].bot_right_x);
			DEBUG_PRINTF("bot_right_y : %d\n", dir_and_file_layout[k].bot_right_y);

			DEBUG_PRINTF("k+1 : %d\n", k+1);
			DEBUG_PRINTF("top_left_x : %d\n", dir_and_file_layout[k+1].top_left_x);
			DEBUG_PRINTF("top_left_y : %d\n", dir_and_file_layout[k+1].top_left_y);
			DEBUG_PRINTF("bot_right_x : %d\n", dir_and_file_layout[k+1].bot_right_x);
			DEBUG_PRINTF("bot_right_y : %d\n", dir_and_file_layout[k+1].bot_right_y);			

			k+=2;
		}
	}
#endif	

	return 0;
}

static int generate_browse_page_dir_and_file(int start, int dir_contents_numbers, dir_content_t **dir_contents_array, video_mem_t * video_mem)
{
	static pixel_datas_t dir_icon_pixel_datas;
	static pixel_datas_t file_icon_pixel_datas;
	static int get_dir_file_bmp = 0;

	pixel_datas_t origin_icon_pixel_datas;

	page_layout_t *page_layout	= &browse_page_dir_and_file_layout;
	layout_t *layout			= page_layout->layout;
	layout_t page_layout_data;


	int dir_content_index = start;

	int i;
	int j;
	int k = 0;

	int ret;

	if (!get_dir_file_bmp)
	{
		dir_icon_pixel_datas.bpp 		=	video_mem->pixel_datas.bpp;
		dir_icon_pixel_datas.pixel_data	=	malloc(page_layout->max_total_bytes);	
		if (dir_icon_pixel_datas.pixel_data == NULL)
		{
			return -1;
		}

		file_icon_pixel_datas.bpp		= video_mem->pixel_datas.bpp;
		file_icon_pixel_datas.pixel_data= malloc(page_layout->max_total_bytes);	
		if (file_icon_pixel_datas.pixel_data == NULL)
		{
			return -1;
		}

		/* 从BMP文件中提取图像数据 */
		/* 1.目录图标 */
		ret = get_pixel_datas_for_icon(dir_icon_name, &origin_icon_pixel_datas);
		if (ret)
		{
			DEBUG_PRINTF("get_pixel_datas_for_icon %s error\n", dir_icon_name);
			return -1;
		}
		dir_icon_pixel_datas.height		= layout[0].bot_right_y - layout[0].top_left_y + 1;
		dir_icon_pixel_datas.width		= layout[0].bot_right_x - layout[0].top_left_x + 1;
		dir_icon_pixel_datas.line_bytes	= dir_icon_pixel_datas.width * dir_icon_pixel_datas.bpp / 8;
		dir_icon_pixel_datas.total_bytes= dir_icon_pixel_datas.line_bytes * dir_icon_pixel_datas.height;

		picture_zoom(&origin_icon_pixel_datas, &dir_icon_pixel_datas);
		free_pixel_datas_from_icon(&origin_icon_pixel_datas);

		/* 2.文件图标 */
		ret = get_pixel_datas_for_icon(file_icon_name, &origin_icon_pixel_datas);
		if (ret)
		{
			DEBUG_PRINTF("get_pixel_datas_for_icon %s error\n", file_icon_name);
			return -1;
		}
		file_icon_pixel_datas.height		= layout[0].bot_right_y - layout[0].top_left_y + 1;
		file_icon_pixel_datas.width		= layout[0].bot_right_x - layout[0].top_left_x + 1;
		file_icon_pixel_datas.line_bytes	= file_icon_pixel_datas.width * file_icon_pixel_datas.bpp / 8;
		file_icon_pixel_datas.total_bytes= file_icon_pixel_datas.line_bytes * file_icon_pixel_datas.height;

		picture_zoom(&origin_icon_pixel_datas, &file_icon_pixel_datas);
		free_pixel_datas_from_icon(&origin_icon_pixel_datas);		

		get_dir_file_bmp = 1;
	}

	page_layout_data.top_left_x = page_layout->top_left_x;
	page_layout_data.top_left_y = page_layout->top_left_y;
	page_layout_data.bot_right_x= page_layout->bot_right_x;
	page_layout_data.bot_right_y= page_layout->bot_right_y;

	clear_rect_in_video_mem(&page_layout_data, video_mem, COLOR_BACKGROUND);

	set_font_size(layout[1].bot_right_y - layout[1].top_left_y - 5);

	for (i = 0; i < dir_file_num_per_col; i++)
	{
		for (j = 0; j < dir_file_num_per_row; j++)
		{
			if (dir_content_index < dir_contents_numbers)
			{
				if (dir_contents_array[dir_content_index]->file_type == FILETYPE_DIR)
				{
					picture_merge(layout[k].top_left_x, layout[k].top_left_y, &dir_icon_pixel_datas, &video_mem->pixel_datas);
				}
				else
				{
					picture_merge(layout[k].top_left_x, layout[k].top_left_y, &file_icon_pixel_datas, &video_mem->pixel_datas); 			
				}
				
				k++;

				ret = merge_string_to_center_of_rect_in_video_mem(&layout[k], (unsigned char *)dir_contents_array[dir_content_index]->name, video_mem);
				//DEBUG_PRINTF("dir_contents_array : %s\n", dir_contents_array[dir_content_index]->name);
				k++;
				
				dir_content_index++;
			}
			else
			{
				break;
			}
		}
		if (dir_content_index >= dir_contents_numbers)
		{
			break;
		}
	}

	return 0;
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
		calc_browse_page_dir_and_files_layout();
	}
	
	ret = generate_page(page_layout, video_mem);
	if (ret)
	{
		DEBUG_PRINTF("generate_page error\n");
		return;
	}

	ret = generate_browse_page_dir_and_file(start_index, dir_contents_number, dir_contents, video_mem);
	if (ret)
	{
		DEBUG_PRINTF("generate_browse_page_dir_and_file error\n");
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

static int get_input_position_in_page_layout(page_layout_t *page_layout, input_event_t *input_event)
{
	int i = 0;
	layout_t *layout = page_layout->layout;

	/* 数据处理 */
	/* 判断触点位于哪一个按钮上 */
	while (layout[i].bot_right_y)
	{
		if ((input_event->x >= layout[i].top_left_x) && (input_event->x <= layout[i].bot_right_x) &&
			(input_event->y >= layout[i].top_left_y) && (input_event->y <= layout[i].bot_right_y))
		{
			return i;
		}
		else
		{
			i++;
		}
	}

	/* 没有文件或目录被按下 */
	return -1;
}

/*
 * browse页面有两个区域: 菜单图标, 目录和文件图标
 * 为统一处理, "菜单图标"的序号为0,1,2,3,..., "目录和文件图标"的序号为1000,1001,1002,....
 *
 */
#define DIRFILE_ICON_INDEX_BASE 1000
static void browse_page_run()
{
	input_event_t input_event;
	input_event_t input_event_press;
	int index;

	/*
	 *	先按下选择按钮，再点击目录图标
	 */
	int have_click_select_icon = 0;

	int icon_pressed = 0;
	int pre_index = -1;

	int ret;

	video_mem_t *dev_video_mem;

	dev_video_mem = get_dev_video_mem();

	/* 0.	获得要显示的目录 */
	ret = get_dir_contents(cur_dir, &dir_contents, &dir_contents_number);
	if (ret)
	{
		DEBUG_PRINTF("get_dir_contents error\n");
		return ;
	}
	
	/* 1.	显示界面 */
	show_browse_page(&browse_page_menu_icons_layout);

	/* 2.	启动prepare线程 */

	/* 3.	处理input事件 */
	while (1)
	{
		/*	确定是否是菜单图标被按下 */
		index = browse_page_get_input_event(&browse_page_menu_icons_layout, &input_event);

		/* 不在菜单图标上，判断是否在“文件目录”上 */
		if (index == -1)
		{
			index = get_input_position_in_page_layout(&browse_page_dir_and_file_layout, &input_event);
			if (index != -1)
			{
				/* 判断该触点上是否有图标 */
				if (start_index + index / 2 < dir_contents_number)
					index += DIRFILE_ICON_INDEX_BASE;
				else
					index = -1;
			}
		}

		if (input_event.pressure == 0)
		{
			if (icon_pressed)
			{
				release_button(&menu_icons_layout[pre_index]);
				icon_pressed = 0;

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
							start_index -= dir_file_num_per_row * dir_file_num_per_col;
							if (start_index >= 0)
							{
								ret = generate_browse_page_dir_and_file(start_index, dir_contents_number, dir_contents, dev_video_mem);
							}
							else
							{
								start_index += dir_file_num_per_row * dir_file_num_per_col;
							}
							
							
							break;
						}

						case 3:		/* next_page.bmp */
						{
							start_index += dir_file_num_per_row * dir_file_num_per_col;
							if (start_index < dir_contents_number)
							{
								ret = generate_browse_page_dir_and_file(start_index, dir_contents_number, dir_contents, dev_video_mem);
							}
							else
							{
								start_index -= dir_file_num_per_row * dir_file_num_per_col;
							}

						
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
				if (!icon_pressed)		//	未按下
				{
					icon_pressed = 1;
					input_event_press = input_event;
					pre_index = index;

					if (index < DIRFILE_ICON_INDEX_BASE)	/* 如果是菜单图标 */
					{
						/* 如果之前未按下选择键 */
						if (!(have_click_select_icon && (pre_index == 1)))
							press_button(&menu_icons_layout[pre_index]);
					}
					else	/* 目录和文件图标 */
					{
						
					}

				}
			}

		}
	}


	return ;
}

static int browse_page_prepare()
{
	return 0;
}


int browse_page_init(void)
{
	return register_page_action(&browse_page_action);
}


