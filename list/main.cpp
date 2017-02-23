#include "CKdNodeData.h"
#include "CKdNodeList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;


int main()
{

	CKdNodeList* list = new CKdNodeList;

	CKdNodeData* data = new CKdNodeData[5];
	
	char buf[20] ="";

	for(int i=0;i<5;i++)
	{
		memset(buf,0,sizeof(buf));
		sprintf(buf,"%d%d%d%d",i,i,i,i);
		data->SetData(buf,sizeof(buf));
		list->AppendNode(data);
		data++;
	}
	
	sleep(3);

	CKdNodeData data2;

	list->PrintList();
	
	list->CheckNode(0);

	cout<<"aaaaa  "<<list->GetListNodeNum()<<endl;


	
	list->ClearList();

	cout<<"clearlist  "<<list->GetListNodeNum()<<endl;
	

	system("pause");
	return 0;

}