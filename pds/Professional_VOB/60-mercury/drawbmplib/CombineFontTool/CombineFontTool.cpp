// CombineFontTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CombineFontTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		CString strHello;
		strHello.LoadString(IDS_HELLO);
		cout << (LPCTSTR)strHello << endl;
	}

	// 
	const char achDefaultEnglish[] = "..\\res\\font\\En24";
	const char achDefaultChinese[] = "..\\res\\font\\Cn24";
	const char achEn50[] = "..\\res\\font\\En3232";
	const char achCn50[] = "..\\res\\font\\Cn3232";
	const char achDefaultFont[] = "..\\res\\font\\fontlib";

	// �ֿ�����
	// ����Ϊ�ַ��ͣ�Ŀ����ֻռһ���ֽ�
	char FT_HZK16 = 0x01;   // 16*16���� 
	char FT_HZK24 = 0x02;	// 24*24����
	char FT_HZK50 = 0x03;	// 50*50����

	// �����С
	char FONT_SIZE_24 = 24;	// 
	char FONT_SIZE_32 = 32;	// 

	// ��������
	char FONT_HEI  = 0x01; // ����
	char FONT_SONG = 0x02; // ����
	char FONT_KAI  = 0x03; // ����

	// ���ֿ��С
	
	// Ӣ���ֿ��С

	// ������
	char FONT_HEAD_END = 0xff;  // 

	FILE *rpEn = fopen(achDefaultEnglish, "rb");

	fseek(rpEn, 0, SEEK_END);
	int nEnFileSize = ftell(rpEn);
	
	char *pbyEnBuff = NULL;
	pbyEnBuff = new char[nEnFileSize];
	memset(pbyEnBuff, 0, nEnFileSize);

	fseek(rpEn, 0, SEEK_SET);
	fread(pbyEnBuff, nEnFileSize, 1, rpEn);// ��������
	fclose(rpEn);
	rpEn = NULL;

	FILE *rpCN = fopen(achDefaultChinese, "rb");
	fseek(rpCN, 0, SEEK_END);
	int nCNFileSize = ftell(rpCN);
	
	char *pbyCNBuf = NULL;
	pbyCNBuf = new char[nCNFileSize];
	fseek(rpCN, 0, SEEK_SET);
	fread(pbyCNBuf, nCNFileSize, 1, rpCN);  // ��������
	fclose(rpCN);
	rpCN = NULL;


	rpEn = fopen(achEn50, "rb");

	fseek(rpEn, 0, SEEK_END);
	int nEn32Size = ftell(rpEn);
	
	char *pbyEn50Buff = NULL;
	pbyEn50Buff = new char[nEn32Size];
	memset(pbyEn50Buff, 0, nEn32Size);

	fseek(rpEn, 0, SEEK_SET);
	fread(pbyEn50Buff, nEn32Size, 1, rpEn);// ��������
	fclose(rpEn);
	rpEn = NULL;

	rpCN = fopen(achCn50, "rb");
	fseek(rpCN, 0, SEEK_END);
	int nCN32Size = ftell(rpCN);
	
	char *pbyCN32Buf = NULL;
	pbyCN32Buf = new char[nCN32Size];
	fseek(rpCN, 0, SEEK_SET);
	fread(pbyCN32Buf, nCN32Size, 1, rpCN);  // ��������
	fclose(rpCN);
	rpCN = NULL;
	
	FILE *wp = fopen(achDefaultFont, "wb");
	// �ֿ�����
	fwrite(&FT_HZK24, sizeof(char), 1, wp);
	// �ֺ�
	fwrite(&FONT_SIZE_24, sizeof(char), 1, wp);
	// ����
	fwrite(&FONT_SONG, sizeof(char), 1, wp);
	// �����ֿ��С
	fwrite(&nCNFileSize, sizeof(int), 1, wp);
	// �����ֿ��С
 	fwrite(&nEnFileSize, sizeof(int), 1, wp);
	// �����ֿ�����
	fwrite(pbyCNBuf, nCNFileSize, 1, wp);
    // �����ֿ�����
 	fwrite(pbyEnBuff, nEnFileSize, 1, wp);

	// �ֿ�����
	fwrite(&FT_HZK50, sizeof(char), 1, wp);
	// �ֺ�
	fwrite(&FONT_SIZE_32, sizeof(char), 1, wp);
	// ����
	fwrite(&FONT_KAI, sizeof(char), 1, wp);
	// �����ֿ��С
	fwrite(&nCN32Size, sizeof(int), 1, wp);
	// �����ֿ��С
 	fwrite(&nEn32Size, sizeof(int), 1, wp);
	// �����ֿ�����
	fwrite(pbyCN32Buf, nCN32Size, 1, wp);
    // �����ֿ�����
 	fwrite(pbyEn50Buff, nEn32Size, 1, wp);

	// �����ֿ������
	fwrite(&FONT_HEAD_END, sizeof(char), 1, wp); 
	
	fclose(wp);
	wp = NULL;
	
	// �ͷ���Դ
	if( NULL != pbyCNBuf)
	{
		delete []pbyCNBuf;
		pbyCNBuf = NULL;
	}

	if( NULL != pbyEnBuff )
	{
		delete []pbyEnBuff;
		pbyEnBuff = NULL;
	}
	
	return nRetCode;
}


