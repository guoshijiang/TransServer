#include <stdio.h>
#include <string.h>
#include <string.h>
#include<math.h>

#define MAXLEN 10240 

char str_str[10];

char* main01(char buf[2048])
{
	strncpy(str_str, buf+49, 7);
	return str_str;
}

int main02()
{
	char url[100] = "http://see.xidian.edu.cn";
  char path[30] = ", /cpp/u/biaozhunku/";
  strncat(url, path, 1000);  // 1000远远超过path的长度
  printf("%s\n", url);
  return  0;
}


//读取文件filename的内容到dest数组，最多可以读maxlen个字节 
//成功返回文件的字节数，失败返回－1 
int read_file(const char *filename, char *dest, int maxlen) 
{ 
 FILE *file; 
 int pos, temp, i; 

 //打开文件 
 file = fopen(filename, "r"); 
 if( NULL == file ) 
 { 
  fprintf(stderr, "open %s error\n", filename); 
  return -1; 
 } 

 pos = 0; 
 //循环读取文件中的内容 
 for(i=0; i<MAXLEN-1; i++) 
 { 
  temp = fgetc(file); 
  if( EOF == temp ) 
   break; 
  dest[pos++] = temp; 
 } 
 //关闭文件
 fclose(file);
 //在数组末尾加0 
 dest[pos] = 0; 

 return pos; 
} 


int main03(int argc, char **argv) 
{ 
 if( argc != 2 ) 
 { 
  fprintf(stderr, "Using: ./read <filename>\n"); 
  return -1; 
 } 

 char buffer[MAXLEN]; 
 int len = read_file(argv[1], buffer, MAXLEN); 

 //输出文件内容 
 printf("len: %d\ncontent: \n%s\n", len, buffer); 

 return 0; 
} 

  
      
int main04()  
{  
    char str[]="ab,cd,ef";  
    char *ptr;  
    printf("before strtok:  str=%s\n",str);  
    printf("begin:\n");  
    ptr = strtok(str, ",");  
    while(ptr != NULL){  
        printf("str=%s\n",str);  
        printf("ptr=%s\n",ptr);  
        ptr = strtok(NULL, ",");  
    }  
    system("pause");  
    return 0;  
}  

 
int main05(void)
{
  char str[100] ="123568qwerSDDAE";
  char lowercase[100];
  int num;
  sscanf(str,"%d %[a-z]", &num, lowercase);
  printf("The number is: %d\n", num);
  printf("The lowercase is: %s\n", lowercase);
  return 0;
}

/*
typedef struct student
{
 char no[9];
 char name[10];
 char mark[10];
}STU;

STU *temp;

int main06()
{ 
	int n;
	FILE *fp=fopen("../etc/sc_config.ini", "r");
	temp=(STU*)malloc(100 * sizeof(STU));
	for (n=0;n<10;n++)
	{
		fscanf(fp,"%[^,],%[^,],%[^,]",temp[n].no,temp[n].name, temp[n].mark);
		printf("1-%s 2-%s 3-%s\n",temp[n].no,temp[n].name,temp[n].mark);
	}
fclose(fp);
}
*/


#define N 60  

typedef struct   
{  
    char work_ID[5];  
    char name[20];  
    char phone_nu[12];  
}student;  
  
  
int main(int argc, char *argv[])  
{  
    student st[N];  
    FILE *fp;  
    int i;  
    int count;  
  
    if(argc != 2)  
    {  
        fprintf(stderr, "usage:argc is not two\n");  
        exit(1);  
    }  
  
    if((fp = fopen(argv[1], "rb")) == NULL)  
    {  
        fprintf(stderr, "Can't open the %s", argv[1]);  
    }  
      
    for(i = 0; i < N; i++)  
    {  
        if((fscanf(fp, "%s%s%s", st[i].work_ID, st[i].name, st[i].phone_nu)) != 3)  
        {  
            break;  
        }  
    }  
      
    count = i;  
    //display  
    printf("the ture count is %d\n",count);  
    for(i = 0; i < count; i++)  
    {  
        printf("work_ID->%s\t name->%s\t phone_nu->%s\n", st[i].work_ID, st[i].name, st[i].phone_nu);  
    }  
      
    return 0;  
}  