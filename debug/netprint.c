/**********************************************************************

    > File Name: netprint.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月20日 星期一 00时16分35秒

***********************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

#include "config.h"
#include "debug_manager.h"

#define SERVER_PORT 6666
#define NET_PRINT_BUF_SIZE (1024*16)

static int server_fd;

static struct sockaddr_in server_addr;
static struct sockaddr_in client_addr;

/* 循环数组，最后一个位置省缺 */
static char *netprint_buf;
static int read_position = 0;
static int write_postion = 0;

static pthread_t send_thread_id;
static pthread_t recv_thread_id;

static pthread_mutex_t	net_debug_send_mutex	= PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t	net_debug_send_conv		= PTHREAD_COND_INITIALIZER;

static int connected = 0;

static int netprint_debug_init(void);
static int netprint_debug_exit(void);
static int netprint_debug_print(char *str);

static debug_operation_t netprint_debug_operation = 
{
	.name = "netprint",

	.flag = 1,

	.debug_init		= netprint_debug_init,
	.debug_exit 	= netprint_debug_exit,
	.debug_print	= netprint_debug_print,

};

static int is_full(void)
{
	return (((write_postion + 1) % NET_PRINT_BUF_SIZE) == read_position);
}

static int is_empty(void)
{
	return (write_postion == read_position);
}

static int put_data(char c)
{
	if (is_full())
		return -1;
	else
	{
		netprint_buf[write_postion] = c;
		write_postion = (write_postion + 1) % NET_PRINT_BUF_SIZE;
		return 0;
	}
}

static int get_data(char *c)
{
	if (is_empty())
		return -1;
	else
	{
		*c = netprint_buf[read_position];
		read_position = (read_position + 1) % NET_PRINT_BUF_SIZE;
		return 0;
	}
}

static void *net_debug_recv_thread_worker(void * arg)
{
	socklen_t 			addr_len;
	int					recv_length;
	char				recv_buf[1024];
	struct sockaddr_in	client_addr_temp;

	while (1)
	{
		addr_len = sizeof(struct sockaddr);
	
		recv_length = recvfrom(server_fd, recv_buf, 1023, 0,  
						(struct sockaddr *)&client_addr_temp, &addr_len);
		if (recv_length > 0)
		{
			recv_buf[recv_length] = '\0';

			/*
			 *	数据解析
			 *	setclient				:	设置接收debug信息的client
			 *	dbg_level				:	修改debug级别
			 *	stdout = 0				:	关闭stdout打印
			 *	stdout = 1				:	打开stdout打印
			 *	netprint = 0			:	关闭网络debug
			 *	netprint = 1			:	打开网络debug
			 */

			 if (STRCMP(recv_buf, ==, "setclient"))
			 {
				client_addr = client_addr_temp;
				connected = 1;
			 }
			 else if (STRNCMP(recv_buf, ==, "dbglevel=", 9))
			 {
				set_debug_device(recv_buf);
			 }
			 else
			 {
				set_debug_device(recv_buf);
			 }
		}

	}

	return NULL;
}

static void *net_debug_send_thread_worker(void *arg)
{
	char temp_buf[512] = {0};
	int i;
	char c;
	int addr_length;
	int	send_length;

	while (1)
	{
		/* 等待条件变量 */
		pthread_mutex_lock(&net_debug_send_mutex);
		pthread_cond_wait(&net_debug_send_conv, &net_debug_send_mutex);
		pthread_mutex_unlock(&net_debug_send_mutex);

		while (connected && !is_empty())
		{
			i = 0;

			/* 读取缓冲取数据 */
			while ((i < 512) && (0 == get_data(&c)))
			{
				temp_buf[i] = c;
				i++;
			}

			addr_length = sizeof(struct sockaddr);
			send_length = sendto(server_fd, temp_buf, i, 0,
						(const struct sockaddr *)&client_addr, addr_length);
		}
	}

	return NULL;
}

static int netprint_debug_init(void)
{
	int ret;

	/* UDP */
	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_fd < 0)
	{
		printf("socket create failed\n");
		return -1;
	}

	server_addr.sin_family	= AF_INET;
	server_addr.sin_port	= htons(SERVER_PORT);	/* host to net, short */
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(server_addr.sin_zero, 0, 8);

	ret = bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	if (ret < 0)
	{
		printf("bind failed\n");
		return -1;
	}

	netprint_buf = malloc(NET_PRINT_BUF_SIZE);
	if (NULL == netprint_buf)
	{
		close(server_fd);
		return -1;
	}

	/* 发送线程 */
	pthread_create(&send_thread_id, NULL, net_debug_send_thread_worker, NULL);

	/* 接收线程 */
	pthread_create(&recv_thread_id, NULL, net_debug_recv_thread_worker, NULL);
	
	return 0;
}

static int netprint_debug_exit(void)
{
	close(server_fd);
	free(netprint_buf);

	return 0;
}

static int netprint_debug_print(char *str)
{
	/* 数据放入缓冲区 */
	int i;

	for (i = 0; i < strlen(str); i++)
	{
		if (0 != put_data(str[i]))
			break;
	}

	/* 唤醒发送线程 */
	pthread_mutex_lock(&net_debug_send_mutex);
	pthread_cond_signal(&net_debug_send_conv);
	pthread_mutex_unlock(&net_debug_send_mutex);

	return i;
}

int netprint_init(void)
{
	return register_debug_operation(&netprint_debug_operation);
}


