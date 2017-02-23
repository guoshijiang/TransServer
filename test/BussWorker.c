/*=========================================================
 *文件名称:  MultiBuss.h
 *创建日期：2017-1-23
 *修改记录：
 *  2017-1-23  首次创建
 *功能描述：该文件中以下函数
 *							ListenBankSecu     监听银行或者券商端
 *							AcceptRecvBankSecu 接受银行或者券商端的连接请求
 *							RecvBank           接收银行端发过来的包
 *							SendBank           发送银行端发过来的包
 *							RecvNewSecu        接收新三方发送过来的包
 *							SendNewSecu        发送新三方发送过来的包
 *							RecvOldSecu        接收老三方发送过来的包
 *							SendOldSecu        发送老三方发送过来的包
 ===========================================================*/
#include "BussWorker.h"
 
int ListenBankSecu(char *iIp, char *iPort)
{
	int               recvBankSecuFd;
	char 							bankAddrIp[20];
	char              bankPort[10];
	int               socketPort;
	char              errLog[LOGLEVELLEN];
	memset(errLog, 0, sizeof(errLog));
	if(iIp == NULL && iPort == NULL)
	{
		sprintf(errLog, "%s", "iIP和iPort为空");
		WriteSySLog(2, errLog);
		return LISTENBANKSECUERR;
	}
	memset(bankAddrIp, 0, sizeof(bankAddrIp));
	sprintf(bankAddrIp, "%s", ReadConItem(iIp));
	memset(bankPort, 0, sizeof(bankPort));
	sprintf(bankPort, "%s", ReadConItem(iPort));
  socketPort = atoi(bankPort);
	recvBankSecuFd = InitListenSocket(bankAddrIp, socketPort);
	memset(errLog, 0, sizeof(errLog));
	if(recvBankSecuFd == 900 || recvBankSecuFd == 901 || recvBankSecuFd == 902 )
	{
		sprintf(errLog, "%s", "CInitListenSocket Fail\n");
		WriteSySLog(5, errLog);
		return LISTENBANKSECUERR;
	}
	return recvBankSecuFd;
}

int AcceptRecvBankSecu(int recvBankSecuFd)
{
	int  connfd;
	char errLog[LOGLEVELLEN];
	struct sockaddr_in cliaddr;
	socklen_t cliaddr_len;
	cliaddr_len = sizeof(cliaddr);
	connfd = Accept(recvBankSecuFd, (struct sockaddr *)&cliaddr, &cliaddr_len);
	memset(errLog, 0, sizeof(errLog));
	if(connfd < 0)
	{
		sprintf(errLog, "%s", "AcceptRecvBankSecu Fail\n");
		WriteSySLog(5, errLog);
		return LISTENBANKSECUERR;
	}
	return connfd;
}

int RecvBank(void *buf)
{
	int                  fifoFd;    //管道句柄
	char                 logBuf[LOGLEVELLEN];  
	int                  lBankSceuFd;  //监听银行端返回来的句柄
	ssize_t              wet;
	ssize_t              ret;          
	int                  aBankSecuFd;   //AcceptRecvBankSecu返回的值
	
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
		sprintf(logBuf, "%s", "监听银行或者券商失败");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "写管道发生错误");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
	printf("等待适配\n");
	aBankSecuFd = AcceptRecvBankSecu(lBankSceuFd);
	memset(logBuf, 0, sizeof(logBuf));
	if(aBankSecuFd  < 0)
	{
		sprintf(logBuf, "%s", "接受银行或者券商连接失败");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "写管道发生错误");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
  ret= read(aBankSecuFd, buf, MAXLINE);
  memset(logBuf, 0, sizeof(logBuf));
  if(ret == -1)
  {
  	sprintf(logBuf, "%s", "获取银行端发过来的消息失败");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "写管道发生错误");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
  }
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "获取银行端发过来的消息成功", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(logBuf, 0, sizeof(logBuf));
	if(wet == -1)
	{
		sprintf(logBuf, "%s", "写管道发生错误");
		WriteSySLog(4, logBuf);
		return WRITEFIFOERR;
	} 
	return ret; 
}

