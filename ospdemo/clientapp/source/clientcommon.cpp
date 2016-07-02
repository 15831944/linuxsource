/*==============================================================================
ģ����  �������ӿ�
�ļ���  ��clientcommon.cpp  
����ļ���clientospapp.h,stdafx.h
ʵ�ֹ��ܣ���װ�����ӿ�
����    ��������
��Ȩ    ��<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���        �߶���        �޸ļ�¼
2013/08/08	  V1.0        ������						����
==============================================================================*/
#include "stdafx.h"
#include "clientospapp.h"

/////////////////////////////��װ�ӿڣ����ⲿ����///////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*==========================================================
��������CInitClient
���ܣ���ʼ��������
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����TRUE ��ʼ��������APP�ɹ�  FALSE ʧ��
============================================================*/	
BOOL CInitClient()
{
	//�ж��Ƿ��ʼ��
	return g_zClientOspApp.InitClient();
}	

/*==========================================================
��������CCreatApp
���ܣ�����APP
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����TRUE ����APP�ɹ�  FALSE ʧ��
============================================================*/	
BOOL CCreatApp()
{
	//�����ͻ���APP�Ƿ�ɹ�
	return g_zClientOspApp.CreatClientApp();
}

/*==========================================================
��������CConnectServer
���ܣ����ӷ�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
void CConnectServer()
{
	g_zClientOspApp.ClientConnectServer();
}

/*==========================================================
��������CDisconnectServer
���ܣ��Ͽ�����������
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/	
void CDisconnectServer()
{
	g_zClientOspApp.ClientDisconnectServer();
}

/*==========================================================
��������CUploadFile
���ܣ��ϴ��ļ�
�㷨ʵ�֣�<��ѡ��>
����˵����chUploadFile���ϴ����ļ���
����ֵ˵������
============================================================*/	
void CUploadFile(s8 *chUploadFile, DWORD dwGetFileSize)
{
	if ( strlen(chUploadFile) == 0 )
	{
		return;
	}

	g_zClientOspApp.ClientUploadFile(chUploadFile, dwGetFileSize);
}

/*==========================================================
��������CPauseUploadFile
���ܣ���ͣ�ϴ�
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
void CPauseUploadFile()
{
	g_zClientOspApp.ClientPauseUploadFile();
}

/*==========================================================
��������CRestartUploadFile
���ܣ��ϴ���ͣ�����¿�ʼ
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/	
void CRestartUploadFile()
{
	g_zClientOspApp.ClientRestartUploadFile();
}

/*==========================================================
��������CRemoveFile
���ܣ�ɾ���б��ļ�
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
void CRemoveFile()
{
	g_zClientOspApp.ClientRemoveFile();
}

/*==========================================================
��������CGetServerIpandPort
���ܣ�����û����õķ�����IP���˿ں�
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/	
void CSetServerIpandPort(const s8 *pchServerIp, s32 nServerPort)
{
	if ( strlen(pchServerIp) == 0 )
	{
		CB_CToSProgress(C_SEND_FILE_TO_S_SETERR);					//��������ص�
		return;
	}
	
	g_zClientOspApp.ClientSetServerIpandPort(pchServerIp, nServerPort);	
}

//////////////////////////////Telnet�ӿ�/////////////////////////////////

#define DEMO_VER  "Vertion: 2013/08/08 DEMO_CLIENT V1.0!"

/*==========================================================
������ clientsetloglevel
���� �����ô�ӡ����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
API void clientsetloglevel(u8 byScrnLevel)
{
	OspSetScrnLogLevel(0, byScrnLevel);
	OspPrintf( TRUE, FALSE, "setloglevel %d!\n", byScrnLevel);
}

/*==========================================================
������ clientversion
���� ����ӡ��ǰ�汾��Ϣ
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
API void clientversion()
{
	OspPrintf( TRUE, FALSE, "%s  compile time: %s, %s\n", DEMO_VER, __TIME__, __DATE__ );
}

/*==========================================================
������ clienthelp
����   ������Ϣ
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
API void clienthelp()
{
    OspPrintf(TRUE, FALSE, "\n");
    OspPrintf(TRUE, FALSE, "********************** ���� *********************\n");
    OspPrintf(TRUE, FALSE, "*                                               *\n");
    OspPrintf(TRUE, FALSE, "*����������������                           *\n");
    OspPrintf(TRUE, FALSE, "*����Log����:      clientsetloglevel            *\n");
    OspPrintf(TRUE, FALSE, "*�鿴�汾��Ϣ:     clientversion                *\n");
    OspPrintf(TRUE, FALSE, "*�鿴����:         clienthelp                   *\n");
    OspPrintf(TRUE, FALSE, "*����Log����ʱ������������磺                  *\n");
    OspPrintf(TRUE, FALSE, "*      clientsetloglevel 1                      *\n");
    OspPrintf(TRUE, FALSE, "*����˵�����£�                                 *\n");
    OspPrintf(TRUE, FALSE, "*      0 ����ӡ�κ�����LOG                      *\n");
    OspPrintf(TRUE, FALSE, "*      1 ��ӡ���ӣ����ͣ��Ͽ���LOG              *\n");
    OspPrintf(TRUE, FALSE, "*      2 ��ӡ������ϢLOG                        *\n");
    OspPrintf(TRUE, FALSE, "*      3 ��ӡ���Ͱٷֱ�LOG                      *\n");
    OspPrintf(TRUE, FALSE, "*************************************************\n");
}

/*==========================================================
������ CB_CToSProgress
����   �ص�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void (*pCB_CToSProgress)(s8 chProgress) = NULL;					//����ָ������

void CB_CToSProgress(s8 chProgress)								
{
	pCB_CToSProgress(chProgress);
}

void CB_CCallBack(void (*pCB_CToCProgress)(s8 chProgress))
{
	pCB_CToSProgress = pCB_CToCProgress;
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

//end