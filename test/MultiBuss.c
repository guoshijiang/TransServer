/*=========================================================
 *文件名称：MultiBuss.c
 *创建日期：2017-1-23
 *修改记录：
 *  2017-1-23  首次创建
 *功能描述：多进程处理业务函数
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
	printf("hello world\n"); //在这里处理定时器超时的进程
}

void DealSigchild(int num)
{
  while(waitpid(0, NULL, WNOHANG) > 0); //在这里处理正常处理业务完成退出的进程
}

int startMain()
{
	int                  fifoFd;    					 //管道句柄
	char                 logBuf[LOGLEVELLEN];
	char                 buf[MAXLINE];  
	int                  lBankSceuFd; 				 //监听银行端返回来的句柄
	ssize_t              wRet;
	ssize_t              rRet;          
	int                  aBankSecuFd;          //AcceptRecvBankSecu返回的值
	int                  proNum;               //进程的最大进程个数
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
	 sprintf(logBuf, "%s", "打开管道失败");
 	 WriteSySLog(3, logBuf);
   return FIFOIPC_OpenErr;
	}
	lBankSceuFd = ListenBankSecu("bankIP", "bankPort");
	memset(logBuf, 0, sizeof(logBuf));
	if(lBankSceuFd == LISTENBANKSECUERR)
	{
		sprintf(logBuf, "%s", "监听银行端F5发过来的消息失败");
		wRet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wRet == -1)
		{
			sprintf(logBuf, "%s", "写管道发生错误");
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
  pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED); //传PTHREAD_PROCESS_SHARED表示进程间的互斥锁
  pthread_mutex_init(&mutex, &mutexattr);   
    
	printf("等待适配\n");
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
			sprintf(logBuf, "%s", "接受银行或者券商连接失败");
			wRet = write(fifoFd, logBuf, strlen(logBuf));
			memset(logBuf, 0, sizeof(logBuf));
			if(wRet == -1)
			{
				sprintf(logBuf, "%s", "写管道发生错误");
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
					sprintf(logBuf, "%s", "shmget出错");
					WriteSySLog(3, logBuf);
					memset(logBuf, 0, sizeof(logBuf));
			  	if (shmid<0 && errno==ENOENT)
			  	{
			  		sprintf(logBuf, "%s", "检测通过共享内存不存在");
				  	WriteSySLog(3, logBuf);
			 	  }
			 	  memset(logBuf, 0, sizeof(logBuf));
			  	sprintf(logBuf, "%s-%d", "shmid错误", shmid);
					WriteSySLog(3, logBuf);
					return errno;
			  }
			  memset(logBuf, 0, sizeof(logBuf));
				cPid = shmat(shmid, NULL, 0);
				if (cPid == (void *)-1 )
				{
					sprintf(logBuf, "%s-%d", "shmat错误", shmid);
					WriteSySLog(3, logBuf);;
					return errno;
				}
				cPid->num = pidNum+1;
				strcpy(cPid->chilId, (char *)pid);
				shmdt(cPid);  //取消连接共享内存
				memset(logBuf, 0, sizeof(logBuf));
				ret = shmctl(shmid, IPC_RMID, NULL);
				if (ret < 0)
				{
				  sprintf(logBuf, "%s-%d", "shmid错误", shmid);
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
				    sprintf(logBuf, "%s", "获取银行端发过来的消息失败");
					  wRet = write(fifoFd, logBuf, strlen(logBuf));
					  memset(logBuf, 0, sizeof(logBuf));
					  if(wRet == -1)
					  {
						  sprintf(logBuf, "%s", "写管道发生错误");
						  WriteSySLog(4, logBuf);
						  return WRITEFIFOERR;
					  }
				  }
				  memset(logBuf, 0, sizeof(logBuf));
				  sprintf(logBuf, "%s %s", "获取银行端发过来的消息成功", buf);
				  wRet = write(fifoFd, logBuf, strlen(logBuf));
					memset(logBuf, 0, sizeof(logBuf));
					if(wRet == -1)
					{
						sprintf(logBuf, "%s", "写管道发生错误");
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
				  	sprintf(logBuf, "%s", "发送给银行端的报文发生错误");
						wRet = write(fifoFd, logBuf, strlen(logBuf));
						memset(logBuf, 0, sizeof(logBuf));
						if(wRet == -1)
						{
							sprintf(logBuf, "%s", "写管道发生错误");
							WriteSySLog(4, logBuf);
							return WRITEFIFOERR;
						}
				  }             
				  memset(logBuf, 0, sizeof(logBuf));
					sprintf(logBuf, "%s %s", "发送报文给银行端成功", buf); 
					wRet = write(fifoFd, logBuf, strlen(logBuf));
					memset(logBuf, 0, sizeof(logBuf));
					if(wRet == -1)
					{
						sprintf(logBuf, "%s", "写管道发生错误");
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
				sprintf(logBuf, "%s", "fork子进程出错了");
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
  	printf("出错啦");
  }
	return 0;
}
 
