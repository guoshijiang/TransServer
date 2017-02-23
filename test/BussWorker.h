/*=========================================================
 *文件名称:  MultiBuss.h
 *创建日期：2017-1-23
 *修改记录：
 *  2017-1-23  首次创建
 *功能描述：该文件中重封装四个函数
 *							ListenBankSecu     监听银行或者券商端
 *							AcceptRecvBankSecu 接受银行或者券商端的连接请求
 *							RecvBank           接收银行端发过来的包
 *							SendBank           发送银行端发过来的包
 *							RecvNewSecu        接收新三方发送过来的包
 *							SendNewSecu        发送新三方发送过来的包
 *							RecvOldSecu        接收老三方发送过来的包
 *							SendOldSecu        发送老三方发送过来的包
 ===========================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>
#include "Sock.h"
#include "Config.h"
#include "WriteLog.h"

/*=========================================================
 *错误码定义
 ===========================================================*/
#define RECVBANKSUCC                200       //接收银行端报文成功
#define SENDBANKSUCC                201       //发送银行端报文成功
#define RECVNEWSECUSUCC             202       //接收新三方报文成功
#define SENDNEWSECUSUCC             203       //发送新三方报文成功
#define RECVOLDSECUSUCC             204       //接收老三方报文成功
#define SENDOLDSECUSUCC             205       //发送老三方报文成功

#define RECVBANKTIMEOUT             300       //接收银行端报文超时
#define SENDBANKTIMEOUT             301       //发送银行端报文超时
#define RECVNEWSECUTIMEOUT          302       //接收新三方报文超时
#define SENDNEWSECUTIMEOUT          303       //发送新三方报文超时
#define RECVOLDSECUTIMEOUT          304       //接收老三方报文超时
#define SENDOLDSECUTIMEOUT          305       //发送老三方报文超时

#define RECVBANKERR                 400       //接收银行端报文错误
#define SENDBANKERR                 401       //发送银行端报文错误
#define RECVNEWSECUERR              402       //接收新三方报文错误
#define SENDNEWSECUERR              403       //发送新三方报文错误
#define RECVOLDSECUERR              404       //接收老三方报文错误
#define SENDOLDSECUERR              405       //发送老三方报文错误
 
#define SOCKETSUCC                  206       //监听SOCKET成功    
#define SOCKETTIMEOUT               306       //监听SOCKET超时
#define SOCKETERR                   406       //监听SOCKET错误
#define LISTENBANKSECUERR           407       //监听银行出错
#define WRITEFIFOERR                408       //写管道出错

/*=========================================================
 *宏定义
 ===========================================================*/
#define RECVLEM 4096
#define SENDLEM 4096
#define MAXLINE 8192

/*=========================================================
 *函数入参: iIp监听的IP, 监听的端口号       
 *函数出参: 无
 *返回值:   成功返回监听的句柄，失败返回错误号
 *功能说明: 监听银行或者券商端的IP和端口号
 *===========================================================*/
int ListenBankSecu(char *iIp, char *iPort);

/*=========================================================
 *函数入参: 无       
 *函数出参: 无
 *返回值:   成功返回接受的句柄，失败返回错误号
 *功能说明: 接受银行或者券商端发送的连接
 *===========================================================*/
int AcceptRecvBankSecu();

/*=========================================================
 *函数入参: 无       
 *函数出参: 无
 *返回值:   
 *功能说明: 接收银行端发送的报文
 *===========================================================*/
int RecvBank(void *buf);

/*=========================================================
 *函数入参: 无       
 *函数出参: 无
 *返回值:   
 *功能说明: 发送报文给银行
 *===========================================================*/
int SendBank(void *buf, int len);

/*=========================================================
 *函数入参: 无       
 *函数出参: 无
 *返回值:   
 *功能说明: 接收老三方报文
 *===========================================================*/
int RecvNewSecu(void *buf);

/*=========================================================
 *函数入参: 无       
 *函数出参: 无
 *返回值:   
 *功能说明: 发送老三方报文
 *===========================================================*/
int SendNewSecu(void *buf, int len);

/*=========================================================
 *函数入参: 无       
 *函数出参: 无
 *返回值:   
 *功能说明: 接收新三方报文
 *===========================================================*/
int RecvOldSecu(void *buf);

/*=========================================================
 *函数入参: 无       
 *函数出参: 无
 *返回值:   
 *功能说明: 发送新三方报文
 *===========================================================*/
int SendOldSecu(void *buf, int len);