#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "PoolSock.h"
                          
void *OldSecuBoard(void *arg)
{
	int 		          ret = 0;
	int 		          timeout = 3;
	int 		          connfd = (int)arg;
	unsigned char 	  *out = NULL;
	int 			        outlen = 0;
	
	while (1)
	{	
		ret =  sckServer_rev(connfd, timeout, &out, &outlen); //1
		if (ret == Sck_ErrPeerClosed)
		{
			printf("�������������˼�⵽�ͻ�����һ�������ѹر� \n");
			break;
		}
		else if (ret == Sck_ErrTimeOut)
		{
			printf("��������������recv��ʱ\n");
			continue;
		}
		else if (ret != 0)
		{
			printf("�������������� sckServer_rev() err\n");
			break;
		}
		printf("�������յ��ı���Ϊout:%s \n", out);
	
		ret =  sckServer_send(connfd, timeout, out, outlen);
		if (ret == Sck_ErrPeerClosed)
		{
			sck_FreeMem((void **)&out);
			printf("�������������˼�⵽�ͻ�����һ�������ѹر�\n");
			break;
		}
		else if (ret == Sck_ErrTimeOut)
		{
			sck_FreeMem((void **)&out);
			printf("��������������send��ʱ\n");
			continue;
		}
		else if (ret != 0)
		{
			sck_FreeMem((void **)&out);
			printf("�������������� sckServer_send() err\n");
			break;
		}
		sck_FreeMem((void **)&out);
	}
	sckServer_close(connfd);
	return NULL;
}


int main()
{
	int 		ret 		= 0;
	char    ip_addr[30] = "192.168.5.248";
	int		 	port 		= 9999;
	int 		listenfd 	= 0;
	int 		timeout		= 3;
	int 		connfd = 0;
	pthread_t 	pid;
	
	ret = sckServer_init(ip_addr, port, &listenfd);
	if (ret != 0)
	{
		printf("func sckServer_init() err:%d \n", ret);
		return ret;
	}
	
	while (1)
	{
		ret = sckServer_accept(listenfd, timeout, &connfd);
		if (ret ==Sck_ErrTimeOut)
		{
			printf("func sckServer_accept() Sck_ErrTimeOut\n");
			continue;
		}
		else if (ret != 0)
		{
			ret = 2;
			printf("fun sckServer_accept() err :%d \n", ret);
			break;
		}
		pthread_create(&pid, NULL,  OldSecuBoard, (void *)(connfd));
	}
	int sckServer_destroy();
	printf("OldSecuServer is End....\n");	
}