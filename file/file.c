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