int SendBank(void *buf, int len)
{
	char 							bankAddrIp[20];
	char              bankPort[10];
	char              errLog[LOGLEVELLEN];
	char              logBuf[LOGLEVELLEN];
	int               sRetfd;
	int               fifoFd;
	int               wRet, wet;
	char              buftest[1024]; 
	
	memset(bankAddrIp, 0, sizeof(bankAddrIp));
	sprintf(bankAddrIp, "%s", ReadConItem("bankIP"));
	memset(bankPort, 0, sizeof(bankPort));
	sprintf(bankPort, "%s", ReadConItem("bankPort"));
	int port = atoi(bankPort);
	
	memset(errLog, 0, sizeof(errLog));
	sRetfd = Connect(bankAddrIp, port);
	if(sRetfd < 0)
	{
		sprintf(logBuf, "%s", "连接银行端失败");
		WriteSySLog(4, errLog);
	  return CONNECTERR;
	} 
	
	memset(logBuf, 0, sizeof(logBuf));
	
  fgets(buftest, sizeof(buftest), stdin);
  wRet = write(sRetfd, buf, len);
  if(wRet < 0)
  {
  	sprintf(logBuf, "%s", "发送给银行端的报文发生错误");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(errLog, 0, sizeof(errLog));
		if(wet == -1)
		{
			sprintf(errLog, "%s", "写管道发生错误");
			WriteSySLog(4, errLog);
			return WRITEFIFOERR;
		}
  }             
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "发送报文给银行端成功", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(errLog, 0, sizeof(errLog));
	if(wet == -1)
	{
		sprintf(errLog, "%s", "写管道发生错误");
		WriteSySLog(4, errLog);
		return WRITEFIFOERR;
	} 
	return wRet;
}

int RecvNewSecu(void *buf)
{
	int                  fifoFd;    //管道句柄
	char                 logBuf[LOGLEVELLEN];  
	int                  lBankSceuFd;  //监听新券商端返回来的句柄
	ssize_t              wet;
	ssize_t              ret;          
	int                  aBankSecuFd;   //AcceptRecvBankSecu返回的值
	
	fifoFd  = IPCFifoOpen("../ipc/myfifo", O_WRONLY);
	memset(logBuf, 0x00, sizeof(logBuf));
	if(fifoFd == -1)
	{
	 sprintf(logBuf, "%s", "打开管道失败");
 	 WriteSySLog(3, logBuf);
   return FIFOIPC_OpenErr;
	}
	
	lBankSceuFd = ListenBankSecu("newSecuIp", "newSecuPort");
	memset(logBuf, 0, sizeof(logBuf));
	if(lBankSceuFd == LISTENBANKSECUERR)
	{
		sprintf(logBuf, "%s", "监听银行或者券商失败");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "写管道发生错误");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
	printf("等待适配\n");
	aBankSecuFd = AcceptRecvBankSecu(lBankSceuFd);
	memset(logBuf, 0, sizeof(logBuf));
	if(aBankSecuFd  < 0)
	{
		sprintf(logBuf, "%s", "接受银行或者券商连接失败");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "写管道发生错误");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
  ret= read(aBankSecuFd, buf, MAXLINE);
  memset(logBuf, 0, sizeof(logBuf));
  if(ret == -1)
  {
  	sprintf(logBuf, "%s", "获取新券商端发过来的消息失败");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "写管道发生错误");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
  }
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "获取新券商端发过来的消息成功", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(logBuf, 0, sizeof(logBuf));
	if(wet == -1)
	{
		sprintf(logBuf, "%s", "写管道发生错误");
		WriteSySLog(4, logBuf);
		return WRITEFIFOERR;
	} 
	return ret; 
}

int SendNewSecu(void *buf, int len)
{
	char 							bankAddrIp[20];
	char              bankPort[10];
	char              errLog[LOGLEVELLEN];
	char              logBuf[LOGLEVELLEN];
	int               fifoFd;
	int               sRetfd;
	int               wRet, wet;
	
	memset(bankAddrIp, 0, sizeof(bankAddrIp));
	sprintf(bankAddrIp, "%s", ReadConItem("newSecuIp"));
	memset(bankPort, 0, sizeof(bankPort));
	sprintf(bankPort, "%s", ReadConItem("newSecuPort"));
	int port = atoi(bankPort);
	
	memset(errLog, 0, sizeof(errLog));
	sRetfd = Connect(bankAddrIp, port);
	if(sRetfd < 0)
	{
		sprintf(logBuf, "%s", "连接新券商端失败");
		WriteSySLog(4, errLog);
	  return CONNECTERR;
	} 
	
	memset(logBuf, 0, sizeof(logBuf));
  wRet = write(sRetfd, buf, len);
  if(wRet < 0)
  {
  	sprintf(logBuf, "%s", "发送给新券商端的报文发生错误");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(errLog, 0, sizeof(errLog));
		if(wet == -1)
		{
			sprintf(errLog, "%s", "写管道发生错误");
			WriteSySLog(4, errLog);
			return WRITEFIFOERR;
		}
  }             
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "发送报文给新券商端成功", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(errLog, 0, sizeof(errLog));
	if(wet == -1)
	{
		sprintf(errLog, "%s", "写管道发生错误");
		WriteSySLog(4, errLog);
		return WRITEFIFOERR;
	} 
	return wRet;
}

