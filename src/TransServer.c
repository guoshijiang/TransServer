/*=========================================================
 *文件名称:  TransServer.h
 *创建日期：2017-2-17
 *修改记录：
 *  2017-2-17 改进第二个版本中消息队列，改为socket发消息
 *功能描述：
 ===========================================================*/
#include "TransServer.h"

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

int startMain()
{
	int                  fifo_fd;    					 //写日志管道句柄
	char                 logBuf[LOGLEVELLEN];  //写日志缓冲区
	char                 buf[1024];  
	int                  lBank_fd; 				     //监听银行端F5返回来的句柄        
	int                  aBank_fd;             //AcceptRecvBankSecu返回的值
	int                  proMax_num;           //进程的最大进程个数
	int                  pro_timeout;          //进程超时时间
	int                  sockUtil_timeout;     //socket实体超时时间
	char                 f5Bank_addr[100];     //监听的银行端的IP地址
	char                 f5Bank_port[10];      //监听的银行端的端口号
	int                  sRet; 
	char                 timeout[10];          //中间变量      
	pid_t                pid;                  //进程id
	int                  i;
	char                 pro_buf[10];          //进程最大数量的中间衔接变量
	pthread_t            tpid;
	struct sigaction     newact;
	struct itimerval     it, oldit;          //使用定时器处理进程超时时间使用的结构体
	char                 sTimeout[20];
	ProcessId            *cPid = NULL;       //进程相关的结构体
	int                  ret = 0;
	int 	               shmid;
	int                  pidNum = 0;
	ssize_t              flret;              //写管道的write函数的返回值
	int                  bussRet;            //业务处理入口函数返回值
	
	//打开写日志的管道
	fifo_fd  = IPCFifoOpen("../ipc/myfifo", O_WRONLY);
	memset(logBuf, 0x00, sizeof(logBuf));
	if(fifo_fd == -1)
	{
	  sprintf(logBuf, "%s\n", "打开管道失败");
 	  WriteSySLog(3, logBuf);
    return FIFOIPC_OpenErr;
	}
	memset(f5Bank_addr, 0, sizeof(f5Bank_addr));
	sprintf(f5Bank_addr, "%s", ReadConItem("bankIP"));
	memset(f5Bank_port, 0, sizeof(f5Bank_port));
	sprintf(f5Bank_port, "%s", ReadConItem("bankPort"));
  int bankPort = atoi(f5Bank_port);
	memset(logBuf, 0x00, sizeof(logBuf));               
	
	sRet = sckServer_init(f5Bank_addr, bankPort, &lBank_fd); //初始化监听银行端
	if (sRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckServer_init() err", sRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//回收正常退出的子进程回调函数DealSigchild
	newact.sa_handler = DealSigchild;
  sigemptyset(&newact.sa_mask);
  newact.sa_flags = 0;
  sigaction(SIGCHLD, &newact, NULL);
  
  //从配置文件中获取进程超时时间
  memset(sTimeout, 0, sizeof(sTimeout));
	sprintf(sTimeout, "%s", ReadConItem("processTimeout"));
  int sig_timeout = atoi(sTimeout);
  
  //使用SIGALRM信号处理超时间的进程
  signal(SIGALRM, dealTimeOutProcessFunc);
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	it.it_interval.tv_sec = sig_timeout;
	it.it_interval.tv_usec = 0;
	
	//从配置文件获取sock实体超时时间
	sprintf(timeout, "%s", ReadConItem("otherTimeout"));
	sockUtil_timeout = atoi(timeout);
	while (1)
	{
		memset(logBuf, 0x00, sizeof(logBuf));
		sRet = sckServer_accept(lBank_fd, sockUtil_timeout, &aBank_fd);
		if (sRet ==Sck_ErrTimeOut)
		{
			continue;
		}
		else if (sRet != 0)
		{
			sRet = 2;
			sprintf(logBuf, "%s-%d\n", "fun sckServer_accept() err", sRet);
			WriteSySLog(4, logBuf);
			return -1;
		}
		//从配置文件中获取最大进程数量
		memset(pro_buf, 0, sizeof(pro_buf));
		sprintf(pro_buf, "%s", ReadConItem("maxProcess"));
	  proMax_num = atoi(pro_buf);
		for(i = 0; i < proMax_num; i++)
		{
			pid = fork();
			if(pid > 0)
			{
				
				/*
				//通过共享内存把子进程的pid推到共享内存
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
				}*/
				exit(0);
			}
			else if(pid == 0)
			{
				memset(logBuf, 0, sizeof(logBuf));
				if(setitimer(ITIMER_REAL, &it, &oldit) == -1)
				{
					sprintf(logBuf, "%s\n", "setitimer error");
					WriteSySLog(4, logBuf);
					return -1;
				}
				
				memset(logBuf, 0x00, sizeof(logBuf));
				sprintf(logBuf, "%s\n", "开始处理业务");
				flret = write(fifo_fd, logBuf, strlen(logBuf));
				if(flret < 0)
				{
					memset(logBuf, 0x00, sizeof(logBuf));
				  sprintf(logBuf, "%s\n", "写管道失败");	
				  WriteSySLog(3, logBuf);
				}
				//关闭管道IPCFifoClose
				IPCFifoClose(fifo_fd);
				memset(logBuf, 0x00, sizeof(logBuf));
				bussRet = startService(aBank_fd, sockUtil_timeout);     //业务处理入口函数
				if(bussRet != 0)
				{
					sprintf(logBuf, "%s\n", "startService处理业务入口函数调用失败");
					WriteSySLog(3, logBuf);
					return -1;
				}
			}
			else
			{
				memset(logBuf, 0x00, sizeof(logBuf));
				sprintf(logBuf, "%s\n", "fork err");
				WriteSySLog(4, logBuf);
			}
		}
	}
	//服务器端环境释放 
	sckServer_destroy();
	return 0;
}

int startService(int bankfd, int timeout)
{
	int                       fifo_fd;
	int 		                  sRet;
	unsigned char 	          *inbuf = NULL;
	int 			                inlen;
	char                      logBuf[LOGLEVELLEN];  //写日志缓冲区
	unsigned char 	          *outbuf = NULL;
	int 			                outlen = 0;
	int                       rsRet;
	void                      *handle = NULL;
	int                       rs_newoldsecu_fd;
	int                       flret;
	char                      *rshBuf;
	int                       shbRet;
	
	
	//打开写日志的管道
	fifo_fd  = IPCFifoOpen("../ipc/myfifo", O_WRONLY);
	memset(logBuf, 0x00, sizeof(logBuf));
	if(fifo_fd == -1)
	{
	  sprintf(logBuf, "%s\n", "打开管道失败");
 	  WriteSySLog(3, logBuf);
    return FIFOIPC_OpenErr;
	}
	
	while (1)
	{	
		memset(logBuf, 0, sizeof(logBuf));
		sRet =  sckServer_rev(bankfd, timeout, &inbuf, &inlen); //接收银行端发过来的报文
		if (sRet == Sck_ErrPeerClosed)
		{
			sprintf(logBuf, "%s\n", "检测银行F5过来有一条连接已关闭");
			WriteSySLog(4, logBuf);
			break;
		}
		else if (sRet == Sck_ErrTimeOut)
		{
			continue;
		}
		else if (sRet != 0)
		{
			sprintf(logBuf, "%s\n", "sckServer_send()错误");
			WriteSySLog(4, logBuf);
			break;
		}
		memset(logBuf, 0, sizeof(logBuf));
		sprintf(logBuf, "%s:%s\n", "接收银行端发过来的报文", inbuf);
		flret = write(fifo_fd, logBuf, strlen(logBuf));    //往管道里写业务日志
		if(flret < 0)
		{
			memset(logBuf, 0x00, sizeof(logBuf));
		  sprintf(logBuf, "%s\n", "写管道失败");	
		  WriteSySLog(3, logBuf);
		}
		rshBuf =  Readfile_StructArray_HandelBuf("../etc/sc_config.ini", inbuf);
		if(rshBuf == NULL)
		{
			printf("出错了");
			return -1;	
		}
		
		shbRet = seg_Handle_Buf(rshBuf);
		if(shbRet == 0)
		{
			//发往老三方做处理
			memset(logBuf, 0, sizeof(logBuf));
			rsRet = recv_Send_OldSecu(handle, rs_newoldsecu_fd, inbuf, inlen, &outbuf, &outlen);
			if(rsRet < 0)
			{
				sprintf(logBuf, "%s\n", "recv_Send_OldSecu()错误");
				WriteSySLog(4, logBuf);
				break;
			}	
			
		}
		else if(shbRet == -1)
		{
			//发往新三方做处理	
			memset(logBuf, 0, sizeof(logBuf));
			rsRet = recv_Send_NewSecu(handle, rs_newoldsecu_fd, inbuf, inlen, &outbuf, &outlen);
			if(rsRet < 0)
			{
				sprintf(logBuf, "%s\n", "recv_Send_NewSecu()错误");
				WriteSySLog(4, logBuf);
				break;
			}	
		}
		/*
		memset(logBuf, 0, sizeof(logBuf));
		rsRet = recv_Send_NewSecu(handle, rs_newsecu_fd, inbuf, inlen, &outbuf, &outlen);
		if(rsRet < 0)
		{
			sprintf(logBuf, "%s\n", "recv_Send_NewSecu()错误");
			WriteSySLog(4, logBuf);
			break;
		}
		*/
		//往管道中写业务日志
		memset(logBuf, 0, sizeof(logBuf));
		sprintf(logBuf, "%s:%s\n", "接收到老三方或者新三方发过来的报文并将其发给F5银行端", outbuf);
		flret = write(fifo_fd, logBuf, strlen(logBuf));
		if(flret < 0)
		{
			memset(logBuf, 0x00, sizeof(logBuf));
		  sprintf(logBuf, "%s\n", "写管道失败");	
		  WriteSySLog(3, logBuf);
		}
	
		
		memset(logBuf, 0, sizeof(logBuf));
		sRet =  sckServer_send(bankfd, timeout, outbuf, outlen);
		if (sRet == Sck_ErrPeerClosed)
		{
			sck_FreeMem((void **)&outbuf);
			sprintf(logBuf, "%s\n", "检测到F5端有一条连接已关闭\n");
			WriteSySLog(4, logBuf);
			break;
		}
		else if (sRet == Sck_ErrTimeOut)
		{
			sck_FreeMem((void **)&outbuf);
			continue;
		}
		else if (sRet != 0)
		{
			sck_FreeMem((void **)&outbuf);
			sprintf(logBuf, "%s\n", "sckServer_send() err\n");
			WriteSySLog(4, logBuf);
			break;
		}
		sck_FreeMem((void **)&outbuf);
		sckCltPool_putConnet(handle, rs_newoldsecu_fd, 1); 
	}
	//关闭管道IPCFifoClose
	IPCFifoClose(fifo_fd);
	rsRet = sckCltPool_destroy(handle);
	sckServer_close(bankfd);
	return 0;
}

void DealSigchild(int num)
{
  while(waitpid(0, NULL, WNOHANG) > 0);
}

void dealTimeOutProcessFunc(int signo)
{
	//在这里处理定时器超时的进程
}

int recv_Send_NewSecu(void *handle, int connfd, char* inbuf, int inlen, unsigned char **outbuf, int *outlen)
{
	int 	                   rsRet;
	
	char                     newSecu_adrr[30];
	char                     newSecu_Mport[10];
	int                      newSecu_port;
	int                      sockUtil_timeout;         //Socket超时时间定义
	char                     timeout[10];              //超时时间中间转接
	char                     logBuf[LOGLEVELLEN];      //写日志缓冲区
	char                     *retBuf = NULL;
	
	SCKClitPoolParam 	       param;                    //初始化连接池
	
	//获取新三方监听的IP和端口号
	memset(newSecu_adrr, 0, sizeof(newSecu_adrr));
	sprintf(newSecu_adrr, "%s", ReadConItem("newSecuIp"));
	memset(newSecu_Mport, 0, sizeof(newSecu_Mport));
	sprintf(newSecu_Mport, "%s", ReadConItem("newSecuPort"));
  newSecu_port = atoi(newSecu_Mport);
  
  //获取Socket实体超时时间
  sprintf(timeout, "%s", ReadConItem("otherTimeout"));
	sockUtil_timeout = atoi(timeout);
	
	//给SocketClient连接池赋值
	strcpy(param.serverip, newSecu_adrr);          //要连接的新三方的IP地址
	param.serverport = newSecu_port;               //要连接的新三方的端口号
	param.bounds = 100;                            //Socket连接池的容量
	param.connecttime = sockUtil_timeout;          //连接新三方超时时间
	param.sendtime = sockUtil_timeout;             //发送报文超时时间
	param.revtime = sockUtil_timeout;	             //接收报文超时时间
	
	//socket连接池初始化
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_init(&handle, &param);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_init() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket池获取一条连接 
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_getConnet(handle, &connfd);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_getConnet() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket池发送数据 
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_send(handle, connfd,  inbuf, inlen);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_send() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket池接受数据
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_rev(handle, connfd, outbuf, outlen); 
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_rev() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	return 0;
}


int recv_Send_OldSecu(void *handle, int connfd, char* inbuf, int inlen, unsigned char **outbuf, int *outlen)
{
	int 	                   rsRet;
	
	char                     newSecu_adrr[30];
	char                     newSecu_Mport[10];
	int                      newSecu_port;
	int                      sockUtil_timeout;         //Socket超时时间定义
	char                     timeout[10];              //超时时间中间转接
	char                     logBuf[LOGLEVELLEN];      //写日志缓冲区
	char                     *retBuf = NULL;
	
	SCKClitPoolParam 	       param;        //初始化连接池
	
	//获取新三方监听的IP和端口号
	memset(newSecu_adrr, 0, sizeof(newSecu_adrr));
	sprintf(newSecu_adrr, "%s", ReadConItem("oldSecuIp"));
	memset(newSecu_Mport, 0, sizeof(newSecu_Mport));
	sprintf(newSecu_Mport, "%s", ReadConItem("oldSecuPort"));
  newSecu_port = atoi(newSecu_Mport);
  
  //获取Socket实体超时时间
  sprintf(timeout, "%s", ReadConItem("otherTimeout"));
	sockUtil_timeout = atoi(timeout);
	
	//给SocketClient连接池赋值
	strcpy(param.serverip, newSecu_adrr);          //要连接的老三方的IP地址
	param.serverport = newSecu_port;               //要连接的老三方的端口号
	param.bounds = 100;                            //Socket连接池的容量
	param.connecttime = sockUtil_timeout;          //连接老三方超时时间
	param.sendtime = sockUtil_timeout;             //发送报文超时时间
	param.revtime = sockUtil_timeout;	             //接收报文超时时间
	
	//socket连接池初始化
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_init(&handle, &param);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_init() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket池获取一条连接 
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_getConnet(handle, &connfd);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_getConnet() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket池发送数据 
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_send(handle, connfd,  inbuf, inlen);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_send() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket池接受数据
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_rev(handle, connfd, outbuf, outlen); 
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_rev() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	return 0;
}