/*==============================================================================
ģ����  ��������APP
�ļ���  ��cserverospapp.cpp 
����ļ���serverospapp.h,stdafx.h
ʵ�ֹ��ܣ���װ������Instance,ʵ���ⲿ���ýӿ�
����    ��������
��Ȩ    ��<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���        �߶���        �޸ļ�¼
2013/08/08	  V1.0        ������						����
==============================================================================*/
#include "stdafx.h"
#include "serverospapp.h"

COspApp g_zServerOspApp;						//��װzTemplateģ����

/*==========================================================
������ CServerOspApp
���� ��CServerOspApp�Ĺ��캯������ʼ����Ա
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
CServerOspApp::CServerOspApp()
{
	m_nServerNode = INVALID_NODE;
}

/*==========================================================
������ ~CServerOspApp
���� ��CServerOspApp�������������ͷ���Դ
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
CServerOspApp::~CServerOspApp()
{
	m_nServerNode = INVALID_NODE;
}

/*==========================================================
������ CInitServer
���� ����ʼ��������
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
void CServerOspApp::CInitServer()
{
	//��ʼ��osp
	if ( IsOspInitd() )
	{
		//�Ѿ���ʼ��
		return;
	}

#ifdef _DEBUG
	if ( OspInit(TRUE, 0) )					//DEBUG��ʼ��osp����telnet��ʾ
	{
#else
	if ( OspInit(FALSE, 0) )				//RELEASE��ʼ��osp���ر�telnet��ʾ
	{
		serversetloglevel(0);				//���ô�ӡ����Ĭ��release״̬�ر����д�ӡ
#endif
		OspSetPrompt("OspServer");			//��ʾtelnet����
		serverhelp();						//���������Ϣ
		CB_RevProgress(SERVER_INIT_OK);
		OspLog(LOGPRINTLEVELH, "��ʼ�������ɹ�!\n");
		return;
	}
	CB_RevProgress(SERVER_INIT_FAIL);		
}

/*==========================================================
������ CCreateServerTcpNode
���� ����������������APP
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
void CServerOspApp::CCreateServerTcpNode()
{
	//����Ƿ��Ѿ����������
	if ( m_nServerNode != INVALID_NODE)
	{
		return;
	}

	//�������������
	m_nServerNode = OspCreateTcpNode(0, g_nServerPort);
	if ( m_nServerNode == INVALID_NODE )
	{
		CB_RevProgress(SERVER_INIT_FAIL);		
		OspLog(LOGPRINTLEVELH, "�������ʧ��!\n");
		return;
	}
	
	//����������APP
	if ( OSP_ERROR == g_zServerOspApp.CreateApp("OspServerApp", SERVER_APP_ID, APP_PRIO) )
	{
		CB_RevProgress(SERVER_INIT_FAIL);		
		OspLog(LOGPRINTLEVELH, "����OspServerApp ʧ��!\n");
		return;
	}
}

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern s64 g_dwServerSpace;
/////////////////////////////Telnet�ӿ�/////////////////////////////////

#define DEMO_VER  "Vertion: 2013/08/08 DEMO_SERVER V1.0!"

/*==========================================================
������ setloglevel
���� �����ô�ӡ����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
API void serversetloglevel(u8 byScrnLevel)
{
	OspSetScrnLogLevel(0, byScrnLevel);
	OspPrintf( TRUE,FALSE, "setloglevel %d!\n", byScrnLevel);
}

/*==========================================================
������ serverver
���� ����ӡ��ǰ�汾��Ϣ
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
API void serverver()
{
	OspPrintf( TRUE,FALSE, "%s  compile time: %s, %s\n", DEMO_VER, __TIME__, __DATE__ );
}

/*==========================================================
������ serverspace
���� ����ӡ��ǰʣ��ռ�
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
API void serverspace()
{
	//��ȡ��ǰ���̴�С
	ULARGE_INTEGER nFreeBytesAvailable;
	GetDiskFreeSpaceEx(_T(g_achServerBase), &nFreeBytesAvailable, 0, 0);
	ULONGLONG dwServerSpace = nFreeBytesAvailable.QuadPart / 1024 / 1024;
	OspPrintf( TRUE,FALSE, "��ǰʣ��ռ䣺%ld M\n", dwServerSpace );
}

/*==========================================================
������ serverhelp
����   ������Ϣ
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
API void serverhelp()
{
    OspPrintf(TRUE, FALSE, "\n");
    OspPrintf(TRUE, FALSE, "********************** ���� *********************\n");
    OspPrintf(TRUE, FALSE, "*                                               *\n");
    OspPrintf(TRUE, FALSE, "*����������������                           *\n");
    OspPrintf(TRUE, FALSE, "*����Log����:      serversetloglevel            *\n");
    OspPrintf(TRUE, FALSE, "*�鿴�汾��Ϣ:     serverversion                *\n");
	OspPrintf(TRUE, FALSE, "*�鿴��ǰʣ��ռ�: serverspace                  *\n");
    OspPrintf(TRUE, FALSE, "*�鿴����:         serverhelp                   *\n");
    OspPrintf(TRUE, FALSE, "*����Log����ʱ������������磺                  *\n");
    OspPrintf(TRUE, FALSE, "*      serversetloglevel 1                      *\n");
    OspPrintf(TRUE, FALSE, "*����˵�����£�                                 *\n");
    OspPrintf(TRUE, FALSE, "*      0 ����ӡ�κ�����LOG                      *\n");
    OspPrintf(TRUE, FALSE, "*      1 ��ӡ���ӣ����ͣ��Ͽ���LOG              *\n");
    OspPrintf(TRUE, FALSE, "*      2 ��ӡ������ϢLOG                        *\n");
    OspPrintf(TRUE, FALSE, "*************************************************\n");
}

/*==========================================================
������ CB_RevProgress
����   �ص�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void (*pCB_RevProgress)(s8 chProgress) = NULL;					//����ָ������

void CB_RevProgress(s8 chProgress)								
{
	pCB_RevProgress(chProgress);
}

void CB_ServerCallBack(void (*pCB_ServerRevProgress)(s8 chProgress))
{
	pCB_RevProgress = pCB_ServerRevProgress;
}
#ifdef __cplusplus
}
#endif  /* __cplusplus */

//end