#include <time.h>
#include "CKdNodeList.h"

CKdNodeList::CKdNodeList(void)
{
	m_lpHead	 = NULL;
	m_lpEndNode	 = NULL;
	m_iNodeNum   = 0;
}


CKdNodeList::~CKdNodeList(void)
{

}


int CKdNodeList::AppendNode(CKdNodeData* lpkdnodedata)
{
	if (lpkdnodedata == NULL)
	{
		return -1;
	}
	time_t tm;

	time(&tm);
	lpkdnodedata->SetPutTime(tm);
	lpkdnodedata->m_lpNext =NULL;
	if (m_iNodeNum == 0)
	{
		m_lpHead    = lpkdnodedata;
		m_lpEndNode = lpkdnodedata;
	}
	else
	{
		m_lpEndNode->m_lpNext = lpkdnodedata;
		m_lpEndNode = lpkdnodedata;

	}
	m_iNodeNum += 1;

	return 0;
}

CKdNodeData* CKdNodeList::GetHeadNode()
{	


	if (m_iNodeNum <= 0)
	{
		return NULL;
	}
	CKdNodeData* temNode = m_lpHead;
	m_lpHead = m_lpHead->m_lpNext;
	m_iNodeNum -= 1;

	return temNode;
}

void CKdNodeList::PrintList()
{

	CKdNodeData* lpTemNode = m_lpHead;
	for (int i=0;i< m_iNodeNum;i++)
	{
		cout<<"CKdNodeData Socket = "<<lpTemNode->GetSocket()<<endl;
		cout<<"            PutTime= "<<lpTemNode->GetPutTime()<<endl;
		cout<<"            szData = "<<lpTemNode->GetData()<<endl;
		cout<<"            DataLen= "<<lpTemNode->GetDataLen()<<endl;
		lpTemNode = lpTemNode->m_lpNext;
	}

}

int CKdNodeList::CheckNode(int timeout)
{
	if(m_iNodeNum == 0)
	{
		return 0;
	}
	CKdNodeData* lptemList = m_lpHead;
	CKdNodeData* temNode   = NULL;
	time_t tm;

	time(&tm);


	for(lptemList;lptemList->m_lpNext != NULL;)
	{
		time(&tm);
		if (lptemList == m_lpHead && (tm - lptemList->GetPutTime()) >= timeout)
		{	
			temNode = m_lpHead;
			m_lpHead = m_lpHead->m_lpNext;
			lptemList	 = m_lpHead;
			m_iNodeNum -= 1;
			delete temNode;
		}
		else if ((tm - lptemList->m_lpNext->GetPutTime()) >= timeout)
		{
			temNode = lptemList->m_lpNext;
			lptemList->m_lpNext = temNode->m_lpNext;
			m_iNodeNum -= 1;
			delete temNode;
		}
		else
		{
			lptemList = lptemList->m_lpNext;
		}
	}
	
	if (m_iNodeNum == 1&& (tm-lptemList->GetPutTime()) >= timeout)
	{
		delete lptemList;
		m_lpHead	= NULL;
		m_lpEndNode = NULL;
	}
	return true;

}

int CKdNodeList::ClearList()
{
	if (0 == m_iNodeNum)
	{
		return true;
	}
	CKdNodeData* lpTemNode = NULL;
	while (m_lpHead!=NULL)
	{
		lpTemNode = m_lpHead;
		m_lpHead  = m_lpHead->m_lpNext;
		m_iNodeNum -= 1;
		if (m_iNodeNum == 0)
		{
			m_lpEndNode = NULL;
		}
		
		delete lpTemNode;

	}
	return 0;
}

int  CKdNodeList::GetListNodeNum()
{

	int NodeNum = m_iNodeNum;

	return NodeNum;
}



