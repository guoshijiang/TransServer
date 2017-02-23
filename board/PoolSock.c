/*=========================================================
 *�ļ�����:  PoolSock.c
 *�������ڣ�2017-2-10
 *�޸ļ�¼��
 *  2017-2-10  �״δ���
 *����������Socket���ӳ�
 ===========================================================*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <sys/time.h>
#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "PoolSock.h"
#include "SockLog.h"

//Socket���ӳؽṹ
typedef struct _SockePoolHandle
{
	int *			fdArray;			    //Socket���ӳ�
	int	*			statusArray;			//ÿ�����ӵ�״̬  eg: statusArray[0] =  1��ʾ ������Ч statusArray[0] =  0��ʾ ������Ч
	int				valid;				    //Socket��Ч������Ŀ 
	int				nvalid;				    //Socket��Ч������Ŀ 
	int				bounds;				    //Socket���ӳص�����
	
	char 			serverip[128];
	int 			serverport;
	
	int 			connecttime;
	int 			sendtime;
	int 			revtime;
	int				sTimeout;         //û������ʱ���ȴ�֮��
	pthread_mutex_t foo_mutex ;
	
	int				terminated;       //�ж����ӳ��Ƿ��Ѿ���ֹ: 1�Ѿ���ֹ 0û����ֹ
}SockePoolHandle;


//�ͻ���socket�س�ʼ��
int sckCltPool_init(void **handle, SCKClitPoolParam *param)
{
	int		ret =  0, i = 0;
	SockePoolHandle *hdl = NULL;
	//��ʼ�����
	hdl = (SockePoolHandle *)malloc(sizeof(SockePoolHandle));
	if (hdl == NULL)
	{
		ret = Sck_ErrMalloc;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckCltPool_init(), check malloc err");
		return ret;
	}
	memset(hdl, 0, sizeof(hdl));
	strcpy(hdl->serverip, param->serverip);
	hdl->serverport = param->serverport;
	hdl->connecttime = param->connecttime;
	hdl->sendtime = param->sendtime;
	hdl->revtime = param->revtime;
	
	//����������
	hdl->bounds  = param->bounds;
	hdl->valid = 0;
	hdl->nvalid = param->bounds;
	hdl->sTimeout = 1;
	pthread_mutex_init(&(hdl->foo_mutex), NULL); 
	pthread_mutex_lock(& (hdl->foo_mutex) ); //���̼���
	
	//Ϊ���Ӿ�������ڴ�
	hdl->fdArray = (int *)malloc( hdl->bounds  * sizeof(int) );
	if (hdl->fdArray == NULL)
	{
		ret = Sck_ErrMalloc;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckCltPool_init(), check malloc err");
		goto END;
	}
	hdl->statusArray = (int *)malloc( hdl->bounds  * sizeof(int) );
	if (hdl->statusArray == NULL)
	{
		ret = Sck_ErrMalloc;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckCltPool_init(), check malloc err");
		goto END;
	}
	
	ret = sckClient_init();
	if (ret != 0)
	{
		printf("func sckClient_init() err:%d\n", ret);
		goto END;
	}
	
	for (i=0; i<hdl->bounds; i++)
	{
		ret = sckClient_connect(hdl->serverip, hdl->serverport , hdl->connecttime , &(hdl->fdArray[i]) );
		if (ret != 0)
		{
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckClient_connect() err");
			break;
		}
		else
		{
			hdl->statusArray[i] = 1;
			hdl->valid ++;			//Socket��Ч������Ŀ
			hdl->nvalid --;			//Socket��Ч������Ŀ
		}
	}
	
	if (hdl->valid < hdl->bounds ) //����Ч������С������
	{
		ret = Sck_Err_Pool_CreateConn;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"��Ч������С������");
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckClient_init() create connect num err:%d,  hdl->valid: %d , hdl->bounds:%d", ret, hdl->valid,   hdl->bounds);	
		for (i=0; i<hdl->bounds; i++)
		{
			if (hdl->statusArray[i] == 1)
			{
				sckClient_closeconn(hdl->fdArray[i]);
			}
		}
	}
	
END:	
	pthread_mutex_unlock(& (hdl->foo_mutex) ); //����
	if (ret != 0)
	{
		if (hdl->fdArray != NULL) 		free(hdl->fdArray);
		if (hdl->statusArray != NULL) 	free(hdl->statusArray);
		free(hdl);
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func pthread_mutex_unlock() err");
		return ret;
	}
	
	*handle = hdl; //��Ӹ�ֵ
	return ret;	
}

//�ͻ��� socket�� ��ȡһ������ 
int sckCltPool_getConnet(void *handle, int *connfd)
{
	int		ret =  0;
	
	SockePoolHandle *hdl = NULL;
	
	if (handle == NULL || connfd==NULL)
	{
		ret = Sck_ErrParam;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckCltPool_getConnet() (handle == NULL || connfd==NULL) err");
		return ret;
	}
	
	hdl = (SockePoolHandle *)handle;
	pthread_mutex_lock( &(hdl->foo_mutex) ); //���̼��� pthread_mutex_unlock(& (hdl->foo_mutex) ); //����
	
	
	//�� ����ֹ
	if (hdl->terminated == 1)
	{
		ret = Sck_Err_Pool_terminated;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckCltPool_getConnet() (terminated == 1)");
		goto END;
	}
	
	//�� ��Ч���� = 0
	if (hdl->valid == 0)
	{
		usleep(hdl->sTimeout); //���ϼ�΢��
		
		if (hdl->valid == 0)
		{
			ret = Sck_Err_Pool_GetConn_ValidIsZero;
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckCltPool_getConnet() Sck_Err_Pool_GetConn_ValidIsZero err");
			goto END;
		}
		
		//�� ����ֹ
		if (hdl->terminated == 1)
		{
			ret = Sck_Err_Pool_terminated;
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckCltPool_getConnet() (terminated == 1)");
			goto END;
		}
	}
		
	//�ж��������ӵ�״̬
	if (hdl->statusArray[hdl->valid-1] == 0 )
	{
		//���ȶϿ�����������
		if (hdl->fdArray[hdl->valid-1] == 0)
		{
			ret = sckClient_closeconn(hdl->fdArray[hdl->valid-1]);
			if (ret != 0)
			{
				Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckClient_closeconn() err �Ͽ�����������ʧ��");
				hdl->fdArray[hdl->valid-1] = 0;
				//������������
			}
		}
	
		//�����޸� �������� 1��
		ret = sckClient_connect(hdl->serverip, hdl->serverport, hdl->connecttime, &(hdl->fdArray[hdl->valid-1]) );
		if (ret != 0)
		{
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckClient_connect() err �����޸� ��������ʧ��");
			hdl->fdArray[hdl->valid-1] = 0;
			goto END;
		}
	}
	
END:
	if (ret == 0)
	{
		*connfd = hdl->fdArray[ --(hdl->valid) ]; //ע ��Ч������ ��1
	}
	
	pthread_mutex_unlock(& (hdl->foo_mutex) ); //����
	return ret;	
}

//�ͻ���socket�ط������� 
int sckCltPool_send(void *handle, int  connfd,  unsigned char *data, int datalen)
{
	int		ret =  0;
	SockePoolHandle *hdl = NULL;
	
	if (handle==NULL || connfd<0 || data==NULL || datalen<=0 )
	{
		ret = Sck_ErrParam;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckCltPool_send() err (handle==NULL || connfd<0 || data==NULL || datalen<=0) ");
		return ret;
	}
	hdl = (SockePoolHandle *)handle;
	
	//�ͻ��˷��ͱ���
	ret = sckClient_send(connfd, hdl->sendtime , data, datalen);
	if (ret != 0)
	{
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckClient_send() err");
		return ret;
	}
	
	return ret;	
}

//�ͻ���socket�ؽ�������
int sckCltPool_rev(void *handle, int  connfd, unsigned char **out, int *outlen)
{
	int		ret =  0;
	SockePoolHandle *hdl = NULL;
	
	if (handle==NULL || connfd<0 || out==NULL || outlen==NULL )
	{
		ret = Sck_ErrParam;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckCltPool_rev() err, check (handle==NULL || connfd<0 || out==NULL || outlen==NULL )");
		return ret;
	}
	hdl = (SockePoolHandle *)handle;
	
	//�ͻ��˽��ܱ���
	ret = sckClient_rev(connfd, hdl->revtime,  out,  outlen); 
	if (ret != 0)
	{
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckClient_rev() err");
		return ret;
	}
	
	return ret;	
}

//�ͻ���socket�ذ����ӷŻ� socket���� 
int sckCltPool_putConnet(void *handle, int connfd, int validFlag)
{
	int		ret =  0, i = 0;

	SockePoolHandle *hdl = NULL;
	
	if (handle == NULL || connfd<0)
	{
		ret = Sck_ErrParam;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckCltPool_putConnet() err, check (handle == NULL || connfd==NULL)");
		goto END;
	}
	
	hdl = (SockePoolHandle *)handle;
	pthread_mutex_lock( &(hdl->foo_mutex) ); //���̼��� 
	
	//������ֹ
	if (hdl->terminated == 1)
	{
		ret = Sck_Err_Pool_terminated;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckCltPool_putConnet() err, check (func sckCltPool_putConnet() (terminated == 1))");
		hdl->fdArray[hdl->valid] = connfd;
		hdl->valid++;
		goto END;
	}
	
	//�ж������Ƿ��Ѿ����Ž���, �жϸ������Ƿ��Ѿ����ͷ�
	for(i=0; i<hdl->valid; i++)
	{
		if (hdl->fdArray[i] == connfd)
		{
			ret = Sck_Err_Pool_HaveExist;
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckCltPool_putConnet() err, check Sck_Err_Pool_HaveExist ");
			goto END;
		}
	}
	
	//�ж���Ч�������Ƿ��Ѿ��������ֵ
	if (hdl->valid >= hdl->bounds)
	{
		ret = Sck_Err_Pool_ValidBounds ; 
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckCltPool_putConnet() err, check (hdl->valid >= hdl->bounds) ");
		goto END;
	}
	
	//�ж��ͷŵ������Ƿ���Ч
	if (validFlag == 1)
	{
		hdl->fdArray[hdl->valid] = connfd;
		hdl->statusArray[hdl->valid] = 1; //������Ч
		hdl->valid++;  
	}
	else
	{
		int tmpconnectfd = 0;
		//���ȶϿ�����������
		ret = sckClient_closeconn(connfd);
		if (ret != 0)
		{
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckClient_closeconn() err, check (hdl->valid >= hdl->bounds) ");
			//ʧ�ܲ�����
		}
		
		//�����޸� �������� 1�� ���������ӳɹ����ټ������ӳ��У�����������ʧ�ܣ�����Ҫ���뵽���ӳ���
		ret = sckClient_connect(hdl->serverip, hdl->serverport, hdl->connecttime, &tmpconnectfd );
		if (ret != 0)
		{
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckClient_connect() err, �����޸� ��������ʧ��");
		}
		else	
		{
			//��Ч��������1
			hdl->fdArray[hdl->valid] = tmpconnectfd;
			hdl->statusArray[hdl->valid] = 1; //������Ч
			hdl->valid++;  //
		}
	}

END:
	
	pthread_mutex_unlock(& (hdl->foo_mutex) ); //����

	return ret;	
}

//�ͻ��� socket�� ��������
int sckCltPool_destroy(void *handle)
{
	int		ret =  0, i = 0;

	SockePoolHandle *hdl = NULL;
	
	if (handle == NULL )
	{
		ret = Sck_ErrParam;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret," func sckCltPool_destroy() err, check (handle == NULL)");
		return ret;
	}
	
	hdl = (SockePoolHandle *)handle;
	pthread_mutex_lock( &(hdl->foo_mutex) ); //���̼��� 
	
	//������ֹ
	hdl->terminated = 1; //���ӳ����ó���ֹ ״̬
	
	
	for (i=0; i<hdl->bounds; i++)
	{
		if (hdl->fdArray[i] != 0)
		{
			sckClient_closeconn(hdl->fdArray[i]);
		}
	}
	
	if (hdl->fdArray) 
	{
		free(hdl->fdArray); hdl->fdArray = NULL;
	}
	
	if (hdl->statusArray)
	{
		free(hdl->statusArray); hdl->statusArray = NULL;
	}

	sckClient_destroy();
	pthread_mutex_unlock(& (hdl->foo_mutex) ); //����
	
	free(hdl);
	
	return ret;	
}
