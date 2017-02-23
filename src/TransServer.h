/*=========================================================
 *�ļ�����:  TransServer.h
 *�������ڣ�2017-1-24
 *�޸ļ�¼��
 *  2017-2-17  �Ľ��ڶ����汾����Ϣ���У���Ϊsocket����Ϣ
 *����������
 ===========================================================*/
#ifndef __TRANSSERVER_H_
#define __TRANSSERVER_H_

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


#endif

