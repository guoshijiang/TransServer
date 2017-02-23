/*=========================================================
 *�ļ�����:  TransServer.h
 *�������ڣ�2017-2-17
 *�޸ļ�¼��
 *  2017-2-17 �Ľ��ڶ����汾����Ϣ���У���Ϊsocket����Ϣ
 *����������
 ===========================================================*/
#include "TransServer.h"

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include "PoolSock.h"
#include "Config.h"
#include "Cfifo.h"
#include "WriteLog.h"
#include "HandleBuss.h"

void *mystart_routine(void *arg)
{
	int 		ret = 0;
	int 		timeout = 3;
	int 		connfd = (int)arg;
	
	unsigned char 	*out = NULL;
	int 			outlen = 0;
	
	while (1)
	{	
		//�������˶˽��ܱ���
		ret =  sckServer_rev(connfd, timeout, &out, &outlen); //1
		if (ret == Sck_ErrPeerClosed)
		{
			//printf("aaaaa \n");
			printf("�������˼�⵽�ͻ�����һ�������ѹر� \n");
			break;
		}
		else if (ret == Sck_ErrTimeOut)
		{
			printf("��������send��ʱ\n");
			continue;
		}
		else if (ret != 0)
		{
			printf("�������� sckServer_send() err\n");
			break;
		}
	
		printf("out:%s \n", out);
		
		//�������˷��ͱ���
		ret =  sckServer_send(connfd, timeout, out, outlen);
		if (ret == Sck_ErrPeerClosed)
		{
			sck_FreeMem((void **)&out);
			printf("�������˼�⵽�ͻ�����һ�������ѹر�\n");
			break;
		}
		else if (ret == Sck_ErrTimeOut)
		{
			sck_FreeMem((void **)&out);
			printf("��������send��ʱ\n");
			continue;
		}
		else if (ret != 0)
		{
			sck_FreeMem((void **)&out);
			printf("�������� sckServer_send() err\n");
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
	char    ip[30]  = "192.168.5.248";
	int		 	port 		= 8888;
	int 		listenfd 	= 0;
	int 		timeout		= 3;
	int 		connfd = 0;
	pthread_t 	pid;
	
	
	//��������
	//�������˳�ʼ��
	ret = sckServer_init(ip, port, &listenfd);
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
	
		pthread_create(&pid, NULL,  mystart_routine, (void *)(connfd));

	}
	//�������˻����ͷ� 
	int sckServer_destroy();
	printf("hello....\n");		
	
}