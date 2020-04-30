/**********************************************************************

    > File Name: render.h

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月25日 星期六 16时35分28秒

***********************************************************************/

#ifndef __RENDER_H__
#define __RENDER_H__

#include "display_manager.h"
#include "page_manager.h"

int picture_zoom(pixel_datas_t *origin_picture, pixel_datas_t *zoom_picture);
int picture_merge(int x, int y, pixel_datas_t *src_picture, pixel_datas_t *dst_picture);


int merge_string_to_center_of_rect_in_video_mem(layout_t *layout, unsigned char *text_string, video_mem_t *video_mem);

void flush_video_mem_to_dev(video_mem_t *video_mem);
int get_pixel_datas_for_icon(char *file_name, pixel_datas_t *pixel_datas);
void free_pixel_datas_from_icon(pixel_datas_t * pixel_datas);

void press_button(layout_t *layout);
void release_button(layout_t *layout);


#endif /* __RENDER_H__ */
