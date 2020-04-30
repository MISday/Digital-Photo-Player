/**********************************************************************

    > File Name: freetype.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月12日 星期日 17时16分00秒

***********************************************************************/

#include "config.h"
#include "fonts_manager.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

static int freetype_font_init(char *font_file, unsigned int font_size);
static int freetype_get_font_bitmap(unsigned int code, font_bitmap_t *font_bitmap);
static void freetype_set_font_size(unsigned int size);

static FT_Library freetype_library;
static FT_Face freetype_face;
static FT_GlyphSlot freetype_slot;

static font_operation_t freetype_font_operation =
{
	.name 				= "freetype",

	.font_init 			= freetype_font_init,
	.get_font_bitmap 	= freetype_get_font_bitmap,
	.set_font_size		= freetype_set_font_size,
};

static int freetype_font_init(char *font_file, unsigned int font_size)
{
	int ret;

	ret = FT_Init_FreeType(&freetype_library);
	if (ret)
	{
		DEBUG_PRINTF("FT_Init_FreeType failed\n");
		goto err1;
	}

	//	获取字体对象
	ret = FT_New_Face(freetype_library, font_file, 0, &freetype_face);
	if (ret == FT_Err_Unknown_File_Format)
	{
		DEBUG_PRINTF("The font file can't be known\n");
		goto err2;
	}
	else if (ret)
	{
		DEBUG_PRINTF("FT_New_Face failed\n");
		goto err1;
	}

	freetype_slot = freetype_face->glyph;

	//	设置字体大小
	ret = FT_Set_Pixel_Sizes(freetype_face, font_size, 0);
	if (ret)
	{
		DEBUG_PRINTF("FT_Set_Pixel_Sizes failed\n");
		goto err2;
	}

	return 0;
	
err2:
	FT_Done_Face(freetype_face);
	
err1:
	FT_Done_FreeType(freetype_library);

	return -1;
}

static int freetype_get_font_bitmap(unsigned int code, font_bitmap_t *font_bitmap)
{
	int ret;
	int pen_x = font_bitmap->current_origin_x;		//	单位为1/64个像素
	int pen_y = font_bitmap->current_origin_y;

	ret = FT_Load_Char(freetype_face, code, FT_LOAD_RENDER);
	if (ret)
	{
		DEBUG_PRINTF("FT_Load_Char error for code : 0x%x\n", code);
		return -1;
	}

	/* font_bitmap_t 记录的是相对位置 */
	font_bitmap->pos_left		= pen_x + freetype_slot->bitmap_left;
	font_bitmap->pos_top 		= pen_y - freetype_slot->bitmap_top; 	//	坐标系转换,top指笛卡尔坐标系的top,实际上是lcd坐标系的
	font_bitmap->pos_right		= font_bitmap->pos_left + freetype_slot->bitmap.width;
	font_bitmap->pos_bottom		= font_bitmap->pos_top	+ freetype_slot->bitmap.rows;

	font_bitmap->bpp			= 8;
	font_bitmap->buffer			= freetype_slot->bitmap.buffer;

	font_bitmap->next_origin_x	= pen_x + freetype_slot->advance.x / 64;
	font_bitmap->next_origin_y	= pen_y;
	
	return 0;
}

static void freetype_set_font_size(unsigned int size)
{
	int ret;

	ret = FT_Set_Pixel_Sizes(freetype_face, size, 0);
	if (ret)
	{
		DEBUG_PRINTF("FT_Set_Pixel_Sizes failed\n");
		return ;
	}	
}


int freetype_init(void)
{
	return register_font_operation(&freetype_font_operation);
}


