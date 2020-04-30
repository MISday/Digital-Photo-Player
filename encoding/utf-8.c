/**********************************************************************

    > File Name: utf-8.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月12日 星期日 17时15分41秒

***********************************************************************/

#include <string.h>

#include "config.h"
#include "encoding_manager.h"

static int is_utf8_coding(unsigned char *file_head_buf);
static int get_utf8_code_from_buf(unsigned char *file_mem_start, 
						unsigned char *file_mem_end, 
						unsigned int *code);


static encoding_operation_t utf8_encoding_operation = 
{
	.name				=	"utf-8",

	.is_supported		=	is_utf8_coding,
	.get_code_from_buf	=	get_utf8_code_from_buf,
};

static int is_utf8_coding(unsigned char *file_head_buf)
{
	const char utf8_file_head[] = {0xEF, 0xBB, 0xBF, 0};

	if (STRNCMP((const char *)file_head_buf, ==, utf8_file_head, 3))
	{
		/* utf-8 */
		return 0;
	}
	else
	{
		return -1;
	}
}

static int get_pre_one_number(const unsigned char first_byte_data)
{
	int		number = 0;
	int		i;

	for (i = 7; i >= 0; i--)
	{
		if (!(first_byte_data & (1<<i)))
			break;
		else
			number++;
	}

	return number;
}

static int get_utf8_code_from_buf(unsigned char *file_mem_start, 
											unsigned char *file_mem_end, 
											unsigned int *code)
{
	unsigned char 	data;
	int 			number;
	unsigned int	sum = 0;
	int				i;

	if (file_mem_start >= file_mem_end)
	{
		/* EOF */
		return 0;
	}

	data = file_mem_start[0];
	number = get_pre_one_number(data);

	if ((file_mem_start + number) > file_mem_end)
	{
		/* EOF */
		return 0;
	}

	if (number == 0)
	{
		/* ASCII */
		*code = file_mem_start[0];
		return 1;
	}
	else
	{
		//	第一字节
		data = data << number;
		data = data >> number;	
		sum += data;

		//	剩余字节
		for (i = 1; i < number; i++)
		{
			data = file_mem_start[i] & 0x3f;	//	不取7、8位
			sum = sum << 6;
			sum += data;
		}
		*code = sum;
		return number;
	}
	
}

int utf8_encoding_init(void)
{
	add_font_operation_for_Encoding(&utf8_encoding_operation, get_font_operation("freetype"));
	return register_encoding_operation(&utf8_encoding_operation);
}

