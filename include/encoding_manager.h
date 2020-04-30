/**********************************************************************

    > File Name: encoding_manager.h

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月12日 星期日 17时14分46秒

***********************************************************************/

#ifndef __ENCODING_MANAGER_H__
#define __ENCODING_MANAGER_H__

#include "fonts_manager.h"

typedef struct _encoding_operation
{
	char 				*name;

	int					head_length;			//	文件头的长度
	font_operation_t	*font_supported_head;	//	受支持的字体

	int (*is_supported)(unsigned char *file_head_buf);
	int (*get_code_from_buf)(unsigned char *file_mem_start, 
						unsigned char *file_mem_end, 
						unsigned int *code);

	struct _encoding_operation *next;
} encoding_operation_t;

int register_encoding_operation(encoding_operation_t *encoding_opr);

void show_all_encoding_operation(void);

encoding_operation_t *get_encoding_operation(encoding_operation_t *encoding_opr);

int encoding_manager_init(void);

int utf8_encoding_init(void);

/* 从文件头中确认要使用的编码方式 */
encoding_operation_t *select_encoding_operation_for_file(unsigned char *file_head_buf);
int add_font_operation_for_Encoding(encoding_operation_t *encoding_opr, font_operation_t *font_opr);
int del_font_operation_for_Encoding(encoding_operation_t *encoding_opr, font_operation_t *font_opr);

int get_code_from_buf(unsigned char *buf_start, unsigned char *buf_end, unsigned int *code);


#endif /* __ENCODING_MANAGER_H__ */




