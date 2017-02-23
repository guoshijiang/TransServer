/*=========================================================
 *文件名称:  MsgQue.c
 *创建日期：2017-2-15
 *修改记录：
 *  2017-2-15  首次创建
 *功能描述：消息队列公共函数
 ===========================================================*/
#include "MsgQue.h"

key_t IPC_FILE_exit(const char *pathname, int proj_id)
{
	key_t ipcRet;
	ipcRet =  ftok(pathname, proj_id);
	if(ipcRet < 0)
	{
	  return MSGIPC_FILE; 	
	}
	return ipcRet;
}

int IPC_MSG_creat(key_t key, int msgflg)
{
 	int ipcRet;
 	ipcRet = msgget(key, msgflg);
 	if(ipcRet < 0)
 	{
 		return 	MSGIPC_CreateErr;
 	}
 	return ipcRet;
}

int IPC_MSG_ctl(int msqid, int cmd, struct msqid_ds *buf)
{
	int ipcRet;
	ipcRet = msgctl(msqid, cmd, buf);
	if(ipcRet < 0)
	{
		return MSGIPC_IsCtl; 	
	}
	return ipcRet;
}

int IPC_MSG_send(int msqid, const void *msgp, size_t msgsz, int msgflg)
{
	int ipcRet;
	ipcRet = msgsnd(msqid, msgp, msgsz, msgflg);
	if(ipcRet < 0)
	{
		return MSGIPC_SENDErr; 	
	}
	return ipcRet;
}

ssize_t IPC_MSG_recv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
{	
	ssize_t ipcRet;
	ipcRet = msgrcv(msqid, msgp, msgsz, msgtyp, msgflg);
	if(ipcRet < 0)
	{
		return MSGIPC_RECVErr; 	
	}
	return ipcRet;
}

