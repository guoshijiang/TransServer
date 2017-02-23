/*=========================================================
 *文件名称:  CmdParse.h
 *创建日期：2017-2-8
 *修改记录：
 *  2017-2-8  首次创建
 *功能描述：共享内存的公共函数封装
 ===========================================================*/
#ifndef __CMDPARSE_H_
#define __CMDPARSE_H_

#define CMDSTART   1   //start命令
#define CMDSTOP    2   //stop命令
#define CMDFORCE   3   //force命令
#define CMDSTATUS  4   //stutas命令

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Cfifo.h"
#include "WriteLog.h"

int CmdParse();

#endif