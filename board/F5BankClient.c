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

void *F5BankClient (void *arg)
{
	int 			           ret = 0;
	void 			           *handle = NULL;
	int 			           connfd = 0;
	unsigned char 	     *data = "qqqq";
	int 			           datalen = 7;
	unsigned char 	     *out = NULL;
	int 			           outlen = 0;
	
	handle = arg;
	
	ret = sckCltPool_getConnet(handle, &connfd);
	if (ret != 0)
	{
		printf("func sckCltPool_getConnet() err:%d \n", ret);
		return NULL;
	}

	ret = sckCltPool_send(handle, connfd,  data, datalen);
	if (ret != 0)
	{
		printf("func sckCltPool_send() err:%d \n", ret);
		return NULL;
	}

	ret = sckCltPool_rev(handle, connfd, &out, &outlen); 
	if (ret != 0)
	{
		printf("func sckCltPool_rev() err:%d \n", ret);
		return NULL;
	}
	printf("client out:%s \n", out);
	sck_FreeMem((void **)&out);
	ret = sckCltPool_putConnet(handle, connfd, 1); 
	
	return ;
}

int main()
{
	int 	               ret = 0;
	int 	               i = 0;
	void 	               *handle = NULL;
	pthread_t            pid[200];

	SCKClitPoolParam 	param;
	strcpy(param.serverip, "192.168.5.248");
	param.serverport = 8888;
	param.bounds = 1;
	param.connecttime = 3;
	param.sendtime = 3;
	param.revtime = 3;
	
	ret = sckCltPool_init(&handle, &param);
	if (ret != 0)
	{
		printf("func sckCltPool_init() err:%d \n", ret);
		return ret;
	}
	
	for (i=0; i<200; i++)
	{
	 	pthread_create( &pid[i], NULL, F5BankClient, handle);
	}

  for (i=0; i<200; i++)
  {
    	 pthread_join(pid[i], NULL);
  }
	ret = sckCltPool_destroy(handle);
	printf("F5BankClient is End\n");	
}