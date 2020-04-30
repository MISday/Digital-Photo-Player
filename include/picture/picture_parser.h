/**********************************************************************

    > File Name: picture_parser.h

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月24日 星期五 15时33分37秒

***********************************************************************/

#ifndef __PICTURE_OPERATION_H__
#define __PICTURE_OPERATION_H__

typedef struct _pixel_datas
{
	int width;
	int height;
	int bpp;
	int line_bytes;
	int total_bytes;
	unsigned char *pixel_data;
} pixel_datas_t;

typedef struct _picture_parser
{
	char *name;

	int (*is_supported)(unsigned char *file_head);
	int (*get_pixel_datas)(unsigned char *file_head, pixel_datas_t *pixel_datas);
	int (*free_pixel_datas)(pixel_datas_t *pixel_datas);
} picture_parser_t;

#endif /* __PICTURE_OPERATION_H__ */


