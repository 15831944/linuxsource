/*
9��5�գ���Ϊ2014У԰��Ƹ�Ļ�����Ŀ
ͨ����������һ��Сд��ĸ(a~z)��ɵ��ַ��������дһ���ַ���ѹ�����򣬽��ַ�����������ϯ���ظ���ĸ����ѹ���������ѹ������ַ�����
ѹ������
    1����ѹ�������ظ����ֵ��ַ��������ַ���"abcbc"�����������ظ��ַ���ѹ������ַ�������"abcbc"��
    2��ѹ���ֶεĸ�ʽΪ"�ַ��ظ��Ĵ���+�ַ�"�����磺�ַ���"xxxyyyyyyz"ѹ����ͳ�Ϊ"3x6yz"��
Ҫ��ʵ�ֺ����� 
     void stringZip(const char *pInputStr, long lInputLen, char *pOutputStr);
    ����pInputStr��  �����ַ���lInputLen��  �����ַ�������
    ��� pOutputStr�� ����ַ������ռ��Ѿ����ٺã��������ַ����ȳ���
ע�⣺ֻ��Ҫ��ɸú��������㷨���м䲻��Ҫ���κ�IO���������
ʾ�� 
    ���룺��cccddecc��   �������3c2de2c��
    ���룺��adef��     �������adef��
    ���룺��pppppppp�� �������8p��
*/
#include <stdio.h>
void stringZip(const char *pInputStr, long lInputLen, char *pOutputStr)
{
	if(pInputStr == NULL || lInputLen == 0)
	{
		memset(pOutputStr, 0, lInputLen);
		return;
	}
	
	char *pIn = (char*)pInputStr;
	char *pOut = pOutputStr;
	char count = 0;
	int i = 0;
	char str = *pIn;
	for(; i < lInputLen; i++)
	{
		if(str == *pIn)
		{
			count++;
		}
		else
		{		
			if(count == 1)
			{
				*pOut = str;
				str = *pIn;
				pOut++;
				count = 1;
			}
			else if(count != 1)
			{
				sprintf(pOut, "%d", count);
				pOut++;
				*pOut = str;
				pOut++;
				str = *pIn;
				count = 1;
			}
		} 
		pIn++;
	}
}

int main(int argc, char *argv[])
{
	char *in = "pppppppp";
	//char *in = "adef";
	//char *in = "pppppppp";
	char out[sizeof("pppppppp")] = {0};
	stringZip(in, sizeof("pppppppp"), out);
	
	printf("out:%s\n", out);
	return 0;
}
