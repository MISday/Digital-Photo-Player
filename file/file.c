/**********************************************************************

    > File Name: file.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月27日 星期一 11时12分13秒

***********************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

#include "config.h"
#include "file.h"

int map_file(file_map_t *file_map)
{
	int fd;
	struct stat file_stat;

	fd = open(file_map->file_name, O_RDWR);
	if (fd < 0)
	{
		DEBUG_PRINTF("can't open %s\n", file_map->file_name);
		return -1;
	}
	file_map->fd = fd;

	fstat(fd, &file_stat);
	file_map->file_size = file_stat.st_size;
	
	file_map->file_map_mem = (unsigned char *)mmap(NULL, file_stat.st_size,
				PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (file_map->file_map_mem == MAP_FAILED)
	{
		DEBUG_PRINTF("mmap error!\n");
		return -1;		
	}

	return 0;
}

void unmap_file(file_map_t *file_map)
{
	munmap(file_map->file_map_mem, file_map->file_size);
	close(file_map->fd);
}

static int is_dir(char *file_path, char *file_name)
{
	char temp[256];
	struct stat dir_stat;

	snprintf(temp, 256, "%s/%s", file_path, file_name);
	temp[255] = '\0';

	if ((stat(temp, &dir_stat) == 0) && S_ISDIR(dir_stat.st_mode))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static int is_reg_file(char *file_path, char *file_name)
{
	char temp[256];
	struct stat reg_stat;

	snprintf(temp, 256, "%s/%s", file_path, file_name);
	temp[255] = '\0';

	if ((stat(temp, &reg_stat) == 0) && S_ISREG(reg_stat.st_mode))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


int get_dir_contents(char *dir_name, dir_content_t ***dir_contents_array, int *dir_content_numbers)
{
	struct dirent **name_list;
	dir_content_t **dir_contents;

	int number;

	int i;
	int j;

	int hiden_dir_or_file = 0;

	/* 
	 * 扫描目录 
	 * name_list是一个三重指针
	 */
	number = scandir(dir_name, &name_list, 0, alphasort);
	if (number < 0)
	{
		DEBUG_PRINTF("scandir error\n");
		return -1;
	}

	/* 统计隐藏目录.xxx */
	for (i = 0; i < number; i++)
	{
		if ((name_list[i]->d_name[0] == '.'))
			hiden_dir_or_file++;
	}

	/* 忽略隐藏目录 */
	dir_contents = malloc(sizeof(dir_content_t *) * (number - hiden_dir_or_file));
	if (NULL == dir_contents)
	{
		DEBUG_PRINTF("malloc error\n");
		return -1;
	}
	*dir_contents_array = dir_contents;


	for (i = 0; i < number-hiden_dir_or_file; i++)
	{
		dir_contents[i] = malloc(sizeof(dir_content_t));
		if (NULL == dir_contents[i])
		{
			DEBUG_PRINTF("malloc error\n");
			return -1;
		}
	}
	
	/* 将目录先放入 */
	for (i = 0, j = 0; i < number; i++)
	{
		if (name_list[i]->d_name[0] == '.')
			continue;

		/* 不一定能使用d_type判断 */
		if (is_dir(dir_name, name_list[i]->d_name))
		{	
			strncpy(dir_contents[j]->name, name_list[i]->d_name, 256);
			dir_contents[j]->name[255] = '\0';
			dir_contents[j]->file_type = FILETYPE_DIR;
			free(name_list[i]);
			name_list[i] = NULL;
			j++;
		}
	}


	/* 再放入file */
	for (i = 0; i < number; i++)
	{
		if (name_list[i] == NULL)
			continue;
	
		if (name_list[i]->d_name[0] == '.')
			continue;
			
		/* 不一定能使用d_type判断 */
		if (is_reg_file(dir_name, name_list[i]->d_name))
		{		
			strncpy(dir_contents[j]->name, name_list[i]->d_name, 256);
			dir_contents[j]->name[255] = '\0';
			dir_contents[j]->file_type = FILETYPE_FILE;
			free(name_list[i]);
			name_list[i] = NULL;
			j++;
		}
	}

	/* 释放未使用的项 */
	for (i = j; i < number - hiden_dir_or_file ; i++)
	{
		free(dir_contents[i]);
	}

	/* 释放scandir的内存 */
	for (i = 0; i < number; i++)
	{
		if (name_list[i])
		{
			free(name_list[i]);
		}
	}

	free(name_list);

	*dir_content_numbers = j;
	
	return 0;
}


void free_dir_contents(dir_content_t **dir_contents, int number)
{
	int i;
	for (i = 0; i < number; i++)
	{
		free(dir_contents[i]);
	}
	free(dir_contents);
}
