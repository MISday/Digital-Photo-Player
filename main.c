/**********************************************************************

    > File Name: main.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月12日 星期日 17时13分13秒

***********************************************************************/

#include "config.h"
#include "display_manager.h"
#include "fonts_manager.h"
#include "encoding_manager.h"
#include "input_manager.h"
#include "debug_manager.h"
#include "page_manager.h"
#include "picture/picture_parser.h"
#include "picture/bmp.h"
#include "render.h"

int main(int argc, char *argv[])
{
	int ret;

	debug_manager_init();
	all_debug_devices_init();

	display_manager_init();
	select_default_display_device("fb");
	
	input_manager_init();
	all_input_devices_init();

	fonts_manager_init();
	set_fonts_detail("freetype", "msyh.ttc", 24);

	alloc_video_mem(6);	

	page_manager_init();
	get_page_action("main")->run();
	
	return 0;
}

