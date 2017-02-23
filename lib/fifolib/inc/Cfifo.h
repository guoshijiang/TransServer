/*=========================================================
 *文件名称:  Cfifo.h
 *创建日期：2017-1-23
 *修改记录：
 *  2017-1-23  首次创建
 *功能描述：管道公共函数封装
 ===========================================================*/
#ifndef __CFIFO_H_
#define __CFIFO_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define		FIFOIPC_OK				       0		  //正确
#define		FIFOIPC_CreateErr			  1003		//创建管道失败
#define		FIFOIPC_OpenErr		      1004		//打开管道失败
#define		FIFOIPC_CloseErr			  1005		//关闭管道失败
#define   FIFOIPC_IsEixst         1006    //创建的管道不存在
/*=========================================================
 *函数入参:  pathname        有名管道的所在目录的路径
             mode            赋予管道的权限
 *函数出参:  无
 *返回值:    成功返回0,失败返回错误码
 *功能说明:  创建管道
 *===========================================================*/
int IPCMKFifo(const char *pathname, mode_t mode);

/*=========================================================
 *函数入参:  pathname        有名管道的所在目录的路径
 *          flags           O_RDONLY,O_WRONLY或者O_RDWR等
 *函数出参:  无
 *返回值:    成功返回fd文件描述符,失败返回错误码
 *功能说明:  打开管道
 *===========================================================*/
int IPCFifoOpen(const char *pathname, int flags);

/*=========================================================
 *函数入参:  fd        要关闭的文件描述符
 *函数出参:  无
 *返回值:    成功返回0，失败返回错误码
 *功能说明:  关闭IPC管道
 *===========================================================*/
int IPCFifoClose(int fd);

#endif

