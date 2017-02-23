/*=========================================================
 *文件名称:  HandleBuss.h
 *创建日期：2017-1-15
 *修改记录：
 *  2017-1-15  
 *功能描述：处理业务
 ===========================================================*/
#include "Cfifo.h"
#include "WriteLog.h"
#include "HandleBuss.h"

//去空格函数
char *str_trim(char *str)
{
	if(str)
	{
		char c;
		char chstr[2048];
		c = str[0];
		while(c == ' ' || c == '\t' || c == '\n')
		{
			str = str + 1;
			c = str[0];
			if(c == '\0')
			{
				return NULL;	
			}	
		}	
		c = str[strlen(str) -1 ];
		while(c == ' ' || c == '\t' || c == '\n')
		{
			memset(chstr, 0, sizeof(chstr));
			memcpy(chstr, str, strlen(str) - 1);	
			strcpy(str, chstr);
			c = str[strlen(str) - 1];
		}
	}
	return str;
}

//截取一个字符串中从49位开长度为7的字符串的字符串，供测试用的函数
char* strstr_49_7(char buf[2048])
{
	strncpy(str_str, buf+48, 7);
	return str_str;
}

//截取一个字符串中从576位开长度为7的字符串的字符串，供测试用的函数
char* strstr_576_7(char buf[2048])
{
	strncpy(str_str, buf+575, 7);
	return str_str;
}

//截取从某位置开始指定长度子字符串
char* strstr_pos_length(char* srcstr, int pos, int length)
{  
  if(srcstr == NULL || pos < 0 || length < 0)
  {
 	  printf("srcstr is NULL, pos and length less than 0\n");	
 	  return NULL;
	}  
  int tLen = strlen(srcstr);
  int rLen = ((tLen - pos) >= length ? length : (tLen - pos)) + 1; 
  char *tmpSpace = (char*) malloc(rLen * sizeof(char));  
  if (tmpSpace== NULL)
  {  
     printf("tmpSpace is NULL\n");  
     return NULL;  
  }  
  strncpy(tmpSpace, srcstr+pos, rLen - 1); 
  tmpSpace[rLen - 1] = '\0';  
  return tmpSpace;  
}

//把文件中的内容扫描到一个数组中
int Readfile_Array(const char *filename, char *dest, int maxlen) 
{ 
  FILE              *fp; 
  int               pos = 0;
  int               temp;
  int               i; 
  char              logBuf[LOGLEVELLEN];             
	memset(logBuf, 0, sizeof(logBuf));
  fp = fopen(filename, "r+"); 
  if(fp == NULL) 
  { 
	  sprintf(logBuf, "%s-%s", "open err", filename);
	  WriteSySLog(4, logBuf);
	  return -1; 
  }   
	for(i=0; i<MAXLEN-1; i++) 
	{ 
		temp = fgetc(fp); 
		if(EOF == temp)
		{
			break; 
		} 
		dest[pos++] = temp; 
	} 
	if (fp != NULL)
  {
		fclose(fp);
		fp = NULL;
	}
	dest[pos] = 0; 
	return pos; 
}

//把文件中的内容扫描到一个结构体数组中并其处理字符串
char* Readfile_StructArray_HandelBuf(char *filename, char* buf)
{
  scCodeFile              scf[N];  
  FILE                    *fp;  
  int                     i;  
  int                     count; 
  char                    logBuf[LOGLEVELLEN]; 
  char                    *str_1_strbuf;
  char                    *str_2_strbuf; 
  
	memset(logBuf, 0, sizeof(logBuf));
  if((fp = fopen(filename, "rb")) == NULL)  
  {  
    sprintf(logBuf, "%s", "打开文件失败"); 
    WriteSySLog(4, logBuf);
    return NULL; 
  }  
   
  for(i = 0; i < N; i++)  
  {  
    if((fscanf(fp, "%s%s%s", scf[i].oneCol, scf[i].twoCol, scf[i].threeCol)) != 3)  
    {  
       break;  
    }  
  }
	str_1_strbuf = strstr_pos_length(buf, 48, 7);
	if(str_1_strbuf == NULL)
	{
		printf("str_1_strbuf is NULL");	
		return NULL;
	} 
  count = i;  
  for(i = 0; i < count; i++)  
  { 
  	if(strcmp(str_1_strbuf, scf[i].oneCol))
  	{
  		str_2_strbuf = strstr_pos_length(buf, 575, 7);
  		if(str_2_strbuf == NULL)
  		{
  			printf("str_2_strbuf is  NULL\n");
  			return NULL;	
  		}
  	} 
  }   
  return str_2_strbuf; 
}

//把文件中的内容扫描到一个结构体数组中并其处理字符串
int seg_Handle_Buf(char *buf)
{
	char                    logBuf[LOGLEVELLEN];
	scCodeFile              scf[N];  
  FILE                    *fp;  
  int                     i;  
  int                     count;
	
	memset(logBuf, 0, sizeof(logBuf)); 
	if(buf == NULL)
	{
		sprintf(logBuf, "%s", "buf is NULL\n");	
		return -1;
	}
	
	memset(logBuf, 0, sizeof(logBuf));
  if((fp = fopen("../etc/sc_config.ini", "rb")) == NULL)  
  {  
    sprintf(logBuf, "%s", "打开文件失败"); 
    WriteSySLog(4, logBuf);
    return -1; 
  }  
   
  for(i = 0; i < N; i++)  
  {  
    if((fscanf(fp, "%s%s%s", scf[i].oneCol, scf[i].twoCol, scf[i].threeCol)) != 3)  
    {  
       break;  
    }  
  }
  count = i;  
  for(i = 0; i < count; i++)  
  { 
  	if(strcmp(buf, scf[i].threeCol) == 0)
  	{
  		return 0;
  	} 
  	else
  	{
  		return -1;	
  	}
  }
  return 0;
}