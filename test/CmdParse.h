/*=========================================================
 *�ļ�����:  CmdParse.h
 *�������ڣ�2017-2-8
 *�޸ļ�¼��
 *  2017-2-8  �״δ���
 *���������������ڴ�Ĺ���������װ
 ===========================================================*/
#ifndef __CMDPARSE_H_
#define __CMDPARSE_H_

#define CMDSTART   1   //start����
#define CMDSTOP    2   //stop����
#define CMDFORCE   3   //force����
#define CMDSTATUS  4   //stutas����

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Cfifo.h"
#include "WriteLog.h"

int CmdParse();

#endif