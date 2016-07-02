/*
����˼���ǣ�ͨ��һ������Ҫ��������ݷָ�ɶ����������֣�
����һ���ֵ��������ݶ�������һ���ֵ��������ݶ�ҪС��
Ȼ���ٰ��˷����������������ݷֱ���п�������
����������̿��Եݹ���У��Դ˴ﵽ�������ݱ���������С�
*/
#include <stdio.h>
void quicksort(int array[], int left, int right) 
{
	if(left > right)
		return;
		
	int base = array[left];
	int i = left;
	int j = right;
	while(i != j)
	{
		//�����ұ��ҵ���baseС��Ϊֹ 
		while(array[j] >= base && i < j)
			j--;
		
		//��������ҵ���base���Ϊֹ 
		while(array[i] <= base && i < j)
			i++;
		
		//�������� 
		if(i < j)
		{
			int tmp = array[i];
			array[i] = array[j];
			array[j] = tmp;
		}
	}
	
	//����base���м�ֵ 
	array[left] = array[i];
	array[i] = base;
			
	quicksort(array, left, i-1); 
	quicksort(array, i+1, right); 
}

int main(int argc, char *argv[])
{
	int b[10] = {2,5,1,9,7,6,8,4,3,0};
	int j = 0;
	quicksort(b,0,9);
	for(j=0;j<10;j++)
		printf("%d  ",b[j]);
	printf("\n");
	return 0;
}
