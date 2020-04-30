/**********************************************************************

    > File Name: bmp.h

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月24日 星期五 16时01分26秒

***********************************************************************/

#ifndef __BMP_H__
#define __BMP_H__

typedef struct _bitmap_header
{
	unsigned char type[2];			//	一般是`B`与`M`
	unsigned long size;				//	文件大小	，32位机器中long一般是4bytes
	unsigned short reserved1;		//	保留1
	unsigned short reserved2;		//	保留2
	unsigned long offset_data;		//	数据偏移量
} __attribute__((packed)) bitmap_header_t;

typedef struct _bitmap_info_header
{
	unsigned long size;						//	头结构大小，一般为40 bytes
	long width;								//	位图宽度 
	long height;							//	位图高度，有正反之分
	unsigned short planes;					//	颜色平面数，1为有效值
	unsigned short bpp;						
	unsigned long compression;				//	压缩方式
	unsigned long image_size;				//	原始位图数据的大小，以字节为单位 
	long x_pixels_per_meter;				
	long y_pixels_per_meter;
	unsigned long palette_used;				//	调色板的颜色数
	unsigned long palette_important;		//	调色板相关，一般不适用

} __attribute__((packed)) bitmap_info_header_t;

#endif /* __BMP_H__ */

