/**********************************************************************

    > File Name: merge.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月25日 星期六 00时56分43秒

***********************************************************************/

#include "config.h"
#include "picture/picture_parser.h"
#include "render.h"

int picture_merge(int x, int y, pixel_datas_t *src_picture, pixel_datas_t *dst_picture)
{
	int i;
	unsigned char *src;
	unsigned char *dst;

	if ((src_picture->width >= dst_picture->width) 		||
		(src_picture->height >= dst_picture->height) 	||
		(src_picture->bpp != dst_picture->bpp))
	{
		return -1;
	}

	src = src_picture->pixel_data;
	dst = dst_picture->pixel_data + y * dst_picture->line_bytes + x * dst_picture->bpp / 8;

	for (i = 0; i < src_picture->height; i++)
	{
		memcpy(dst, src, src_picture->line_bytes);
		src += src_picture->line_bytes;
		dst += dst_picture->line_bytes;
	}

	return 0;
}
