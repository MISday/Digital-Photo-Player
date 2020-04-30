/**********************************************************************

    > File Name: crt.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月16日 星期四 21时46分29秒

***********************************************************************/

static int crt_device_init(void);
static int crt_show_pixel(int x, int y, unsigned int color);
static int crt_clean_screen(unsigned int backgroundcolor);


static display_operation_t crt_display_operation = 
{
	.name = "crt",

	.device_init 	= crt_device_init,
	.show_pixel		= crt_show_pixel,	
	.clean_screen	= crt_clean_screen,
};


static int crt_device_init(void)
{

}

static int crt_show_pixel(int x, int y, unsigned int color)
{

}

static int crt_clean_screen(unsigned int backgroundcolor)
{

}

