/*=========================================================
 *文件名称: 			Config.h
 *创建日期：			2017-1-23
 *修改记录：			2017-1-23  首次创建
 *功能描述：     读配置文件公共函数
 *===========================================================*/
#ifndef __CONFIG_H_
#define __CONFIG_H_

#include<stdio.h>  
#include<string.h>  
#include<stdlib.h>
#include "WriteLog.h"

#define LineMaxLen 2048  
#define KeyMaxLen 128  
#define MaxFileLength 1024*10  

/*===========================================================
*全局变量定义
============================================================*/
char value[KeyMaxLen];

/*=========================================================
 *函数入参: linebuf    要读取的行缓冲区
 *          pKey       配置文件中key=value中key的值
 *函数出参: 无
 *返回值:   在这儿不解释
 *功能说明:该函数作用是判断linebuf中是否存在pKey这里使用的完全匹配
 *===========================================================*/
static char *substr(char *linebuf,char *pKey);

/*=========================================================
 *函数入参:   pFileName  文件的名字
 *            pKey       配置文件中key=value中key的值
 *						pValue     配置文件中key=value中value的值
 *函数出参:   pValueLen  配置文件中key=value中value的长度
 *            pValue     配置文件中key=value中value的值
 *返回值:     在这儿不解释
 *功能说明:解析每一行，若匹配key关键字，在进行value值的提取,提取value值需要 去除前后空格
 *===========================================================*/  
int GetCfItem(const char *pFileName, char *pKey, char * pValue, int * pValueLen);  

/*=========================================================
 *函数入参:  key    传入的是配置文件中的key=value中的key
 *函数出参:  无
 *返回值:    返回配置文件中key=value中的value的值
 *功能说明:  获取配置文件中的配置项
 *===========================================================*/
char *ReadConItem(char *key);
 
#endif  