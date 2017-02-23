/*=========================================================
 *�ļ����ƣ�MultiBuss.c
 *�������ڣ�2017-1-23
 *�޸ļ�¼��
 *  2017-1-23  �״δ���
 *��������������̴���ҵ����
 ===========================================================*/
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
#include "Sock.h"
#include "Config.h"
#include "Cfifo.h"
#include "WriteLog.h"
#include "BussWorker.h"

typedef struct _ProcessId
{
	int num;
	char chilId[4096];
}ProcessId;

void dealTimeOutProcessFunc(int signo)
{
	printf("hello world\n"); //�����ﴦ��ʱ����ʱ�Ľ���
}

void DealSigchild(int num)
{
  while(waitpid(0, NULL, WNOHANG) > 0); //�����ﴦ����������ҵ������˳��Ľ���
}

int startMain()
{
	int                  fifoFd;    					 //�ܵ����
	char                 logBuf[LOGLEVELLEN];
	char                 buf[MAXLINE];  
	int                  lBankSceuFd; 				 //�������ж˷������ľ��
	ssize_t              wRet;
	ssize_t              rRet;          
	int                  aBankSecuFd;          //AcceptRecvBankSecu���ص�ֵ
	int                  proNum;               //���̵������̸���
	int                  i;
	struct sigaction     newact;
	pid_t                pid;
	pid_t                childId;
	//struct timeval       timvl;
  //struct timeval       *ptimvl;
  //fd_set 	             fdset;
  //char                 timeout[20];
  struct itimerval       it, oldit;
  int                 	 sret;
  char                   sTimeout[20];
  int                    timeout;
  pthread_mutex_t 			 mutex;
  pthread_mutexattr_t 	 mutexattr;
  int                    ret = 0;
	int 	                 shmid;
	ProcessId              *cPid = NULL;
	int                    pidNum = 0;
	
	fifoFd  = IPCFifoOpen("../ipc/myfifo", O_WRONLY);
	memset(logBuf, 0x00, sizeof(logBuf));
	if(fifoFd == -1)
	{
	 sprintf(logBuf, "%s", "�򿪹ܵ�ʧ��");
 	 WriteSySLog(3, logBuf);
   return FIFOIPC_OpenErr;
	}
	lBankSceuFd = ListenBankSecu("bankIP", "bankPort");
	memset(logBuf, 0, sizeof(logBuf));
	if(lBankSceuFd == LISTENBANKSECUERR)
	{
		sprintf(logBuf, "%s", "�������ж�F5����������Ϣʧ��");
		wRet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wRet == -1)
		{
			sprintf(logBuf, "%s", "д�ܵ���������");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
  newact.sa_handler = DealSigchild;
  sigemptyset(&newact.sa_mask);
  newact.sa_flags = 0;
  sigaction(SIGCHLD, &newact, NULL);
  
  memset(sTimeout, 0, sizeof(sTimeout));
	sprintf(sTimeout, "%s", ReadConItem("processTimeout"));
  timeout = atoi(sTimeout);
  signal(SIGALRM, dealTimeOutProcessFunc);
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	it.it_interval.tv_sec = timeout;
	it.it_interval.tv_usec = 0;
	
	pthread_mutexattr_init(&mutexattr);                                 
  pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED); //��PTHREAD_PROCESS_SHARED��ʾ���̼�Ļ�����
  pthread_mutex_init(&mutex, &mutexattr);   
    
	printf("�ȴ�����\n");
	while(1)
  {
  	/*
	  if ( timeout != TIME_INFINITE ) 
		{
		  timvl.tv_sec  = timeout/1000;
		  timvl.tv_usec = (timeout%1000)*1000;
		  ptimvl = &tv;
		}
		else 
		{
		  ptimvl = 0;
		}
		FD_ZERO(&fdset);
	  FD_SET(lBankSceuFd, &fdset);
	  
		sret = select(lBankSceuFd + 1, &fdset, 0, 0, ptimvl);
		switch (sret) 
	  {
	    case 0: 
	      return -1;
	    case -1:
	      return -1; 
	  }
	
	  if( FD_ISSET(lBankSceuFd,&fdset) )
	  {
	  	
	  }  
	  return -1;
	  */
		aBankSecuFd = AcceptRecvBankSecu(lBankSceuFd);
		memset(logBuf, 0, sizeof(logBuf));
		if(aBankSecuFd  < 0)
		{
			sprintf(logBuf, "%s", "�������л���ȯ������ʧ��");
			wRet = write(fifoFd, logBuf, strlen(logBuf));
			memset(logBuf, 0, sizeof(logBuf));
			if(wRet == -1)
			{
				sprintf(logBuf, "%s", "д�ܵ���������");
				WriteSySLog(4, logBuf);
				return WRITEFIFOERR;
			}
		}
		for(proNum = 0; proNum < 200; proNum++)
		{
	    pid = fork();
	    if(pid > 0)
	    {
	    	pthread_mutex_lock(&mutex);
		    shmid = shmget(0x2232, sizeof(ProcessId), 0666|IPC_CREAT); 
		    memset(logBuf, 0, sizeof(logBuf));
		    if (shmid == -1)
		    {
					sprintf(logBuf, "%s", "shmget����");
					WriteSySLog(3, logBuf);
					memset(logBuf, 0, sizeof(logBuf));
			  	if (shmid<0 && errno==ENOENT)
			  	{
			  		sprintf(logBuf, "%s", "���ͨ�������ڴ治����");
				  	WriteSySLog(3, logBuf);
			 	  }
			 	  memset(logBuf, 0, sizeof(logBuf));
			  	sprintf(logBuf, "%s-%d", "shmid����", shmid);
					WriteSySLog(3, logBuf);
					return errno;
			  }
			  memset(logBuf, 0, sizeof(logBuf));
				cPid = shmat(shmid, NULL, 0);
				if (cPid == (void *)-1 )
				{
					sprintf(logBuf, "%s-%d", "shmat����", shmid);
					WriteSySLog(3, logBuf);;
					return errno;
				}
				cPid->num = pidNum+1;
				strcpy(cPid->chilId, (char *)pid);
				shmdt(cPid);  //ȡ�����ӹ����ڴ�
				memset(logBuf, 0, sizeof(logBuf));
				ret = shmctl(shmid, IPC_RMID, NULL);
				if (ret < 0)
				{
				  sprintf(logBuf, "%s-%d", "shmid����", shmid);
					WriteSySLog(3, logBuf);
				}
	     pthread_mutex_unlock(&mutex);
	     exit(1);
	  	}
	    else if (pid == 0)
	    {
	    	Close(lBankSceuFd);
	    	pthread_mutex_lock(&mutex);
	    	if(setitimer(ITIMER_REAL, &it, &oldit) == -1)
	    	{
					perror("setitimer error");
					return -1;
				}
	      while(1) 
	      {
	      	memset(logBuf, 0, sizeof(logBuf));
	        rRet= read(aBankSecuFd, buf, MAXLINE);
				  if(rRet == -1)
				  {
				    sprintf(logBuf, "%s", "��ȡ���ж˷���������Ϣʧ��");
					  wRet = write(fifoFd, logBuf, strlen(logBuf));
					  memset(logBuf, 0, sizeof(logBuf));
					  if(wRet == -1)
					  {
						  sprintf(logBuf, "%s", "д�ܵ���������");
						  WriteSySLog(4, logBuf);
						  return WRITEFIFOERR;
					  }
				  }
				  memset(logBuf, 0, sizeof(logBuf));
				  sprintf(logBuf, "%s %s", "��ȡ���ж˷���������Ϣ�ɹ�", buf);
				  wRet = write(fifoFd, logBuf, strlen(logBuf));
					memset(logBuf, 0, sizeof(logBuf));
					if(wRet == -1)
					{
						sprintf(logBuf, "%s", "д�ܵ���������");
						WriteSySLog(4, logBuf);
						return WRITEFIFOERR;
					} 
	        for (i = 0; i < rRet; i++)
	        {
	         buf[i] = toupper(buf[i]);
	        }
	        write(STDOUT_FILENO, buf, rRet);
	        wRet = write(aBankSecuFd, buf, rRet);
				  if(wRet < 0)
				  {
				  	sprintf(logBuf, "%s", "���͸����ж˵ı��ķ�������");
						wRet = write(fifoFd, logBuf, strlen(logBuf));
						memset(logBuf, 0, sizeof(logBuf));
						if(wRet == -1)
						{
							sprintf(logBuf, "%s", "д�ܵ���������");
							WriteSySLog(4, logBuf);
							return WRITEFIFOERR;
						}
				  }             
				  memset(logBuf, 0, sizeof(logBuf));
					sprintf(logBuf, "%s %s", "���ͱ��ĸ����ж˳ɹ�", buf); 
					wRet = write(fifoFd, logBuf, strlen(logBuf));
					memset(logBuf, 0, sizeof(logBuf));
					if(wRet == -1)
					{
						sprintf(logBuf, "%s", "д�ܵ���������");
						WriteSySLog(4, logBuf);
						return WRITEFIFOERR;
					} 
	      }
	      Close(aBankSecuFd);
	      pthread_mutex_unlock(&mutex);
	    } 
	    else
	    {
	    	memset(logBuf, 0, sizeof(logBuf));
				sprintf(logBuf, "%s", "fork�ӽ��̳�����");
				WriteSySLog(4, logBuf);
	    }
	  }    
  }
  pthread_mutexattr_destroy(&mutexattr);          
  pthread_mutex_destroy(&mutex);  
  return 0;               
}

int main()
{
	pid_t pid;
	pid = fork();
	if(pid > 0)
	{
		ReadLogFromFifo();
	}
	else if(pid == 0)
	{
		sleep(1);
		startMain();
	}
	else
  {
  	printf("������");
  }
	return 0;
}
 
