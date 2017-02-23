/*=========================================================
 *文件名称:  SockLog.h
 *创建日期：2017-2-9
 *修改记录：
 *  2017-2-9  首次创建
 *功能描述：SockLog
 ===========================================================*/
#ifndef _SOCKETLOG_H_
#define _SOCKETLOG_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*=========================================================
 *函数入参:   
 *函数出参:
 *返回值:
 *功能说明:
 *===========================================================*/
extern int SocketLevel[5];

/*=========================================================
 *函数入参:   
 *函数出参:
 *返回值:
 *功能说明:
 *===========================================================*/
void Socket_Log(const char *file, int line, int level, int status, const char *fmt, ...);

#endif
