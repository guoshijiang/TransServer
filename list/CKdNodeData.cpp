#include "CKdNodeData.h"
#include <string.h>

CKdNodeData::CKdNodeData(void)
{
	m_lpData = NULL;
	m_iDataLen = 0;
}

CKdNodeData::CKdNodeData(const CKdNodeData& objNode)
{
	m_iSocket  = objNode.m_iSocket;
	m_tmPutTime= objNode.m_tmPutTime;
	m_iDataLen = objNode.m_iDataLen;
	m_lpData   = new char[m_iDataLen];
	memcpy(m_lpData,objNode.m_lpData,m_iDataLen);
	m_lpNext   = objNode.m_lpNext;
}

CKdNodeData::~CKdNodeData(void)
{
	if (m_lpData != NULL)
	{
		delete []m_lpData;
		m_lpData = NULL;
	}

}


int CKdNodeData::GetSocket()
{
	return this->m_iSocket;
}

time_t CKdNodeData::GetPutTime()
{
	return this->m_tmPutTime;
}

char* CKdNodeData::GetData()
{
	return this->m_lpData;
}

int CKdNodeData::GetDataLen()
{
	return this->m_iDataLen;
}

void CKdNodeData::SetSocket(int fd)
{
	this->m_iSocket = fd;
}

void CKdNodeData::SetPutTime(time_t tm)
{
	this->m_tmPutTime = tm;
}

int CKdNodeData::SetData(char* lpData , int ilen)
{
	if (lpData == NULL)
	{
		return -1;
	}

	this->m_lpData = new char[ilen];
	memcpy(this->m_lpData ,lpData, ilen);
	this->m_iDataLen = ilen;
	return 0;
}