/**********************************************************************

    > File Name: fonts_manager.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月12日 星期日 17时15分53秒

***********************************************************************/

#include "config.h"
#include "fonts_manager.h"

#include <string.h>

static font_operation_t *fonts_operation_head; 
static int font_size;

int register_font_operation(font_operation_t *font_opr)
{
	font_operation_t *temp;

	//	如果链表上无设备
	if (!fonts_operation_head)
	{
		fonts_operation_head = font_opr;
		fonts_operation_head->next = NULL;
	}
	else	//	链表上有设备
	{
		temp = fonts_operation_head;

		while (temp->next)
			temp = temp->next;

		temp->next = font_opr;
		font_opr->next = NULL;
	}

	return 0;

}

void show_all_fonts_operation(void)
{
	font_operation_t	*temp = fonts_operation_head;
	int	i = 0;

	puts("fonts : ");
	while (temp)
	{
		printf("%02d %s\n", i++, temp->name);
		temp = temp->next;
	}

}

font_operation_t *get_font_operation(char *name)
{
	font_operation_t *temp = fonts_operation_head;

	while (temp)
	{
		if (STRCMP(temp->name, ==, name))
		{
			return temp;
		}
		temp = temp->next;
	}

	return NULL;

}

void set_font_size(unsigned int size)
{
	font_operation_t *temp = fonts_operation_head;

	font_size = size;

	while (temp)
	{
		if (temp->set_font_size)
		{
			temp->set_font_size(size);
		}
		temp = temp->next;
	}
}

unsigned int get_font_size(void)
{
	return font_size;
}

int get_font_bitmap(unsigned int code, font_bitmap_t *font_bitmap)
{
	int ret;
	font_operation_t *temp = fonts_operation_head;

	while (temp)
	{
		ret = temp->get_font_bitmap(code, font_bitmap);
		if (ret == 0)
		{
			return 0;
		}
		temp = temp->next;
	}

	return -1;
}

int set_fonts_detail(char *fonts_name, char *fonts_file, unsigned int size)
{
	int ret = 0;
	font_operation_t *font_opr;

	font_opr = get_font_operation(fonts_name);
	if (font_opr == NULL)
	{
		return -1;
	}

	font_size = size;

	ret = font_opr->font_init(fonts_file, size);

	return ret;
}	



int fonts_manager_init(void)
{
	return freetype_init();
}



