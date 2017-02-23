/*=========================================================
 *文件名称:  Deamon.h
 *创建日期：2017-1-24
 *修改记录：
 *  2017-1-24  首次创建
 *功能描述：写日志
 ===========================================================*/
#ifndef __DEAMON_H_
#define __DEAMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "WriteLog.h"

#define MAXFILE 65535

//volatile sig_atomic_t _running = 1;
/*=========================================================
 *函数入参:无   
 *函数出参:无
 *返回值:
 *功能说明:
 *===========================================================*/
//void SigtermHandler(int arg);

/*=========================================================
 *函数入参:无   
 *函数出参:无
 *返回值:
 *功能说明:
 *===========================================================*/
void Daemon();

#endif