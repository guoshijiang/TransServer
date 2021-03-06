#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

struct msg_buf
{
    long mtype;
    char data[255];
};
 
/* 注意long 和 int 在32bit 和 64bit系统之下是不一样的
struct msg_buf
{
    long mtype;
    char data[255];
};
*/
 
int main()
{
    key_t key;
    int msgid;
    int ret;
    struct msg_buf msgbuf;
    int msgtype =  getpid();

    key=ftok("./msgfile",'a');
    printf("key =[%x]\n",key);
    
    printf("sizeof(long):%ld, sizeof(int):%d \n", sizeof(long), sizeof(int));
    
    msgid=msgget(key, IPC_CREAT |IPC_EXCL|0666); //通过文件对应

    if(msgid==-1)
    {
			if (errno == EEXIST)
			{
				printf("EEXIST:.....\n");
				key=ftok("./msgfile",'a');
				msgid=msgget(key, IPC_CREAT|0666); //通过文件对应
			}
			else
			{
			 	printf("create error\n");
				perror("msget: \n");
				return -1;
			}
    }
    printf("msgid:%d \n", msgid);

   	msgbuf.mtype = msgtype; //        getpid();

    printf("getpid(): %d \n", getpid());
    strcpy(msgbuf.data,"test haha");
    ret = msgsnd(msgid,&msgbuf, sizeof(msgbuf.data), IPC_NOWAIT);
    if(ret==-1)
    {
      printf("send message err\n");
      perror("senderr");
      return -1;
    }
    sleep(1);

    memset(&msgbuf,0,sizeof(msgbuf));
    
    ret=msgrcv(msgid, &msgbuf, sizeof(msgbuf.data), msgtype, IPC_NOWAIT);
    if(ret==-1)
    {
      printf("recv message err\n");
      perror("dd");
      return -1;
    }
    printf("recv msg =[%s]\n",msgbuf.data);
 
}