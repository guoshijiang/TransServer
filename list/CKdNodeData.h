/*=========================================================
 *文件名称:  SocketInfo.h
 *创建日期：2017-2-20
 *修改记录：
 *  2017-2-20  created
===========================================================*/

#ifndef __SOCKETINFO_H
#define __SOCKETINFO_H

#include <iostream>
#include <time.h>

class CKdNodeData
{
public:
	CKdNodeData();
	CKdNodeData(const CKdNodeData&);
	~CKdNodeData();
private:
  int		m_iSocket;			/* Socket 句柄  */
  time_t	m_tmPutTime;		/* 句柄放入时间  */
  char*     m_lpData;			/* 字符串       */
  int       m_iDataLen;			/* 字符串长度   */
 
public:
  CKdNodeData*     m_lpNext;	/* 当前节点下一个节点 */ 

public:
	int		GetSocket();
	time_t	GetPutTime();
	char*	GetData();
	int		GetDataLen();
	
	void    SetSocket(int fd);
	void	SetPutTime(time_t tm);
	int	    SetData(char* szData ,int ilen);
};

#endif