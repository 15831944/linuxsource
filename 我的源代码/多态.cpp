//����ʾ�������ǿ�ܣ�˵�����������֮�����ʵ�ֶ�̬�ģ� 
#include <iostream>
using namespace std;

class cFather
{
public:
	cFather(){}
	virtual void fuc1()		//ע��ؼ���virtual 
	{
		printf("This is father!\n");
	}	
};

class cSon : public cFather	//ע��ؼ���public 
{
public:
	cSon(){}
	void fuc1()
	{
		printf("This is son!\n");
	}	
};

int main(int argc, char *argv[])
{
	cFather cfa;
	cSon cso;
	
	cfa.fuc1();
	cso.fuc1();
	
	cFather *pcfa = &cso;//new cSon();		//ע��˴���Ҫnew 
	pcfa->fuc1();
	
	if(pcfa != NULL)		//ע��˴���Ҫ�п� 
	{
//		delete pcfa;
	}
		
	return 0;
}
