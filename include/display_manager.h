/**********************************************************************

    > File Name: display_manager.h

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月12日 星期日 17时14分12秒

***********************************************************************/

#ifndef __DISPLAY_MANAGER_H__
#define __DISPLAY_MANAGER_H__

#include "picture/picture_parser.h"

typedef enum
{
	VMS_FREE = 0,				//	该显存区域未使用
	VMS_USED_FOR_PREPARE,		
	VMS_USED_FOR_CUR,
} video_mem_state_e;

typedef enum
{
	PS_BLANK = 0,				//	picture_data中无数据
	PS_GENERATING,				
	PS_GENERATED,
} picture_state_e;

typedef struct _video_mem
{
	int	id;

	int is_framebuffer;
	video_mem_state_e	video_mem_state; 
	picture_state_e		picture_state;
	pixel_datas_t		pixel_datas;

	struct _video_mem	*next;
} video_mem_t;

typedef struct _display_operation
{
	char	*name;		
	int		Xres;		//	X轴分辨率		
	int		Yres;		//	Y轴分辨率
	int		bpp;		//	位深

	unsigned char * display_mem;

	int		(*device_init)(void);
	int		(*show_pixel)(int x, int y, unsigned int color);
	int		(*clean_screen)(unsigned int backgroundcolor);
	int		(*show_page)(video_mem_t * video_mem);
	
	struct	_display_operation *next;
} display_operation_t;

/*
 * display_operation
 */
int register_display_operation(display_operation_t *disp_opr);
void show_all_display_operation(void);
display_operation_t *get_display_operation(char *name);
int display_manager_init(void);
display_operation_t *get_default_display_device(void);
void select_default_display_device(char *name);
int get_display_resolution(int *xres, int *yres, int *bpp);
int fb_init(void);

/*
 * 显存管理
 */
int alloc_video_mem(int num);
video_mem_t *get_video_mem(int id, int cur);
void put_video_mem(video_mem_t *video_mem);
void clear_video_mem(video_mem_t *video_mem, unsigned int color);

video_mem_t *get_dev_video_mem();



#endif /* __DISPLAY_MANAGER_H__ */

