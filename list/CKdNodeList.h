/*=========================================================
 *文件名称:  SocketInfo.h
 *创建日期：	2017-2-20
 *创建者	 ：	zyc
 *修改记录：
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
	CKdNodeData*			m_lpHead;			/* 链表头结点 */
	CKdNodeData*			m_lpEndNode;		/* 链表尾节点 */
	int						m_iNodeNum;			/* 链表中节点数量 */

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