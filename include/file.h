/**********************************************************************

    > File Name: file.h

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月27日 星期一 11时12分32秒

***********************************************************************/

#ifndef __FILE_H__
#define __FILE_H__

typedef struct _file_map
{
	char file_name[128];
	int fd;
	int file_size;
	unsigned char *file_map_mem;
} file_map_t;

int map_file(file_map_t *file_map);
void unmap_file(file_map_t *file_map);

#endif /* __FILE_H__ */

