/**********************************************************************

    > File Name: render.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月23日 星期四 23时06分11秒

***********************************************************************/

#include <stdio.h>

#include "config.h"
#include "display_manager.h"
#include "page_manager.h"
#include "file.h"
#include "encoding_manager.h"

extern picture_parser_t bmp_parser;


void flush_video_mem_to_dev(video_mem_t *video_mem)
{
	if (!video_mem->is_framebuffer)
	{
		get_default_display_device()->show_page(video_mem);
	}
}

int get_pixel_datas_for_icon(char *file_name, pixel_datas_t *pixel_datas)
{
	file_map_t file_map;
	int ret;
	int x_res;
	int y_res;
	int bpp;

	/* /etc/digitpic/icons */
	snprintf(file_map.file_name, 128, "%s/%s", ICON_PATH, file_name);
	file_map.file_name[127] = '\0';

	ret = map_file(&file_map);
	if (ret)
	{
		DEBUG_PRINTF("map file %s failed \n", file_name);
		return -1;
	}
	
	ret = bmp_parser.is_supported(file_map.file_map_mem);
	if (ret == 0)
	{
		DEBUG_PRINTF("%s is not bmp file\n", file_name);
		return -1;	
	}

	get_display_resolution(&x_res, &y_res, &bpp);
	pixel_datas->bpp = bpp;
	ret = bmp_parser.get_pixel_datas(file_map.file_map_mem, pixel_datas);
	if (ret)
	{
		DEBUG_PRINTF("get_pixel_datas for %s error\n", file_name);
		return -1;
	}
	
	return 0;
}

void free_pixel_datas_from_icon(pixel_datas_t * pixel_datas)
{
	bmp_parser.free_pixel_datas(pixel_datas);
}

/* 返回设置的字节数 */
static int set_color_for_pixel_in_video_mem(int x, int y, video_mem_t *video_mem, unsigned int color)
{
	/* 只考虑32bpp */
	unsigned int *video_mem_32_bpp_index;
	unsigned char *mem_start;

	mem_start = video_mem->pixel_datas.pixel_data;
	mem_start += y * video_mem->pixel_datas.line_bytes + x * video_mem->pixel_datas.bpp / 8;

	video_mem_32_bpp_index = (unsigned int *)mem_start;

	*video_mem_32_bpp_index = color;
	return 4;
}

static int merge_one_font_to_video_mem(font_bitmap_t *font_bitmap, video_mem_t *video_mem)
{
	int x;
	int y;
	int i = 0;

	int num;

	/* 只考虑8的情况 */
	if (font_bitmap->bpp == 8)
	{
		for (y = font_bitmap->pos_top; y < font_bitmap->pos_bottom; y++)
		{
			for (x = font_bitmap->pos_left; x < font_bitmap->pos_right; x++)
			{
				if (font_bitmap->buffer[i++])
				{
					num = set_color_for_pixel_in_video_mem(x, y, video_mem, COLOR_FOREGROUND);
				}
				else
				{
					num = set_color_for_pixel_in_video_mem(x, y, video_mem, COLOR_BACKGROUND);
				}

				if (num == -1)
				{
					return -1;
				}
			}
		}
	}
	else
	{
		DEBUG_PRINTF("show_one_font error, can't support %d bpp\n", font_bitmap->bpp);
		return -1;
	}

	return 0;
}

void clear_rect_in_video_mem(layout_t *layout, video_mem_t * video_mem, unsigned int color)
{
	int x;
	int y;

	for (y = layout->top_left_y; y <= layout->bot_right_y; y++)
	{
		for (x = layout->top_left_x; x <= layout->bot_right_x; x++)
		{
			set_color_for_pixel_in_video_mem(x, y, video_mem, color);
		}
	}
}

