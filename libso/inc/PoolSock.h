#ifndef _POOLSOCK_H_
#define _POOLSOCK_H_

#ifdef __cplusplus
extern 'C'
{
#endif
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

//�����붨��  
#define Sck_Ok             	             0
#define Sck_BaseErr   		               3000

#define Sck_ErrParam                	   (Sck_BaseErr+1)
#define Sck_ErrTimeOut                	 (Sck_BaseErr+2)
#define Sck_ErrPeerClosed                (Sck_BaseErr+3)
#define Sck_ErrMalloc			   	         	 (Sck_BaseErr+4)

#define Sck_Err_Pool_CreateConn				   (Sck_BaseErr+20)  //�������ӳ� ��û�дﵽ�����������
#define Sck_Err_Pool_terminated				   (Sck_BaseErr+21)  //����ֹ
#define Sck_Err_Pool_GetConn_ValidIsZero (Sck_BaseErr+22)  //��Ч����������
#define Sck_Err_Pool_HaveExist				   (Sck_BaseErr+22)  //�����Ѿ��ڳ���
#define Sck_Err_Pool_ValidBounds			   (Sck_BaseErr+22)  //��Ч������Ŀ���������������

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�ͻ��� ��ʼ��
 *===========================================================*/
int sckClient_init();

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�ͻ��� ���ӷ�����
 *===========================================================*/
int sckClient_connect(char *ip, int port, int connecttime, int *connfd);

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�ͻ��� �رպͷ���˵�����
 *===========================================================*/
int sckClient_closeconn(int connfd);

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:
 *===========================================================*/
int sckClient_send(int connfd, int sendtime, unsigned char *data, int datalen);

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:
 *===========================================================*/
int sckClient_rev(int connfd, int revtime, unsigned char **out, int *outlen); //1

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:
 *===========================================================*/
int sck_FreeMem(void **buf);

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:
 *===========================================================*/
int sckClient_destroy();


typedef struct _SCKClitPoolParam
{
	char 	serverip[64];
	int 	serverport;
	int 	bounds; //������
	int 	connecttime;
	int 	sendtime;
	int 	revtime;
}SCKClitPoolParam;

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�ͻ��� socket�س�ʼ��
 *===========================================================*/
int sckCltPool_init(void **handle, SCKClitPoolParam *param);

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�ͻ��� socket�� ��ȡһ������ 
 *===========================================================*/
int sckCltPool_getConnet(void *handle, int *connfd);

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�ͻ��� socket�� �������� 
 *===========================================================*/
int sckCltPool_send(void *handle, int  connfd,  unsigned char *data, int datalen);

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�ͻ��� socket�� ��������
 *===========================================================*/
int sckCltPool_rev(void *handle, int  connfd, unsigned char **out, int *outlen); //1

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�ͻ��� socket�� �����ӷŻ� socket����  
 *===========================================================*/
int sckCltPool_putConnet(void *handle, int connfd, int validFlag); //0���� 1

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�ͻ��� socket�� �������� 
 *===========================================================*/
int sckCltPool_destroy(void *handle);

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�������˳�ʼ��
 *===========================================================*/
int sckServer_init(char *addr, int port, int *listenfd);

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:
 *===========================================================*/
int sckServer_accept(int listenfd, int timeout, int *connfd);

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�������˷��ͱ���
 *===========================================================*/
int sckServer_send(int connfd, int timeout, unsigned char *data, int datalen);

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�������˶˽��ܱ��� 
 *===========================================================*/
int sckServer_rev(int  connfd, int timeout, unsigned char **out, int *outlen); //1

/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�رշ���˾��
 *===========================================================*/
int sckServer_close(int connfd);


/*=========================================================
 *�������:        
 *��������: 
 *����ֵ:   
 *����˵��:�������˻����ͷ� 
 *===========================================================*/
int sckServer_destroy();

#ifdef __cplusplus
}
#endif


#endif