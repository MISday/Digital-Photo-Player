/**********************************************************************

    > File Name: fb.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月12日 星期日 17时16分41秒

***********************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "config.h"
#include "display_manager.h"

#define FB_PATH	"/dev/fb0"

static int fb_device_init(void);
static int fb_show_pixel(int x, int y, unsigned int color);
static int fb_clean_screen(unsigned int backgroundcolor);
static int fb_show_page(video_mem_t *video_mem);


static int	fb_fd;
static struct fb_var_screeninfo fb_var;
static struct fb_fix_screeninfo fb_fix;

static unsigned long fb_mem_length;
static unsigned int *fb_mem_start;
static unsigned int *fb_mem_end;


static display_operation_t fb_display_operation = 
{
	.name = "fb",

	.device_init 	= fb_device_init,
	.show_pixel		= fb_show_pixel,	
	.clean_screen	= fb_clean_screen,
	.show_page		= fb_show_page,
};


static int fb_device_init(void)
{
	int ret;

	fb_fd = open(FB_PATH, O_RDWR);
	if (fb_fd < 0)
	{
		DEBUG_PRINTF("can't open %s\n", FB_PATH);
		return -1;
	}

    ret = ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_fix);
	if (ret < 0)
    {
		DEBUG_PRINTF("can't get fb's fix\n");
        goto err1;
    }   

	ret = ioctl(fb_fd, FBIOGET_VSCREENINFO, &fb_var);
	if (ret < 0)
    {
		DEBUG_PRINTF("can't get fb's var\n");
        goto err1;
    }   

	fb_mem_length = fb_var.xres_virtual * 	\
					fb_var.yres_virtual * 	\
					fb_var.bits_per_pixel / 8;

	fb_mem_start = (unsigned int *)mmap(NULL, fb_mem_length, 
					PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (MAP_FAILED == fb_mem_start)
    {
		DEBUG_PRINTF("can't mmap\n");
        goto err1;
    }

	fb_mem_end = fb_mem_start + fb_mem_length;

	fb_display_operation.Xres = fb_var.xres;
	fb_display_operation.Yres = fb_var.yres;
	fb_display_operation.bpp = fb_var.bits_per_pixel;
	fb_display_operation.display_mem = (unsigned char *)fb_mem_start;

	return 0;
err1:
	close(fb_fd);	

	return -1;
}

//								   col   row
static int fb_show_pixel(int x, int y, unsigned int color)
{
	if ((x > fb_display_operation.Xres) || (y > fb_display_operation.Yres))
	{
		DEBUG_PRINTF("out of region\n");
		return -1;
	}

	switch (fb_display_operation.bpp)
	{
		case 8:
		{
			color = color << 8	| color;
			color = color << 16	| color;
			*(fb_mem_start + y * fb_display_operation.Xres + x) = color;
			break;
		}

		case 32:
		{
			*(fb_mem_start + y * fb_display_operation.Xres + x) = color;
			break;
		}

		default :
		{
			DEBUG_PRINTF("can't support %d bpp\n", fb_display_operation.bpp);
			return -1;
		}
	};

	return 0;
}

static int fb_clean_screen(unsigned int backgroundcolor)
{
	unsigned long i = 0;

	switch (fb_display_operation.bpp)
	{
		case 8:
		{
			while (i < fb_mem_length)
			{
				backgroundcolor = backgroundcolor << 8	| backgroundcolor;
				backgroundcolor = backgroundcolor << 16	| backgroundcolor;
				*(fb_mem_start + i) = backgroundcolor;
				i++;
			}
			break;
		}

		case 32:
		{
			memset(fb_mem_start, backgroundcolor, fb_mem_length);
			break;
		}

		default :
		{
			DEBUG_PRINTF("can't support %d bpp\n", fb_display_operation.bpp);
			return -1;
		}		
	}

	return 0;

}

static int fb_show_page(video_mem_t *video_mem)
{
	memcpy(fb_display_operation.display_mem, 
			video_mem->pixel_datas.pixel_data, 
			video_mem->pixel_datas.total_bytes);

	return 0;
}


int fb_init(void)
{
	register_display_operation(&fb_display_operation);

	return 0;
}

