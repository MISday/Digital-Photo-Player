/**********************************************************************

    > File Name: display_manager.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月12日 星期日 17时16分24秒

***********************************************************************/

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "display_manager.h"

/* 管理所有显示设备的链表头 */
static display_operation_t	*display_operation_head;
static display_operation_t	*default_display_operation;

static video_mem_t			*video_mem_head;

int register_display_operation(display_operation_t *disp_opr)
{
	display_operation_t *temp;

	//	如果链表上无设备
	if (!display_operation_head)
	{
		display_operation_head = disp_opr;
		display_operation_head->next = NULL;
	}
	else	//	链表上有设备
	{
		temp = display_operation_head;

		while (temp->next)
			temp = temp->next;

		temp->next = disp_opr;
		disp_opr->next = NULL;
	}

	return 0;
}

void show_all_display_operation(void)
{
	display_operation_t	*temp = display_operation_head;
	int	i = 0;

	puts("display device : ");
	while (temp)
	{
		printf("%02d %s\n", i++, temp->name);	
		temp = temp->next;
	}
}

display_operation_t *get_display_operation(char *name)
{
	display_operation_t *temp = display_operation_head;

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

display_operation_t *get_default_display_device(void)
{
	return default_display_operation;
}


void select_default_display_device(char *name)
{
	default_display_operation = get_display_operation(name);
	
	if (default_display_operation)
	{
		default_display_operation->device_init();
		default_display_operation->clean_screen(0);
	}
}

int get_display_resolution(int *xres, int *yres, int *bpp)
{
	if (default_display_operation)
	{
		*xres	= default_display_operation->Xres;
		*yres	= default_display_operation->Yres;
		*bpp	= default_display_operation->bpp;

		return 0;
	}
	else
	{
		DEBUG_PRINTF("the display device is not open\n");
		return -1;
	}
}

/* 链表中最开始被初始化的是framebuffer */
int alloc_video_mem(int num)
{
	int i;

	int x_res;
	int y_res;
	int bpp;

	int vm_size;
	int line_bytes;

	video_mem_t *video_mem_new = NULL;

	get_display_resolution(&x_res, &y_res, &bpp);

	vm_size = x_res * y_res * bpp / 8;
	line_bytes = x_res * bpp / 8;

	/* 载入framebuffer */
	video_mem_new = malloc(sizeof(video_mem_t));
	if (video_mem_new == NULL)
	{
		return -1;
	}
	video_mem_new->pixel_datas.pixel_data = default_display_operation->display_mem;

	video_mem_new->id = 0;
	video_mem_new->is_framebuffer = 1;
	video_mem_new->video_mem_state = VMS_FREE;
	video_mem_new->picture_state = PS_BLANK;
	video_mem_new->pixel_datas.width = x_res;
	video_mem_new->pixel_datas.height = y_res;
	video_mem_new->pixel_datas.bpp = bpp;
	video_mem_new->pixel_datas.line_bytes = line_bytes;
	video_mem_new->pixel_datas.total_bytes = vm_size;

	video_mem_new->next = video_mem_head;
	video_mem_head = video_mem_new;

	if (num != 0)
	{
		video_mem_new->video_mem_state = VMS_USED_FOR_CUR;
	}

	for (i = 0; i < num; i++)
	{
		video_mem_new = malloc(sizeof(video_mem_t) + vm_size);
		if (video_mem_new == NULL)
		{
			return -1;
		}
		video_mem_new->pixel_datas.pixel_data = (unsigned char *)(video_mem_new + 1);

		video_mem_new->id = i;
		video_mem_new->is_framebuffer = 0;
		video_mem_new->video_mem_state = VMS_FREE;
		video_mem_new->picture_state = PS_BLANK;
		video_mem_new->pixel_datas.width = x_res;
		video_mem_new->pixel_datas.height = y_res;
		video_mem_new->pixel_datas.bpp = bpp;
		video_mem_new->pixel_datas.line_bytes = line_bytes;
		video_mem_new->pixel_datas.total_bytes = vm_size;

		video_mem_new->next = video_mem_head;
		video_mem_head = video_mem_new;
	}

	return 0;
}

/* cur == 1表示 */
video_mem_t *get_video_mem(int id, int cur)
{
	video_mem_t *temp = video_mem_head;

	/* 1.取出空闲的ID相同的videomem */
	while (temp)
	{
		if ((temp->video_mem_state == VMS_FREE) && (temp->id == id))
		{
			temp->video_mem_state = cur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return temp;
		}
		temp = temp->next;
	}


	/* 2.取出任意一个空闲的videomem */
	temp = video_mem_head;
	while (temp)
	{
		if (temp->video_mem_state == VMS_FREE)
		{
			temp->id = id;
			temp->picture_state = PS_BLANK;
			temp->video_mem_state = cur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return temp;
		}
		temp = temp->next;
	}

	return NULL;
}

void put_video_mem(video_mem_t *video_mem)
{
	video_mem->video_mem_state = VMS_FREE;
}

void clear_video_mem(video_mem_t *video_mem, unsigned int color)
{
	int i = 0;

	unsigned int *vm_32_bpp = (unsigned int *)video_mem->pixel_datas.pixel_data;

	while (i < video_mem->pixel_datas.total_bytes)
	{
		*vm_32_bpp = color;
		vm_32_bpp++;
		i += 4;
	}
}

video_mem_t *get_dev_video_mem()
{
	video_mem_t *temp = video_mem_head;

	while (temp)
	{
		if (temp->is_framebuffer)
		{
			return temp;
		}
		temp = temp->next;
	}

	return NULL;
}


int display_manager_init(void)
{
	int		err;

	err = fb_init();

	return err;
}

