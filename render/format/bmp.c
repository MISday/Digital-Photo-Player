/**********************************************************************

    > File Name: bmp.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月23日 星期四 23时06分33秒

***********************************************************************/

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "picture/picture_parser.h"
#include "picture/bmp.h"

static int is_bmp_format(unsigned char *file_head);
static int get_pixel_datas_from_bmp(unsigned char *file_head, pixel_datas_t *pixel_datas);
static int free_pixel_datas_from_bmp(pixel_datas_t *pixel_datas);

picture_parser_t bmp_parser = 
{
	.name = "bmp",

	.is_supported 		= is_bmp_format,
	.get_pixel_datas 	= get_pixel_datas_from_bmp,
	.free_pixel_datas 	= free_pixel_datas_from_bmp,
};

static int is_bmp_format(unsigned char *file_head)
{
	if (file_head[0] != 'B' || file_head[1] != 'M')
		return 0;
	else
		return 1;
}

static int convert_one_line(int width, int src_bpp, int dst_bpp, 
								unsigned char *src_data, unsigned char *dst_data)
{
	unsigned int red;
	unsigned int green;
	unsigned int blue;
	unsigned int color;

	unsigned short	*dst_data_16_bpp_index = (unsigned short *)dst_data;
	unsigned int	*dst_data_32_bpp_index = (unsigned int *)dst_data;

	int i;
	int pos = 0;

	if (src_bpp != 24)
	{
		return -1;
	}

	if (dst_bpp == 24)
	{
		memcpy(dst_data, src_data, width*3);
	}
	else
	{
		for (i = 0; i < width; i++)
		{
			/* bitmap中排列为rbg */
			red 	= src_data[pos++];
			blue 	= src_data[pos++];
			green	= src_data[pos++];
			if (dst_bpp == 32)
			{
				color = (red << 16) | (green << 8) | blue;
				*dst_data_32_bpp_index = color;
				dst_data_32_bpp_index++;
			}
			else if (dst_bpp == 16)
			{
				/* rgb 565 */
				red = red >> 3;
				green = green >> 2;
				blue = blue >> 3;
				color = (red << 11) | (green << 5) | blue;
				*dst_data_16_bpp_index = color;
				dst_data_16_bpp_index++;
			}
		}
	}

	return 0;
}

static int get_pixel_datas_from_bmp(unsigned char *file_head, 
											pixel_datas_t *pixel_datas)
{
	bitmap_header_t			*bitmap_header;
	bitmap_info_header_t	*bitmap_info_header;

	int width;
	int height;
	int bpp;

	int data_line_width;
	int data_line_align;
		
	unsigned char *src_data;
	unsigned char *dst_data;

	int row;
	
	bitmap_header 		= (bitmap_header_t *)file_head;
	bitmap_info_header 	= (bitmap_info_header_t *)(file_head + sizeof(bitmap_header_t));

	bpp = bitmap_info_header->bpp;
	width = bitmap_info_header->width;
	height = bitmap_info_header->height;

	/* 只处理rgb888 */
	if (bpp != 24)	
	{
		DEBUG_PRINTF("bmp bpp : %d\n", bpp);
		DEBUG_PRINTF("sizeof(bitmap_header_t) : %d\n", sizeof(bitmap_header_t));
		return -1;
	}

	pixel_datas->width = width;
	pixel_datas->height = height;
	pixel_datas->pixel_data = malloc(width * height * pixel_datas->bpp / 8);
	pixel_datas->line_bytes = width * pixel_datas->bpp / 8;

	if (pixel_datas->pixel_data == NULL)
	{
		return -1;
	}

	data_line_width = width * bpp / 8;	//	实际数据行宽
	data_line_align = (data_line_width + 3) & ~0x3;	//	4字节对齐

	src_data = file_head + bitmap_header->offset_data;
	src_data = src_data + (height - 1) * data_line_align;	//	bmp图形的显示方式

	dst_data = pixel_datas->pixel_data;


	for (row = 0; row < height; row++)
	{
		convert_one_line(width, bpp, pixel_datas->bpp, src_data, dst_data);
		src_data -= data_line_align;
		dst_data += pixel_datas->line_bytes;
	}

	return 0;
}

static int free_pixel_datas_from_bmp(pixel_datas_t *pixel_datas)
{
	free(pixel_datas->pixel_data);
	return 0;
}



