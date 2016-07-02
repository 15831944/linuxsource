/*
���ֲ��ҿ��Խ����Ԥ��������Ĳ��ң����⣺ֻҪ�����а���T����Ҫ���ҵ�ֵ����
��ôͨ��������С����T�ķ�Χ�����վͿ����ҵ�����һ��ʼ����Χ�����������顣
��������м�����T���бȽϣ������ų�һ��Ԫ�أ���Χ��Сһ�롣
�����������Ƚϣ�������С��Χ�����վͻ����������ҵ�T������ȷ��ԭ��ΪT���ڵķ�Χʵ��Ϊ�ա�
���ڰ���N��Ԫ�صı��������ҹ��̴�ԼҪ����log(2)N�αȽ�
*/
#include <stdio.h>

int midFind(int array[], int len, int value)
{
	int left = 0;
	int right = len-1;
	int mid = 0;
	
	while(left <= right)
	{
		mid = left + ((right - left)>>1);
		if(value > array[mid])
		{
			left = mid+1;
		}
		else if(value < array[mid])
		{
			right = mid-1;
		}
		else
			return mid;
	}
	return -1;
}

int main(int argc, char *argv[])
{
	int array[] = {1, 9, 10, 14, 17, 22, 44, 51, 77};
	int len = sizeof(array)/sizeof(int);
	int value = 13;
	printf("%d\n", midFind(array, len, value));
	return 0;
}
