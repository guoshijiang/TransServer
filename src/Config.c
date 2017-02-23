/*=========================================================
 *文件名称: 			Config.c
 *创建日期：			2017-1-23
 *修改记录：			2017-1-23  首次创建
 *功能描述：     读配置文件公共函数
 *===========================================================*/  
#include"Config.h"  

static char *substr(char *linebuf,char *pKey)  
{  
	char *pTmp = NULL, *pRv = NULL;  
	int lenKey = 0,len = 0;
	char logErr[LOGLEVELLEN];
	memset(logErr, 0, sizeof(logErr));  
	if (linebuf==NULL || pKey==NULL)  
	{
		sprintf(logErr, "%s", "参数为空出错(str==NULL || substr==NULL)");  
		WriteSySLog(5, logErr);  
		return pRv;  
	}  
	pRv = strstr(linebuf, pKey);  
	if(pRv == NULL)  
	{  
		return pRv;  
	}  
	pTmp = pRv;  
	lenKey = strlen(pKey);  
	while(*pTmp != '\0' && *pTmp != ' '&& *pTmp != '=' && *pTmp != '\n')  
	{  
		len++;  
		pTmp++;  
		if(len>lenKey)  
		{  
			break;  
		}  
	}  
	if (lenKey != len)  
	{  
		return NULL;  
	}  

	return pRv;  
}  
 
int GetCfItem(const char *pFileName, char *pKey, char * pValue, int * pValueLen )  
{  
	int rv = 0; 
	char logErr[LOGLEVELLEN]; 
	FILE *fp = NULL;  
	char linebuf[LineMaxLen];  
	char *pTmp = NULL, *pBegin = NULL, *pEnd = NULL;  
	memset(logErr, 0, sizeof(logErr)); 
	if (pFileName==NULL||pKey==NULL||pValue==NULL||pValueLen==NULL)  
	{  
		rv = -1;
		sprintf(logErr, "%s", "获取参数失败!");  
		WriteSySLog(5, logErr);  
		goto End;  
	}  
	fp = fopen(pFileName,"r"); 
	memset(logErr, 0, sizeof(logErr));
	if (fp==NULL)  
	{  
		rv = -2; 
		sprintf(logErr, "%s %d", "打开文件失败", rv);  
		WriteSySLog(5, logErr);  
		goto End;  
	}  
	while(!feof(fp))  
	{  
		memset(linebuf,0,LineMaxLen);  
		pTmp = fgets(linebuf, LineMaxLen, fp);
		if (pTmp==NULL)  
		{  
			break;  
		}  
		pTmp = substr(linebuf, pKey);
		if (pTmp==NULL)  
		{  
			continue;  
		} 
		pTmp = strchr(linebuf, '='); 
		if (pTmp==NULL)  
		{  
			continue;  
		}  
		pTmp=pTmp+1;

		while (1) 
		{  
			if(*pTmp==' ')  
			{  
				pTmp++;  
			}  
			else  
			{  
				pBegin = pTmp; 
				memset(logErr, 0, sizeof(logErr)); 
				if(*pBegin == '\n'||*pBegin=='\0')  
				{  
					rv = -3; 
					sprintf(logErr, "%s %s", "对应的键没有这个值", pKey);  
					WriteSySLog(5, logErr);    
				}  
				break;  
			}  
		}  
		while (1) 
		{  
			if(*pTmp==' '||*pTmp=='\n'||*pTmp=='\0')  
			{  
				break;  
			}  
			else  
			{  
				pTmp++;  
			}  
		}  
		pEnd = pTmp;  
		*pValueLen = pEnd-pBegin;  
		memcpy(pValue,pBegin,*pValueLen); 
		*(pValue+*pValueLen)='\0';  
		break;  
	} 
	memset(logErr, 0, sizeof(logErr)); 
	if(pBegin==NULL)   
	{  
		sprintf(logErr, "%s %s", "不能找到键:", pKey);  
		WriteSySLog(5, logErr);    
		rv = -4;  
	}  
End:  
	if (fp!=NULL)  
	{  
		fclose(fp);  
	}  
	return rv;  
} 

char *ReadConItem(char *key)
{
	char fname[128];
	memset(fname, 0, sizeof(fname));
	sprintf(fname, "%s/trans/etc/config.ini",getenv("HOME"));
	char *pFileName= fname;
  int len = 0, ret = 0; 
  char logErr[LOGLEVELLEN];
  memset(value, 0, sizeof(value));   
  ret = GetCfItem(pFileName,key,value,&len); 
  memset(logErr, 0, sizeof(logErr)); 
  if(ret!=0)  
  {  
  	sprintf(logErr, "%s", "获取键值错误");  
		WriteSySLog(5, logErr);
    return NULL;  
  }
  return value;
} 