/**********************************************************************

    > File Name: config.h

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月12日 星期日 17时13分35秒

***********************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>
#include <string.h>

#include "debug_manager.h"

#define STRCMP( a, R, b )   (strcmp(a, b)  R 0)
#define STRNCMP( a, R, b, len ) (strncmp(a, b, len)  R 0)

#define COLOR_BACKGROUND   0xE7DBB5  /* 泛黄的纸 */
#define COLOR_FOREGROUND   0x514438  /* 褐色字体 */

//#define DEBUG_PRINTF(...)
#define DEBUG_PRINTF debug_printf

//#define POLL			//	轮询
//#define SELECT		//	select/poll
//#define EPOLL
#define THREAD			//	多线程


/*****************************触摸屏输入方式***************************/
//#define PRESS
//#define SLIP
#define DIGITAL

/*******************************图象处理********************************/
#define NEAREST_INTERPOLATION		//	最近邻内插
//#define BILINEAR_INTERPOLATION		//	双线性内插

#define ICON_PATH "/etc/digitpic/icons"
#endif /* __CONFIG_H__ */