int RecvOldSecu(void *buf)
{
	int                  fifoFd;    //管道句柄
	char                 logBuf[LOGLEVELLEN];  
	int                  lBankSceuFd;  //监听老券商端返回来的句柄
	ssize_t              wet;
	ssize_t              ret;          
	int                  aBankSecuFd;   //AcceptRecvBankSecu返回的值
	
	fifoFd  = IPCFifoOpen("../ipc/myfifo", O_WRONLY);
	memset(logBuf, 0x00, sizeof(logBuf));
	if(fifoFd == -1)
	{
	 sprintf(logBuf, "%s", "打开管道失败");
 	 WriteSySLog(3, logBuf);
   return FIFOIPC_OpenErr;
	}
	
	lBankSceuFd = ListenBankSecu("oldSecuIp", "oldSecuPort");
	memset(logBuf, 0, sizeof(logBuf));
	if(lBankSceuFd == LISTENBANKSECUERR)
	{
		sprintf(logBuf, "%s", "监听银行或者券商失败");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "写管道发生错误");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
	printf("等待适配\n");
	aBankSecuFd = AcceptRecvBankSecu(lBankSceuFd);
	memset(logBuf, 0, sizeof(logBuf));
	if(aBankSecuFd  < 0)
	{
		sprintf(logBuf, "%s", "接受银行或者券商连接失败");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "写管道发生错误");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
  ret= read(aBankSecuFd, buf, MAXLINE);
  memset(logBuf, 0, sizeof(logBuf));
  if(ret == -1)
  {
  	sprintf(logBuf, "%s", "获取老券商端发过来的消息失败");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "写管道发生错误");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
  }
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "获取老券商端发过来的消息成功", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(logBuf, 0, sizeof(logBuf));
	if(wet == -1)
	{
		sprintf(logBuf, "%s", "写管道发生错误");
		WriteSySLog(4, logBuf);
		return WRITEFIFOERR;
	} 
	return ret; 
}

int SendOldSecu(void *buf, int len)
{
	char 							bankAddrIp[20];
	char              bankPort[10];
	char              errLog[LOGLEVELLEN];
	char              logBuf[LOGLEVELLEN];
	int               sRetfd;
	int               fifoFd;
	int               wRet, wet;
	
	memset(bankAddrIp, 0, sizeof(bankAddrIp));
	sprintf(bankAddrIp, "%s", ReadConItem("oldSecuIp"));
	memset(bankPort, 0, sizeof(bankPort));
	sprintf(bankPort, "%s", ReadConItem("oldSecuPort"));
	int port = atoi(bankPort);
	
	memset(errLog, 0, sizeof(errLog));
	sRetfd = Connect(bankAddrIp, port);
	if(sRetfd < 0)
	{
		sprintf(logBuf, "%s", "连接银行端失败");
		WriteSySLog(4, errLog);
	  return CONNECTERR;
	} 
	
	memset(logBuf, 0, sizeof(logBuf));
  wRet = write(sRetfd, buf, len);
  if(wRet < 0)
  {
  	sprintf(logBuf, "%s", "发送给老券商端的报文发生错误");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(errLog, 0, sizeof(errLog));
		if(wet == -1)
		{
			sprintf(errLog, "%s", "写管道发生错误");
			WriteSySLog(4, errLog);
			return WRITEFIFOERR;
		}
  }             
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "发送报文给老券商端成功", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(errLog, 0, sizeof(errLog));
	if(wet == -1)
	{
		sprintf(errLog, "%s", "写管道发生错误");
		WriteSySLog(4, errLog);
		return WRITEFIFOERR;
	} 
	return wRet;
}