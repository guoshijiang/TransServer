/*=========================================================
 *文件名称:  Cfifo.c
 *创建日期：2017-1-23
 *修改记录：
 *  2017-1-23  首次创建
 *功能描述：创建管道
 ===========================================================*/
#include "Cfifo.h"

int IPCMKFifo(const char *pathname, mode_t mode)
{
		int ret = mkfifo(pathname, mode);
		if(ret == -1)
		{
			ret = FIFOIPC_CreateErr;
			return ret;
		}
		return ret;
}

int IPCFifoOpen(const char *pathname, int flags)
{
	int fd;
	fd = open(pathname, flags);
	if(fd == -1)
	{
		fd = FIFOIPC_OpenErr;
		return fd;
	}
	return fd;
}

int IPCFifoClose(int fd)
{
	int ret = 0;
	ret = close(fd);
	if(ret == -1)
	{
		ret = FIFOIPC_CloseErr;
		return ret;
	}
	return ret;
}