int merge_string_to_center_of_rect_in_video_mem(layout_t *layout, unsigned char *text_string, video_mem_t *video_mem)
{
	int ret;
	int len;

	unsigned char *buf_start;
	unsigned char *buf_end;
	int has_get_code = 0;
	unsigned int code;
	font_bitmap_t font_bitmap;

	int min_x = 32000, max_x = -1;
	int min_y = 32000, max_y = -1;

	int width;
	int height;

	int first_char_top_left_x = 32000;
	int first_char_top_left_y = 32000;
	int new_char_top_left_x;
	int new_char_top_left_y;

	buf_start = text_string;
	buf_end = buf_start + strlen((char *)text_string);
	font_bitmap.current_origin_x = 0;
	font_bitmap.current_origin_y = 0;

	/* 1.	清除该区域内容 */
	clear_rect_in_video_mem(layout, video_mem, COLOR_BACKGROUND);

	/* 2.	计算字符串显示的总体宽度高度 */
	while (1)
	{
		len = get_code_from_buf(buf_start, buf_end, &code);
		if (0 == len)
		{
			/* 字符串结束 */
			if (!has_get_code)
			{
				return -1;
			}
			else
				break;
		}

		buf_start += len;
		has_get_code = 1;

		ret = get_font_bitmap(code, &font_bitmap);
		if (ret == 0)
		{
			if (first_char_top_left_x == 32000)
			{
				first_char_top_left_x = font_bitmap.pos_left;
				first_char_top_left_y = font_bitmap.pos_top;
			}

			/* 字符显示区域最值 */
			if (min_x > font_bitmap.pos_left)
			{
				min_x = font_bitmap.pos_left;
			}
			if (max_x < font_bitmap.pos_right)
			{
				max_x = font_bitmap.pos_right;
			}
			if (min_y > font_bitmap.pos_top)
			{
				min_y = font_bitmap.pos_top;
			}
			if (max_y < font_bitmap.pos_bottom)
			{
				max_y = font_bitmap.pos_bottom;
			}
			
			font_bitmap.current_origin_x = font_bitmap.next_origin_x;
			font_bitmap.current_origin_y = font_bitmap.next_origin_y;
		}
		else
		{
			DEBUG_PRINTF("get_font_bitmap for calc width/height error!\n");	
		}
	}

	width	= max_x - min_x;
	height	= max_y - min_y;

	/* 字符串过长 */
	if (width > (layout->bot_right_x - layout->top_left_x))
	{
		width = layout->bot_right_x - layout->top_left_x;
	}

	/* 字符串过高 */
	if (height > (layout->bot_right_y - layout->top_left_y))
	{
		DEBUG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);	
		return -1;
	}

	/* 3.	确定第一个字符的原点，利用第一个字符左上角点的相对位置算出字符原点的相对位置 */
	/* 3.1	计算左上角坐标 */
	new_char_top_left_x = layout->top_left_x + (layout->bot_right_x - layout->top_left_x - width) / 2;
	new_char_top_left_y = layout->top_left_y + (layout->bot_right_y - layout->top_left_y - height) / 2;

	/* 3.2	计算第一个字符原点坐标 */
	font_bitmap.current_origin_x = new_char_top_left_x - first_char_top_left_x;
	font_bitmap.current_origin_y = new_char_top_left_y - first_char_top_left_y;

	buf_start = text_string;
	has_get_code = 0;
	while (1)
	{
		len = get_code_from_buf(buf_start, buf_end, &code);
		if (0 == len)
		{
			/* 字符串结束 */
			if (!has_get_code)
			{
				return -1;
			}
			else
				break;

		}

		buf_start += len;
		has_get_code = 1;
		
		ret = get_font_bitmap(code, &font_bitmap);
		if (ret == 0)
		{
			if (merge_one_font_to_video_mem(&font_bitmap, video_mem))
			{
				DEBUG_PRINTF("merge_one_font_to_video_mem error for code 0x%x\n", code);
				return -1;
			}

			font_bitmap.current_origin_x = font_bitmap.next_origin_x;
			font_bitmap.current_origin_y = font_bitmap.next_origin_y;
		}
		else
		{
			DEBUG_PRINTF("get_font_bitmap for calc width/height error!\n");	
		}

	}

	return 0;
}


static void invert_button(layout_t *layout)
{
	int y;
	int i;
	unsigned char *icon_video_mem;
	display_operation_t *disp_opr = get_default_display_device();
	int button_width_bytes;
	int line_bytes;

	line_bytes = disp_opr->Xres * disp_opr->bpp / 8;

	icon_video_mem = disp_opr->display_mem;
	icon_video_mem += layout->top_left_y * line_bytes + layout->top_left_x * disp_opr->bpp / 8;
	button_width_bytes = (layout->bot_right_x - layout->top_left_x) * disp_opr->bpp / 8;

	for (y = layout->top_left_y; y <= layout->bot_right_y; y++)
	{
		for (i = 0; i < button_width_bytes; i++)
		{
			icon_video_mem[i] = ~icon_video_mem[i];
		}
		icon_video_mem += line_bytes;
	}
}

void press_button(layout_t *layout)
{
	invert_button(layout);
}

void release_button(layout_t *layout)
{
	invert_button(layout);
}

