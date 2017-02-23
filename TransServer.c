/*=========================================================
 *�ļ�����:  TransServer.h
 *�������ڣ�2017-2-17
 *�޸ļ�¼��
 *  2017-2-17 �Ľ��ڶ����汾����Ϣ���У���Ϊsocket����Ϣ
 *����������
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
  	printf("������");
  }
	return 0;
}

int startMain()
{
	int                  fifo_fd;    					 //д��־�ܵ����
	char                 logBuf[LOGLEVELLEN];  //д��־������
	char                 buf[1024];  
	int                  lBank_fd; 				     //�������ж�F5�������ľ��        
	int                  aBank_fd;             //AcceptRecvBankSecu���ص�ֵ
	int                  proMax_num;           //���̵������̸���
	int                  pro_timeout;          //���̳�ʱʱ��
	int                  sockUtil_timeout;     //socketʵ�峬ʱʱ��
	char                 f5Bank_addr[100];     //���������ж˵�IP��ַ
	char                 f5Bank_port[10];      //���������ж˵Ķ˿ں�
	int                  sRet; 
	char                 timeout[10];          //�м����      
	pid_t                pid;                  //����id
	int                  i;
	char                 pro_buf[10];          //��������������м��νӱ���
	pthread_t            tpid;
	struct sigaction     newact;
	struct itimerval     it, oldit;          //ʹ�ö�ʱ��������̳�ʱʱ��ʹ�õĽṹ��
	char                 sTimeout[20];
	ProcessId            *cPid = NULL;       //������صĽṹ��
	int                  ret = 0;
	int 	               shmid;
	int                  pidNum = 0;
	ssize_t              flret;              //д�ܵ���write�����ķ���ֵ
	int                  bussRet;            //ҵ������ں�������ֵ
	
	//��д��־�Ĺܵ�
	fifo_fd  = IPCFifoOpen("../ipc/myfifo", O_WRONLY);
	memset(logBuf, 0x00, sizeof(logBuf));
	if(fifo_fd == -1)
	{
	  sprintf(logBuf, "%s\n", "�򿪹ܵ�ʧ��");
 	  WriteSySLog(3, logBuf);
    return FIFOIPC_OpenErr;
	}
	memset(f5Bank_addr, 0, sizeof(f5Bank_addr));
	sprintf(f5Bank_addr, "%s", ReadConItem("bankIP"));
	memset(f5Bank_port, 0, sizeof(f5Bank_port));
	sprintf(f5Bank_port, "%s", ReadConItem("bankPort"));
  int bankPort = atoi(f5Bank_port);
	memset(logBuf, 0x00, sizeof(logBuf));               
	
	sRet = sckServer_init(f5Bank_addr, bankPort, &lBank_fd); //��ʼ���������ж�
	if (sRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckServer_init() err", sRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//���������˳����ӽ��̻ص�����DealSigchild
	newact.sa_handler = DealSigchild;
  sigemptyset(&newact.sa_mask);
  newact.sa_flags = 0;
  sigaction(SIGCHLD, &newact, NULL);
  
  //�������ļ��л�ȡ���̳�ʱʱ��
  memset(sTimeout, 0, sizeof(sTimeout));
	sprintf(sTimeout, "%s", ReadConItem("processTimeout"));
  int sig_timeout = atoi(sTimeout);
  
  //ʹ��SIGALRM�źŴ���ʱ��Ľ���
  signal(SIGALRM, dealTimeOutProcessFunc);
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	it.it_interval.tv_sec = sig_timeout;
	it.it_interval.tv_usec = 0;
	
	//�������ļ���ȡsockʵ�峬ʱʱ��
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
		//�������ļ��л�ȡ����������
		memset(pro_buf, 0, sizeof(pro_buf));
		sprintf(pro_buf, "%s", ReadConItem("maxProcess"));
	  proMax_num = atoi(pro_buf);
		for(i = 0; i < proMax_num; i++)
		{
			pid = fork();
			if(pid > 0)
			{
				
				/*
				//ͨ�������ڴ���ӽ��̵�pid�Ƶ������ڴ�
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
				sprintf(logBuf, "%s\n", "��ʼ����ҵ��");
				flret = write(fifo_fd, logBuf, strlen(logBuf));
				if(flret < 0)
				{
					memset(logBuf, 0x00, sizeof(logBuf));
				  sprintf(logBuf, "%s\n", "д�ܵ�ʧ��");	
				  WriteSySLog(3, logBuf);
				}
				//�رչܵ�IPCFifoClose
				IPCFifoClose(fifo_fd);
				memset(logBuf, 0x00, sizeof(logBuf));
				bussRet = startService(aBank_fd, sockUtil_timeout);     //ҵ������ں���
				if(bussRet != 0)
				{
					sprintf(logBuf, "%s\n", "startService����ҵ����ں�������ʧ��");
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
	//�������˻����ͷ� 
	sckServer_destroy();
	return 0;
}

int startService(int bankfd, int timeout)
{
	int                       fifo_fd;
	int 		                  sRet;
	unsigned char 	          *inbuf = NULL;
	int 			                inlen;
	char                      logBuf[LOGLEVELLEN];  //д��־������
	unsigned char 	          *outbuf = NULL;
	int 			                outlen = 0;
	int                       rsRet;
	void                      *handle = NULL;
	int                       rs_newoldsecu_fd;
	int                       flret;
	char                      *rshBuf;
	int                       shbRet;
	
	
	//��д��־�Ĺܵ�
	fifo_fd  = IPCFifoOpen("../ipc/myfifo", O_WRONLY);
	memset(logBuf, 0x00, sizeof(logBuf));
	if(fifo_fd == -1)
	{
	  sprintf(logBuf, "%s\n", "�򿪹ܵ�ʧ��");
 	  WriteSySLog(3, logBuf);
    return FIFOIPC_OpenErr;
	}
	
	while (1)
	{	
		memset(logBuf, 0, sizeof(logBuf));
		sRet =  sckServer_rev(bankfd, timeout, &inbuf, &inlen); //�������ж˷������ı���
		if (sRet == Sck_ErrPeerClosed)
		{
			sprintf(logBuf, "%s\n", "�������F5������һ�������ѹر�");
			WriteSySLog(4, logBuf);
			break;
		}
		else if (sRet == Sck_ErrTimeOut)
		{
			continue;
		}
		else if (sRet != 0)
		{
			sprintf(logBuf, "%s\n", "sckServer_send()����");
			WriteSySLog(4, logBuf);
			break;
		}
		memset(logBuf, 0, sizeof(logBuf));
		sprintf(logBuf, "%s:%s\n", "�������ж˷������ı���", inbuf);
		flret = write(fifo_fd, logBuf, strlen(logBuf));    //���ܵ���дҵ����־
		if(flret < 0)
		{
			memset(logBuf, 0x00, sizeof(logBuf));
		  sprintf(logBuf, "%s\n", "д�ܵ�ʧ��");	
		  WriteSySLog(3, logBuf);
		}
		rshBuf =  Readfile_StructArray_HandelBuf("../etc/sc_config.ini", inbuf);
		if(rshBuf == NULL)
		{
			printf("������");
			return -1;	
		}
		
		shbRet = seg_Handle_Buf(rshBuf);
		if(shbRet == 0)
		{
			//����������������
			memset(logBuf, 0, sizeof(logBuf));
			rsRet = recv_Send_OldSecu(handle, rs_newoldsecu_fd, inbuf, inlen, &outbuf, &outlen);
			if(rsRet < 0)
			{
				sprintf(logBuf, "%s\n", "recv_Send_OldSecu()����");
				WriteSySLog(4, logBuf);
				break;
			}	
			
		}
		else if(shbRet == -1)
		{
			//����������������	
			memset(logBuf, 0, sizeof(logBuf));
			rsRet = recv_Send_NewSecu(handle, rs_newoldsecu_fd, inbuf, inlen, &outbuf, &outlen);
			if(rsRet < 0)
			{
				sprintf(logBuf, "%s\n", "recv_Send_NewSecu()����");
				WriteSySLog(4, logBuf);
				break;
			}	
		}
		/*
		memset(logBuf, 0, sizeof(logBuf));
		rsRet = recv_Send_NewSecu(handle, rs_newsecu_fd, inbuf, inlen, &outbuf, &outlen);
		if(rsRet < 0)
		{
			sprintf(logBuf, "%s\n", "recv_Send_NewSecu()����");
			WriteSySLog(4, logBuf);
			break;
		}
		*/
		//���ܵ���дҵ����־
		memset(logBuf, 0, sizeof(logBuf));
		sprintf(logBuf, "%s:%s\n", "���յ������������������������ı��Ĳ����䷢��F5���ж�", outbuf);
		flret = write(fifo_fd, logBuf, strlen(logBuf));
		if(flret < 0)
		{
			memset(logBuf, 0x00, sizeof(logBuf));
		  sprintf(logBuf, "%s\n", "д�ܵ�ʧ��");	
		  WriteSySLog(3, logBuf);
		}
	
		
		memset(logBuf, 0, sizeof(logBuf));
		sRet =  sckServer_send(bankfd, timeout, outbuf, outlen);
		if (sRet == Sck_ErrPeerClosed)
		{
			sck_FreeMem((void **)&outbuf);
			sprintf(logBuf, "%s\n", "��⵽F5����һ�������ѹر�\n");
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
	//�رչܵ�IPCFifoClose
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
	//�����ﴦ��ʱ����ʱ�Ľ���
}

