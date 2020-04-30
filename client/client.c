/**********************************************************************

    > File Name: client.c

    > Author: 0nism

    > Email: alikefd@163.com

    > Description: 

    > Created Time: 2020年04月22日 星期三 12时57分34秒

***********************************************************************/

#include <sys/types.h>         
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#define STRCMP( a, R, b )   (strcmp(a, b)  R 0)

#define SERVER_PORT 6666

static void help(char *argv[])
{
	printf("Usage:\n");
	printf("%s <server_ip> dbglevel=<0-9>\n", argv[0]);
	printf("%s <server_ip> stdout=0|1\n", argv[0]);
	printf("%s <server_ip> netprint=0|1\n", argv[0]);
	printf("%s <server_ip> show\n", argv[0]);	
}

int main(int argc, char *argv[])
{
	int client_fd;
	struct sockaddr_in server_addr;
	char recv_buf[1024];

	int ret;
	int addr_length;
	int recv_length;
	int send_length;

	if (argc != 3)
	{
		help(argv);
		return -1;
	}

	client_fd = socket(AF_INET, SOCK_DGRAM, 0);

	server_addr.sin_family 	= AF_INET;
	server_addr.sin_port 	= htons(SERVER_PORT);

	if (0 == inet_aton(argv[1], &server_addr.sin_addr))
	{
		printf("invalid server_ip\n");
		return -1;
	}
	memset(server_addr.sin_zero, 0, 8);

	printf("argv[2] : %s\n", argv[2]);

	if (STRCMP(argv[2], ==, "show"))
	{
		addr_length = sizeof(struct sockaddr);
		send_length = sendto(client_fd, "setclient", 9, 0, 
						(const struct sockaddr *)&server_addr, addr_length);

		while (1)
		{
			addr_length = sizeof(struct sockaddr);
			recv_length = recvfrom(client_fd, recv_buf, 1024-1, 0, 
							(struct sockaddr *)&server_addr, &addr_length);
			if (recv_length > 0)
			{
				recv_buf[recv_length] = '\0';
				printf("%s", recv_buf);
			}
			memset(recv_buf, 0, sizeof(recv_buf));
		}
	}
	else
	{
		addr_length = sizeof(struct sockaddr);
		send_length = sendto(client_fd, argv[2], strlen(argv[2]), 0, 
						(const struct sockaddr *)&server_addr, addr_length);		
	}

	return 0;
}
