/*=========================================================
 *�ļ�����:  SocketInfo.h
 *�������ڣ�	2017-2-20
 *������	 ��	zyc
 *�޸ļ�¼��
 *  2017-2-20  created
===========================================================*/

#ifndef __SOCKETINFOLIST_H
#define __SOCKETINFOLIST_H

#include <iostream>
#include <time.h>
#include "CKdNodeData.h"
using namespace std;



class CKdNodeList
{
public:
	CKdNodeList();
	~CKdNodeList();

private:
	CKdNodeData*			m_lpHead;			/* ����ͷ��� */
	CKdNodeData*			m_lpEndNode;		/* ����β�ڵ� */
	int						m_iNodeNum;			/* �����нڵ����� */

public:


public:

	int AppendNode(CKdNodeData* lpsocketinfo);
	CKdNodeData* GetHeadNode();
	void PrintList();
	int CheckNode(int timeout);
	int ClearList();
	int GetListNodeNum();
	
};

#endif