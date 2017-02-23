/*=========================================================
 *�ļ�����:  SocketInfo.h
 *�������ڣ�2017-2-20
 *�޸ļ�¼��
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
  int		m_iSocket;			/* Socket ���  */
  time_t	m_tmPutTime;		/* �������ʱ��  */
  char*     m_lpData;			/* �ַ���       */
  int       m_iDataLen;			/* �ַ�������   */
 
public:
  CKdNodeData*     m_lpNext;	/* ��ǰ�ڵ���һ���ڵ� */ 

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