int recv_Send_NewSecu(void *handle, int connfd, char* inbuf, int inlen, unsigned char **outbuf, int *outlen)
{
	int 	                   rsRet;
	
	char                     newSecu_adrr[30];
	char                     newSecu_Mport[10];
	int                      newSecu_port;
	int                      sockUtil_timeout;         //Socket��ʱʱ�䶨��
	char                     timeout[10];              //��ʱʱ���м�ת��
	char                     logBuf[LOGLEVELLEN];      //д��־������
	char                     *retBuf = NULL;
	
	SCKClitPoolParam 	       param;                    //��ʼ�����ӳ�
	
	//��ȡ������������IP�Ͷ˿ں�
	memset(newSecu_adrr, 0, sizeof(newSecu_adrr));
	sprintf(newSecu_adrr, "%s", ReadConItem("newSecuIp"));
	memset(newSecu_Mport, 0, sizeof(newSecu_Mport));
	sprintf(newSecu_Mport, "%s", ReadConItem("newSecuPort"));
  newSecu_port = atoi(newSecu_Mport);
  
  //��ȡSocketʵ�峬ʱʱ��
  sprintf(timeout, "%s", ReadConItem("otherTimeout"));
	sockUtil_timeout = atoi(timeout);
	
	//��SocketClient���ӳظ�ֵ
	strcpy(param.serverip, newSecu_adrr);          //Ҫ���ӵ���������IP��ַ
	param.serverport = newSecu_port;               //Ҫ���ӵ��������Ķ˿ں�
	param.bounds = 100;                            //Socket���ӳص�����
	param.connecttime = sockUtil_timeout;          //������������ʱʱ��
	param.sendtime = sockUtil_timeout;             //���ͱ��ĳ�ʱʱ��
	param.revtime = sockUtil_timeout;	             //���ձ��ĳ�ʱʱ��
	
	//socket���ӳس�ʼ��
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_init(&handle, &param);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_init() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket�ػ�ȡһ������ 
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_getConnet(handle, &connfd);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_getConnet() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket�ط������� 
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_send(handle, connfd,  inbuf, inlen);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_send() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket�ؽ�������
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
	int                      sockUtil_timeout;         //Socket��ʱʱ�䶨��
	char                     timeout[10];              //��ʱʱ���м�ת��
	char                     logBuf[LOGLEVELLEN];      //д��־������
	char                     *retBuf = NULL;
	
	SCKClitPoolParam 	       param;        //��ʼ�����ӳ�
	
	//��ȡ������������IP�Ͷ˿ں�
	memset(newSecu_adrr, 0, sizeof(newSecu_adrr));
	sprintf(newSecu_adrr, "%s", ReadConItem("oldSecuIp"));
	memset(newSecu_Mport, 0, sizeof(newSecu_Mport));
	sprintf(newSecu_Mport, "%s", ReadConItem("oldSecuPort"));
  newSecu_port = atoi(newSecu_Mport);
  
  //��ȡSocketʵ�峬ʱʱ��
  sprintf(timeout, "%s", ReadConItem("otherTimeout"));
	sockUtil_timeout = atoi(timeout);
	
	//��SocketClient���ӳظ�ֵ
	strcpy(param.serverip, newSecu_adrr);          //Ҫ���ӵ���������IP��ַ
	param.serverport = newSecu_port;               //Ҫ���ӵ��������Ķ˿ں�
	param.bounds = 100;                            //Socket���ӳص�����
	param.connecttime = sockUtil_timeout;          //������������ʱʱ��
	param.sendtime = sockUtil_timeout;             //���ͱ��ĳ�ʱʱ��
	param.revtime = sockUtil_timeout;	             //���ձ��ĳ�ʱʱ��
	
	//socket���ӳس�ʼ��
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_init(&handle, &param);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_init() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket�ػ�ȡһ������ 
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_getConnet(handle, &connfd);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_getConnet() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket�ط������� 
	memset(logBuf, 0, sizeof(logBuf));
	rsRet = sckCltPool_send(handle, connfd,  inbuf, inlen);
	if (rsRet != 0)
	{
		sprintf(logBuf, "%s-%d\n", "func sckCltPool_send() err:", rsRet);
		WriteSySLog(4, logBuf);
		return -1;
	}
	
	//socket�ؽ�������
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