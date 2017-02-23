#include "Sock.h"

int g_Sock;  //ȫ�ֱ�������

int InitListenSocket(char *addr,  unsigned short port)
{
	struct sockaddr_in sockAddr;
	char errLog[LOGLEVELLEN];
  int ret;
  g_Sock = socket(AF_INET, SOCK_STREAM, 0);
  memset(errLog, 0, sizeof(errLog));
  if (g_Sock  < 0)
	{
		sprintf(errLog, "%s", "����Socket������");
		WriteSySLog(5, errLog);
		return SOCKFAIL;
	}
  bzero(&sockAddr, sizeof(sockAddr));
  sockAddr.sin_family = AF_INET;
  sockAddr.sin_addr.s_addr = inet_addr(addr);
  sockAddr.sin_port = htons(port);
  
  ret = bind(g_Sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
  memset(errLog, 0, sizeof(errLog));
	if (ret  < 0)
	{
		sprintf(errLog, "%s", "��IP�Ͷ˿ںų�����");
		WriteSySLog(5, errLog);
		return BINDERR;
	}
  ret = listen(g_Sock, 200);
  memset(errLog, 0, sizeof(errLog));
	if (ret < 0)
	{
		sprintf(errLog, "%s", "���������Ŀ����");
		WriteSySLog(5, errLog);
		return LISTENERR;
	}
  return g_Sock; 
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;
	char errLog[LOGLEVELLEN];
again:
	if ((n = accept(fd, sa, salenptr)) < 0) {
		if ((errno == ECONNABORTED) || (errno == EINTR))
		{
			goto again;
		}			
		else
		{
			sprintf(errLog, "%s", "Accept������");
		  WriteSySLog(5, errLog);
		}		
	}
	return n;
}

int Connect(char *addr, unsigned short port)
{
	struct sockaddr_in  sockAddr;
	char errLog[LOGLEVELLEN];
  int cRet;
  
  memset(errLog, 0, sizeof(errLog));
  g_Sock = socket( AF_INET, SOCK_STREAM, 0 ); 
  if(g_Sock < 0)
  {
    sprintf(errLog, "%s", "����Socket������");
		WriteSySLog(5, errLog);
		return SOCKFAIL;
  }
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(addr);
	sockAddr.sin_port = htons( port ); 
	memset(errLog, 0, sizeof(errLog));
	cRet = connect( g_Sock,(struct sockaddr*)&sockAddr,sizeof(sockAddr));
	if(cRet < 0)
  {
    sprintf(errLog, "%s", "connect������");
		WriteSySLog(5, errLog);
 		return CONNECTERR;
  }
	return g_Sock;
}

int Close(int fd)
{
	int ret;
	char errLog[LOGLEVELLEN];
	memset(errLog, 0, sizeof(errLog));
	if ((ret = close(fd)) == -1)
	{
		sprintf(errLog, "%s", "�ر�Socket�������");
		WriteSySLog(5, errLog);	
	}
	return ret;
}
