/*==============================================================================
ģ����  ���ͻ���APP
�ļ���  ��clientospapp.h.cpp 
����ļ���clientospapp.h,stdafx.h
ʵ�ֹ��ܣ�ʵ�����ⲿ���ͻ��˵Ľ���
����    ��������
��Ȩ    ��<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���        �߶���        �޸ļ�¼
2013/08/08	  V1.0        ������						����
==============================================================================*/
#include "stdafx.h"
#include "clientospapp.h"

COspApp g_zClientOspApp;
s32 g_nServerNode = INVALID_NODE;							//�ͻ��˻�ȡ��node
/*==========================================================
������ CClientOspApp
���� ��CClientOspApp�Ĺ��캯������ʼ����Ա
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
CClientOspApp::CClientOspApp()
{
	//��Ա������ʼ��
}

/*==========================================================
��������~CClientOspApp
���ܣ�CClientOpsApp�������������ͷ���Դ
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
CClientOspApp::~CClientOspApp()
{
	
}

/*==========================================================
��������CInitClient
���ܣ���ʼ��������
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����TRUE ��ʼ���ɹ�  FALSE ʧ��
============================================================*/	
BOOL CClientOspApp::InitClient()
{
	//�ж��Ƿ��ʼ��
	if ( IsOspInitd() )
	{
		OspLog(LOGPRINTLEVELH, "�Ѿ���ʼ��!\n"); 
		return TRUE;
	}
	else
	{
		//δ��ʼ��
#ifdef _DEBUG
		if ( OspInit(TRUE, 0, "OspClient") )					//��ʼ��
		{
#else
		if ( OspInit(FALSE, 0, "OspClient") )
		{
			clientsetloglevel(0);								//ReleaseģʽĬ�Ϲرմ�ӡ
#endif		
			OspSetPrompt("OspClient");							//��ʾtelnet����
			clienthelp();										//��ʾ��ӡ��Ϣ
			OspLog(LOGPRINTLEVELH, "��ʼ���ɹ�!\n");
			return TRUE;
		}
		else
		{
			OspLog(LOGPRINTLEVELH, "��ʼ��ʧ��!\n");
			return FALSE;
		}
	}
}	

/*==========================================================
��������CCreatApp
���ܣ�����APP
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����TRUE ����APP�ɹ�  FALSE ʧ��
============================================================*/	
BOOL CClientOspApp::CreatClientApp()
{
	//�����ͻ���APP
	if ( g_zClientOspApp.CreateApp("OspClientApp", CLIENT_APP_ID, APP_PRIO) == OSP_ERROR )
	{
		OspLog(LOGPRINTLEVELH, "CreatClientApp����ʧ��!\n"); 
		return FALSE;
	}

	//���ӷ��������
	g_nServerNode = OspConnectTcpNode(inet_addr(m_pchServerIp), m_nServerPort);
	if ( g_nServerNode == INVALID_NODE )
	{
		OspLog(LOGPRINTLEVELH, "OspConnectTcpNode���ӷ��������ʧ��!\n");
		return FALSE;
	}
	return TRUE;
}

/*==========================================================
��������CConnectServer
���ܣ����ӷ�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/	
void CClientOspApp::ClientConnectServer()
{
	//��ͻ���Instance����������ϢC->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_CONN_REQ, NULL, 0);
}

/*==========================================================
��������CDisconnectServer
���ܣ��Ͽ�����������
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/	
void CClientOspApp::ClientDisconnectServer()
{
	//��ͻ���Instance���ͶϿ���ϢC->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_CONN_DISC, NULL, 0);
}

/*==========================================================
��������CUploadFile
���ܣ��ϴ��ļ�
�㷨ʵ�֣�<��ѡ��>
����˵����chUploadFile���ϴ����ļ���
����ֵ˵������
============================================================*/	
void CClientOspApp::ClientUploadFile(s8 *chUploadFile, DWORD dwGetFileSize)
{
	//�����ϴ����ļ���Ϣ
	CFileInfo m_tCFileInfo;
	memset(&m_tCFileInfo,0,sizeof(CFileInfo));
	memcpy(m_tCFileInfo.achFileName,chUploadFile,MAX_PATH);
	m_tCFileInfo.dwFileSize = dwGetFileSize;
	//��ͻ���Instance�����ϴ��ļ���ϢC->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_UPLOAD_REQ, &m_tCFileInfo, sizeof(CFileInfo));
}

/*==========================================================
��������CPauseUploadFile
���ܣ���ͣ�ϴ�
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/	
void CClientOspApp::ClientPauseUploadFile()
{
	//��ͻ���Instance������ͣ�ϴ���ϢC->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_UPLOADING_PAUSE, NULL, 0);
}

/*==========================================================
��������CRestartUploadFile
���ܣ��ϴ���ͣ�����¿�ʼ
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/	
void CClientOspApp::ClientRestartUploadFile()
{
	//��ͻ���Instance���ͼ����ϴ���ϢC->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_UPLOADING_RESTART, NULL, 0);
}

/*==========================================================
��������CRemoveFile
���ܣ�ɾ���б��ļ�
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/	
void CClientOspApp::ClientRemoveFile()
{
	//��ͻ���Instance����ɾ���б��ļ���ϢC->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_FILE_REMOVE, NULL, 0);
}

/*==========================================================
��������CGetServerIpandPort
���ܣ�����û����õķ�����IP���˿ں�
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/	
void CClientOspApp::ClientSetServerIpandPort(const s8 *pchServerIp, s32 nServerPort)
{
	memset(&m_pchServerIp, 0, MAX_NAME_LEN);

	//�ж��Ƿ�Ƿ�
	if ( strlen(pchServerIp) == 0 )
	{
		CB_CToSProgress(C_SEND_FILE_TO_S_SETERR);					//��������ص�
		return;
	}
	//����IP
	memcpy(m_pchServerIp, pchServerIp, MAX_NAME_LEN);		
	//����˿ں�
	m_nServerPort = nServerPort; 
}

//end
