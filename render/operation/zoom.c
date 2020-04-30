/**********************************************************************

    > File Name: zoom.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月25日 星期六 00时56分47秒

***********************************************************************/

#include <stdlib.h>

#include "config.h"
#include "picture/picture_parser.h"
#include "render.h"

int picture_zoom(pixel_datas_t *origin_picture, pixel_datas_t *zoom_picture)
{
	unsigned long y;
	unsigned long x;

	unsigned int pixel_bytes = origin_picture->bpp/8;

/* 最近邻内插 */
#if defined(NEAREST_INTERPOLATION)

	unsigned char *dst;
	unsigned char *src;

	unsigned long src_y;
	unsigned long *src_table = malloc(sizeof(unsigned long) * zoom_picture->width);

	if (origin_picture->bpp != zoom_picture->bpp)
	{
		return -1;
	}

	/* Dx = Sx * (Dw / Sw) */
	for (x = 0; x < zoom_picture->width; x++)
	{
		src_table[x] = (x * origin_picture->width / zoom_picture->width);	
	}

	for (y = 0; y < zoom_picture->height; y++)
	{
		src_y = (y * origin_picture->height / zoom_picture->height);

		dst = zoom_picture->pixel_data + y * zoom_picture->line_bytes;
		src = origin_picture->pixel_data + src_y * origin_picture->line_bytes;

		for (x = 0; x < zoom_picture->width; x++)
		{
			/*
			 *	源图像坐标:	src_table[x],		srcy
			 *	缩放坐标:		x, 				y
			 */
			memcpy(dst + x*pixel_bytes, src + src_table[x]*pixel_bytes, pixel_bytes);
		}
	}

	free(src_table);

#endif

/* 双线性内插 */
#if defined(BILINEAR_INTERPOLATION)

#endif

	return 0;
}




