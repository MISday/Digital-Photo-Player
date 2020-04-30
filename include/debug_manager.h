/**********************************************************************

    > File Name: debug_manager.h

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月21日 星期二 14时28分47秒

***********************************************************************/

#ifndef __DEBUG_MANAGER_H__
#define __DEBUG_MANAGER_H__

#define	APP_EMERG	"<0>"	/* system is unusable			*/
#define	APP_ALERT	"<1>"	/* action must be taken immediately	*/
#define	APP_CRIT	"<2>"	/* critical conditions			*/
#define	APP_ERR	    "<3>"	/* error conditions			*/
#define	APP_WARNING	"<4>"	/* warning conditions			*/
#define	APP_NOTICE	"<5>"	/* normal but significant condition	*/
#define	APP_INFO	"<6>"	/* informational			*/
#define	APP_DEBUG	"<7>"	/* debug-level messages			*/

#define DEFAULT_DBGLEVEL 7

typedef struct _debug_operation
{
	char *name;

	int	flag;		/* flag为1表示该debug方式可用 */

	int (*debug_init)(void);
	int (*debug_exit)(void);
	int (*debug_print)(char *str);

	struct _debug_operation *next;
}  debug_operation_t;

int register_debug_operation(debug_operation_t *debug_opr);

void show_all_debug_operation(void);

debug_operation_t *get_debug_operation(char *name);

int all_debug_devices_init(void);

int debug_manager_init(void);

int set_debug_level(char *str);
int set_debug_device(char *str);
int debug_printf(const char *format, ...);

int stdout_init(void);
int netprint_init(void);


#endif /* __DEBUG_MANAGER_H__ */

