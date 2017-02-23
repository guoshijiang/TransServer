/*=========================================================
 *文件名称:  				Sock.h
 *创建日期：					2017-1-22
 *修改记录：					2017-1-22  首次创建
 *功能描述           本地套接字公共函数封装 
 ===========================================================*/
#ifndef __SOCK_H_
#define __SOCK_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include "WriteLog.h"

/*=========================================================
 *错误码定义
 *===========================================================*/
#define TIME_INFINITE -1
#define SOCKFAIL   900
#define BINDERR    901
#define LISTENERR  902
#define CONNECTERR 903

int InitListenSocket(char *socketAddr,  unsigned short port);

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

int Connect(char *addr, unsigned short port);

int Close(int fd);

#endif
