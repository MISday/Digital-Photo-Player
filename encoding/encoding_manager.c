/**********************************************************************

    > File Name: encoding_manager.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月12日 星期日 17时15分30秒

***********************************************************************/


#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "encoding_manager.h"

/* 管理所有显示设备的链表头 */
static encoding_operation_t *encoding_operation_head;

int register_encoding_operation(encoding_operation_t *encoding_opr)

{
	encoding_operation_t *temp;

	//	如果链表上无设备
	if (!encoding_operation_head)
	{
		encoding_operation_head = encoding_opr;
		encoding_operation_head->next = NULL;
	}
	else	//	链表上有设备
	{
		temp = encoding_operation_head;

		while (temp->next)
			temp = temp->next;

		temp->next = encoding_opr;
		encoding_opr->next = NULL;
	}

	return 0;
}

void show_all_encoding_operation(void)

{
	encoding_operation_t	*temp = encoding_operation_head;
	int	i = 0;

	puts("encoding : ");
	while (temp)
	{
		printf("%02d %s\n", i++, temp->name);
		temp = temp->next;
	}
	
}

encoding_operation_t *get_encoding_operation(encoding_operation_t *encoding_opr)

{
	encoding_operation_t *temp = encoding_operation_head;

	while (temp)
	{
		if (STRCMP(temp->name, ==, encoding_opr->name))
		{
			return temp;
		}
		temp = temp->next;
	}

	return NULL;
}


encoding_operation_t *select_encoding_operation_for_file(unsigned char *file_head_buf)
{
	encoding_operation_t *temp = encoding_operation_head;

	while (temp)
	{
		if (temp->is_supported(file_head_buf))
		{
			return temp;
		}
		else
		{
			temp = temp->next;
		}
	}

	return NULL;
}

int add_font_operation_for_Encoding(encoding_operation_t *encoding_opr, font_operation_t *font_opr)
{
	font_operation_t *font_temp;

	if (!encoding_opr || !font_opr)
	{
		return -1;
	}
	else
	{
		font_temp = (font_operation_t *)malloc(sizeof(font_operation_t));
		if (!font_temp)
		{
			return -1;
		}
		else
		{
			memcpy(font_temp, font_opr, sizeof(font_operation_t));
			font_temp->next = encoding_opr->font_supported_head;
			encoding_opr->font_supported_head = font_temp;
			return 0;			
		}
	}
}

int del_font_operation_for_Encoding(encoding_operation_t *encoding_opr, font_operation_t *font_opr)
{
	font_operation_t *font_temp;
	font_operation_t *font_pre_temp;

	if (!encoding_opr || !font_opr)
	{	
		return -1; 
	}

	font_temp = encoding_opr->font_supported_head;
	if (STRCMP(font_temp->name, ==, font_opr->name))
	{
		encoding_opr->font_supported_head = font_temp->next;
		free(font_temp);
		return 0;
	}

	font_pre_temp = encoding_opr->font_supported_head;
	font_temp = font_pre_temp->next;

	while (font_temp)
	{
		if (STRCMP(font_temp->name, ==, font_opr->name))
		{
			font_pre_temp->next = font_temp->next;
			free(font_temp);
			return 0;
		}
		else
		{
			font_pre_temp = font_temp;
			font_temp = font_temp->next;
		}

	}

	return -1;

}

int get_code_from_buf(unsigned char *buf_start, unsigned char *buf_end, unsigned int *code)
{
	unsigned char *buf_index = buf_start;
	unsigned char c = *buf_index;
	if ((buf_start < buf_end) && (c < (unsigned char)0x80))
	{
		*code = (unsigned int)c;
		return 1;
	}

	if (buf_start < buf_end)
	{
		*code = (unsigned int)c;
		return 1;
	}
	else	/* 文件处理完毕 */
	{
		return 0;
	}
}


int encoding_manager_init(void)

{
	int		err;

	err = utf8_encoding_init();

	return err;
}






