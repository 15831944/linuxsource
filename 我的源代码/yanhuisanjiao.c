//#define N 13 //�������������,�������趨�������Ҫ����65536�� 
#include <stdio.h> 
int main(void)
{ 
	int n,i,j; int a[100][1000]; 
	printf("How Many Rows Do You Want:\n"); 
	scanf("%d",&n); 
	
 	//��ʼ�����飻 
	for(i=0;i<=n;i++)
		for(j=0;j<=2*n;j++)
			a[i][j]=0; 

	a[1][n]=1; 
	//����������ǵ���ֵ�� 
	for(i=0;i<=n;i++)	
		for(j=0;j<2*n;j++)		
			if(a[i][j]!=0)
			{ 
				a[i+1][j-1]=a[i][j-2]+a[i][j]; 
				a[i+1][j+1]=a[i][j+2]+a[i][j]; 
			}
			  
	//��ӡ������ǣ� 
	for(i=0;i<=n;i++)
	{ 
		for(j=0;j<2*n;j++)
		{ 
			if(a[i][j]!=0) 
				printf("%4d",a[i][j]); //��ӡ��ʽ���裬��ӡ�\t����ո�ȣ� 
			 //��Ҫ��Ϊ�˴�ӡ��ͼ�����ۣ� 
			else
				printf("   ");  
			if(j==(2*n-1))
				printf("\n");
		}
	}
	getchar();
	getchar();
	return 0;
}
