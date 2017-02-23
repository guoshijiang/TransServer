/*=========================================================
 *文件名称:  MsgQue.h
 *创建日期：2017-2-15
 *修改记录：
 *  2017-2-15  首次创建
 *功能描述：消息队列公共函数
 ===========================================================*/
#ifndef __MSGQUE_H_
#define __MSGQUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define		MSGIPC_OK				        0		    
#define		MSGIPC_CreateErr			  5003		
#define		MSGIPC_RECVErr		      5004		
#define		MSGIPC_SENDErr			    5005		
#define   MSGIPC_IsCtl            5006  
#define   MSGIPC_FILE             5006   

/*=========================================================
 *函数入参:  pathname        文件名
             proj_id         根据自己的意愿随意设置(字节范围是int的范围)
 *函数出参:  无
 *返回值:    成功返回一个key_t类型的数,失败返回-1
 *功能说明:  判断一个文件是否存在
 *===========================================================*/
key_t IPC_FILE_exit(const char *pathname, int proj_id);

/*=========================================================
 *函数入参:  key             某个消息队列的名字
             msgflg          赋予消息队列权限
 *函数出参:  无
 *返回值:    成功返回一个大于0的数,失败返回错误码
 *功能说明:  用来创建和访问一个消息队列
 *===========================================================*/
int IPC_MSG_creat(key_t key, int msgflg);

/*=========================================================
 *函数入参:  msqid        由msgget函数返回的消息队列标识码
 *           cmd          cmd:是将要采取的动作,（有三个可取值）{IPC_STAT, IPC_SET, IPC_RMID}
 *           buf          结构体缓冲区
 *函数出参:  无
 *返回值:    成功返回0,失败返回错误码
 *功能说明:  消息队列的控制函数
 *===========================================================*/
int IPC_MSG_ctl(int msqid, int cmd, struct msqid_ds *buf);

/*=========================================================
 *函数入参:  msqid        由msgget函数返回的消息队列标识码
 *           msgp         是一个指针，指针指向准备发送的消息
 *           msgsz        是msgp指向的消息长度，这个长度不含保存消息类型的那个long int长整型
 *           msgflg       控制着当前消息队列满或到达系统上限时将要发生的事情;
 *                        msgflg=IPC_NOWAIT表示队列满不等待，返回EAGAIN错误
 *函数出参:  无
 *返回值:    成功返回0,失败返回错误码
 *功能说明:  把一条消息添加到消息队列中
 *===========================================================*/
int IPC_MSG_send(int msqid, const void *msgp, size_t msgsz, int msgflg);

/*=========================================================
 *函数入参:  msqid        由msgget函数返回的消息队列标识码
 *           msgp         是一个指针，指针指向准备发送的消息
 *           msgsz        是msgp指向的消息长度，这个长度不含保存消息类型的那个long int长整型
 *           msgtyp       可以实现接收优先级的简单形式
 *           msgflg       控制着队列中没有相应类型的消息可供接收时将要发生的事            
 *函数出参:  无
 *返回值:    成功返回实际放到接收缓冲区里去的字符个数，失败返回错误码
 *功能说明:  是从一个消息队列接收消息
 *===========================================================*/
 /*
 msgtype=0返回队列第一条信息
 msgtype>0返回队列第一条类型等于msgtype的消息　
 msgtype<0返回队列第一条类型小于等于msgtype绝对值的消息，并且是满足条件的消息类型最小的消息
 
 msgflg=IPC_NOWAIT，队列没有可读消息不等待，返回ENOMSG错误。
 msgflg=MSG_NOERROR，消息大小超过msgsz时被截断 
 msgtype>0且msgflg=MSG_EXCEPT，接收类型不等于msgtype的第一条消息。
 */
ssize_t IPC_MSG_recv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);


#endif