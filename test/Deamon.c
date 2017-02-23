/*=========================================================
 *�ļ�����:  Deamon.c
 *�������ڣ�2017-1-24
 *�޸ļ�¼��
 *  2017-1-24  �״δ���
 *����������д��־
 ===========================================================*/
#include "Deamon.h"

/*=========================================================
 *�������: �� 
 *��������: ��
 *����ֵ:   ��
 *����˵��: �����ػ�����
 *===========================================================*/
/*
void SigtermHandler(int arg)
{
  _running = 0;
}
 */
void Daemon(void)
{
  pid_t                pid;
  int                  i;
  char                 errLog[LOGLEVELLEN];
  
  memset(errLog, 0, sizeof(errLog));
  pid = fork();
  if(pid < 0)
  {
    sprintf(errLog, "%s", "error fork\n");
    WriteSySLog(5, errLog);
    exit(1);
  }
  else if(pid > 0)
  {
    exit(0);
  }
  setsid();
  chdir("/btmsapp/btms/trans/bin");
  umask(0);
  for(i = 0; i < MAXFILE; i++)
  {
    close(i);
  }
  //signal(SIGTERM, SigtermHandler);
}
