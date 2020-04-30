/**********************************************************************

    > File Name: fonts_manager.h

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月12日 星期日 17时14分29秒

***********************************************************************/

#ifndef __FONTS_MANAGER_H__
#define __FONTS_MANAGER_H__

typedef struct _font_bitmap
{
	int		pos_left;
	int		pos_top;
	int		pos_right;
	int		pos_bottom;

	int		bpp;
	int		current_origin_x;
	int		current_origin_y;

	int 	next_origin_x;
	int 	next_origin_y;

	unsigned char *buffer;
} font_bitmap_t;

typedef struct _font_operation
{
	char 		*name;

	int (*font_init)(char *font_file, unsigned int font_size);
	int (*get_font_bitmap)(unsigned int code, font_bitmap_t *font_bitmap);
	void (*set_font_size)(unsigned int size);

	struct _font_operation *next;
} font_operation_t;

int register_font_operation(font_operation_t *font_opr);

void show_all_fonts_operation(void);

font_operation_t *get_font_operation(char *name);

int fonts_manager_init(void);

int freetype_init(void);

void set_font_size(unsigned int size);
unsigned int get_font_size(void);

int get_font_bitmap(unsigned int code, font_bitmap_t *font_bitmap);

int set_fonts_detail(char *fonts_name, char *fonts_file, unsigned int size);

#endif /* __FONTS_MANAGER_H__ */





















