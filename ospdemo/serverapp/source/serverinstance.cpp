/*==============================================================================
ģ����  ��������ʵ��
�ļ���  ��serverinstance.cpp 
����ļ���serverospapp.h,stdafx.h
ʵ�ֹ��ܣ�ʵ��Instanceͨ�Ź���
����    ��������
��Ȩ    ��<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���        �߶���        �޸ļ�¼
2013/08/08	  V1.0        ������						����
==============================================================================*/

#include "stdafx.h"
#include "serverospapp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern s8 g_achServerBase[MAX_NAME_LEN];			//��¼�ļ���ŵ�Ŀ¼

#ifdef __cplusplus
}
#endif  /* __cplusplus */

/*==========================================================
������ CServerInstance
���� ��CServerInstance�Ĺ��캯������ʼ����Ա
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
CServerInstance::CServerInstance()						
{
	//��Ա������ʼ��																
	ServerInstanceRelease();
	m_nSaveClientNode   = 0;
	m_nSaveClientID		= 0;
	m_dwServerSpace		= 0;
}

/*==========================================================
������ ~CServerInstance
���� ��CServerInstance�������������ͷ���Դ
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
CServerInstance::~CServerInstance()
{
	ServerInstanceRelease();					//�ͷ���Դ
	m_nSaveClientNode   = 0;
	m_nSaveClientID		= 0;
}

/*==========================================================
������ DaemonInstanceEntry
���� ��app�����շ�����ڣ������������Ӧ����
�㷨ʵ�֣�<��ѡ��>
����˵����pMsg:��Ϣ
		  pcApp:��instance������app
����ֵ˵����void
============================================================*/
void CServerInstance::DaemonInstanceEntry(CMessage *const pMsg,CApp* pcApp)
{

}

/*==========================================================
������ DaemonInstanceEntry
���� ��app�����շ�����ڣ������������Ӧ����
�㷨ʵ�֣�<��ѡ��>
����˵����pMsg:��Ϣ
����ֵ˵����void
============================================================*/
void CServerInstance::InstanceEntry(CMessage *const pMsg)
{
	//�ж���Ϣ�Ƿ�Ϊ��
	if ( pMsg == NULL )
	{
		return;
	}

	//���μ����ϴ���Ϣ��ӡ
	if ( pMsg->event != CTOS_UPLOADING_REQ)
	{
		OspLog(LOGPRINTLEVELM, "GET MESSAGE NO.%d!\n", pMsg->event);
	}

	switch ( pMsg->event )
	{
	case CTOS_CONN_REQ:								//����������Ϣ
		{
			CConnectSReqManage(pMsg);
		}
		break;
	case CTOS_UPLOAD_REQ:							//�ϴ�������Ϣ						
		{
			CUploadtoSReqManage(pMsg);
		}
		break;
	case CTOS_UPLOAD_FREADERR:						//�ϴ��ļ�ʱ�쳣������Ϣ						
		{
			CUploadtoSErrManage();
		}
		break;
	case CTOS_UPLOADING_REQ:						//�����ϴ��ļ�������Ϣ
		{
			CUploadingtoSReqManage(pMsg);
		}
		break;
	case OSP_DISCONNECT:							//����
		{
			DisConnectedCtoSManage(m_nSaveClientNode);
		}
		break;
	case S_WAIT_UPLOAD_TIMER:						//���ӵ��ϴ��Ķ�ʱ
		{
			DisConnectedCtoSManage(m_nSaveClientNode);
		}
		break;
	case S_WAIT_UPLOAD_AGAIN_TIMER:					//�ϴ���ɵ��ٴ��ϴ��Ķ�ʱ
		{
			DisConnectedCtoSManage(m_nSaveClientNode);
		}
		break;
	default :
		{
			OspLog(LOGPRINTLEVELH,"GET INSTANCE NONE_SENCE!\n");  
		}
		break;
	}	
}

/*==========================================================
������ CConnectSReqManage
���� ���յ�����Ҫ��ʱ�Ĵ�����
�㷨ʵ�֣�<��ѡ��>
����˵����pMsg:��Ϣ 
����ֵ˵����void
============================================================*/
void CServerInstance::CConnectSReqManage(CMessage *const pMsg)
{
	//�����Ϣ�Ƿ�Ϊ��
	if ( pMsg == NULL)
	{
		NextState(STATE_IDLE);
		return;
	}

	m_nSaveClientNode = pMsg->srcnode;                              // ��������ʱ������ͻ���NODE��
	m_nSaveClientID = pMsg->srcid;                                  // ��������ʱ������ͻ���ID�� 

	OspNodeDiscCBRegQ(m_nSaveClientNode, GetAppID(), GetInsID());	//���ö������
	//��ͻ��˷�������Ӧ��
	OspPost(m_nSaveClientID, STOC_CONN_ACK, NULL, 0, m_nSaveClientNode,
		MAKEIID(SERVER_APP_ID, GetInsID()), 0);
	NextState(STATE_READY);
	SetTimer(S_WAIT_UPLOAD_TIMER, 3*60*1000); 					//���ж�ʱ�������ȴ��ͻ����ϴ�3���ӳ�ʱ
}

/*==========================================================
������ DisConnectedCtoSManage
���� ����ʱ�Ͽ�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CServerInstance::DisConnectedCtoSManage(u32 SrcNodeID)
{	
	//����ʱ�鿴�Ƿ���δ��������ļ���ɾ��֮
	if ( strlen(m_achPathFileName) != 0 )
	{
		FILE *pFile = fopen(m_achPathFileName, "rb+");
		fseek(pFile, 0, SEEK_END);
		DWORD dwFileLength = ftell(pFile);
		fclose(pFile);
		if (m_tCGetFileInfo.dwFileSize > dwFileLength)
		{
			//ɾ������δ��ɵ��ļ�
			remove(m_achPathFileName);										
		}
	}
	OspDisconnectTcpNode(SrcNodeID);		//�Ͽ�����
	ServerInstanceRelease();				//�ͷ���Դ
	m_nSaveClientNode   = 0;
	m_nSaveClientID		= 0;
	KillTimer(S_WAIT_UPLOAD_TIMER); 							//�յ��ϴ�����ȡ��Timer
	KillTimer(S_WAIT_UPLOAD_AGAIN_TIMER); 						//�յ��ϴ�����ȡ��Timer
	NextState(STATE_IDLE);
	OspLog(LOGPRINTLEVELH, "�Ͽ��������!\n");
}

/*==========================================================
������ CUploadtoSReqManage
���� ���յ������ϴ�Ҫ��ʱ�Ĵ�����
�㷨ʵ�֣�<��ѡ��>
����˵����pMsg:��Ϣ
����ֵ˵����void
============================================================*/
void CServerInstance::CUploadtoSReqManage(CMessage *const pMsg)
{
	KillTimer(S_WAIT_UPLOAD_TIMER); 							//�յ��ϴ�����ȡ��Timer
	KillTimer(S_WAIT_UPLOAD_AGAIN_TIMER); 						//�յ��ϴ�����ȡ��Timer

	s8 achFileName[MAX_NAME_LEN] = {0};							//��ʱ�����ļ���

	//�ж��Ƿ�Ϊ��
	if ( pMsg == NULL )
	{
		s8 eErrCode = STOC_FILE_CREATEERR;
		//��ͻ��˷��;ܾ��ϴ���Ϣ
		post(m_nSaveClientID, STOC_UPLOAD_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		OspLog(LOGPRINTLEVELH, "CUploadtoSReqManage��pMsg == NULL!\n"); 
		return;
	}

	//�жϵ�ǰ״̬
	if ( CurState() != STATE_READY )
	{
		s8 eErrCode = STOC_FILE_CREATEERR;
		//��ͻ��˷��;ܾ��ϴ���Ϣ
		post(m_nSaveClientID, STOC_UPLOAD_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		OspLog(LOGPRINTLEVELH, "CUploadtoSReqManage��STATE ERROR.%d!\n", CurState()); 
		CUploadtoSErrManage();
		return;
	}

	memset(&m_tCGetFileInfo, 0, sizeof(CFileInfo));
	memcpy(&m_tCGetFileInfo, pMsg->content, pMsg->length);	
	memcpy(achFileName, m_tCGetFileInfo.achFileName, MAX_NAME_LEN);

	FileNameOpt(achFileName);											//��ȡ�ϴ����ļ���
	if ( strlen(achFileName) == 0 )
	{
		s8 eErrCode = STOC_FILE_CREATEERR;
		//��ͻ��˷��;ܾ��ϴ���Ϣ
		post(m_nSaveClientID, STOC_UPLOAD_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		OspLog(LOGPRINTLEVELH, "CUploadtoSReqManage���ļ���Ϊ��!\n");
		return;
	}

	//��ϳɴ��·���ļ���
	memset(m_achPathFileName, 0, MAX_NAME_LEN);
	memcpy(m_achPathFileName, g_achServerBase, MAX_NAME_LEN);
	strcat(m_achPathFileName, "\\");
	strcat(m_achPathFileName, achFileName);
	FileRenameManage(m_achPathFileName);						 		//�ж��Ƿ��ļ�����

	//�жϿռ��Ƿ����
	if ( !IsServerSpaceEnough(m_tCGetFileInfo.dwFileSize) )	
	{
		s8 eErrCode = STOC_SERVER_NOSPACE;
		OspLog(LOGPRINTLEVELH,"CUploadtoSReqManage:�ռ䲻��!\n");
		//��ͻ��˷����ļ�����ʧ����Ϣ
		post(m_nSaveClientID, STOC_UPLOAD_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();	
		CB_RevProgress(SERVER_NOSPACE);								//�ռ䲻��
		return;
	}	
	
	m_phGetFile = fopen(m_achPathFileName, "wb+");						//�õ��ļ����,�������ļ�
	if ( m_phGetFile == NULL )
	{
		s8 eErrCode = STOC_FILE_CREATEERR;
		OspLog(LOGPRINTLEVELH,"CUploadtoSReqManage:m_phGetFile == NULL!\n");
		post(m_nSaveClientID, STOC_UPLOAD_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		return;
	}	

	//��ͻ��˷����ϴ�Ӧ��
	post(m_nSaveClientID, STOC_UPLOAD_ACK, &m_tCGetFileInfo, sizeof(CFileInfo), m_nSaveClientNode);
	fclose(m_phGetFile);
	m_phGetFile = NULL;
	NextState(STATE_BUSY);													
}

/*==========================================================
������ CUploadingtoSReqManage
���� �����յ������ϴ�δ��ɵ��ļ�Ҫ��ʱ�Ĵ�����
�㷨ʵ�֣�<��ѡ��>
����˵����pMsg:��Ϣ
����ֵ˵����void
============================================================*/
void CServerInstance::CUploadingtoSReqManage(CMessage *const pMsg)
{
	//�ж��Ƿ��
	if ( pMsg == NULL )
	{
		s8 eErrCode = STOC_FILE_WRITEERR;
		//��ͻ��˷���дʧ����Ϣ
		post(m_nSaveClientID, STOC_UPLOADING_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		OspLog(LOGPRINTLEVELH, "CUploadingtoSReqManage:pMsg == NULL!\n"); 
		return;
	}

	if ( (CurState() != STATE_BUSY) )
	{
		s8 eErrCode = STOC_FILE_WRITEERR;
		//��ͻ��˷���дʧ����Ϣ
		post(m_nSaveClientID, STOC_UPLOADING_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		OspLog(LOGPRINTLEVELH, "CUploadingtoSReqManage:STATE ERROR.%d!\n", CurState());
		CUploadtoSErrManage();
		return;
	}

	memset(&m_tGetFileInfo, 0 , sizeof(UploadFileInfo));
	memcpy(&m_tGetFileInfo, pMsg->content, pMsg->length);

	m_phGetFile = fopen(m_achPathFileName, "ab+");					//�õ��ļ����,�����ļ�ĩβ׷������
	if ( m_phGetFile == NULL )
	{
		s8 eErrCode = STOC_FILE_WRITEERR;
		//��ͻ��˷���дʧ����Ϣ
		post(m_nSaveClientID, STOC_UPLOADING_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		OspLog(LOGPRINTLEVELH, "CUploadingtoSReqManage:m_phGetFile == NULL!\n"); 
		return;
	}

	if ( fwrite(m_tGetFileInfo.achData, 1, m_tGetFileInfo.nDataLen, m_phGetFile) == 0 )	//д���ݵ��ļ�
	{
		s8 eErrCode = STOC_FILE_WRITEERR;
		post(m_nSaveClientID, STOC_UPLOADING_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		OspLog(LOGPRINTLEVELH, "CUploadingtoSReqManage:fwrite error!\n"); 
		return;
	}
	fflush(m_phGetFile); 
	fseek(m_phGetFile, 0, SEEK_END);
	DWORD dwFileLength = ftell(m_phGetFile);
	fclose(m_phGetFile);//�ر��ļ����

	//�жϵ�ǰ�Ƿ��Ѿ�д��
	if ( dwFileLength == m_tCGetFileInfo.dwFileSize )
	{
		SetTimer(S_WAIT_UPLOAD_AGAIN_TIMER, 3*60*1000); 				//�ȴ��´��ϴ�3���ӳ�ʱ
		ServerInstanceRelease();										//�ͷ���Դ
		NextState(STATE_READY);
		OspLog(LOGPRINTLEVELH, "CUploadingtoSReqManage:write done!\n"); 
		return;
	}

	//��ͻ��˷��ͼ����ϴ�����Ϣ
	post(m_nSaveClientID, STOC_UPLOADING_ACK, NULL, 0, m_nSaveClientNode);			
}

/*==========================================================
������ CUploadtoSErrManage
���� ���ļ��ϴ������У��ͻ��˶�����Ĵ�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CServerInstance::CUploadtoSErrManage()
{
	//�鿴�Ƿ���δ��������ļ���ɾ��֮
	if ( strlen(m_achPathFileName) != 0 )
	{
		FILE *pFile = fopen(m_achPathFileName, "rb+");
		fseek(pFile, 0, SEEK_END);
		DWORD dwFileLength = ftell(pFile);
		fclose(pFile);
		if (m_tCGetFileInfo.dwFileSize > dwFileLength)
		{
			//ɾ������δ��ɵ��ļ�
			remove(m_achPathFileName);											
		}
	}
	//�ͷ���Դ
	ServerInstanceRelease();													
	NextState(STATE_READY);
}

/*==========================================================
������ FileNameOpt
���� ����ȡ�ϴ��ļ����ļ���
�㷨ʵ�֣�<��ѡ��>
����˵����pName��������·�����ڿͻ��ˣ����ļ���
����ֵ˵����
============================================================*/
void CServerInstance::FileNameOpt(char *pchName)
{
	//�ж��Ƿ��
	if ( pchName == NULL )
	{
		return;
	}

	s8 achName[MAX_NAME_LEN] = {0};
	strcpy(achName, pchName);
	//��ȡ·���ļ��е��ļ���
	for ( int i = strlen(achName); i > 0; i-- )
	{
		if ( pchName[i-1] == '\\' )
		{
			strcpy(pchName, &achName[i]);
		}
	}
}

/*==========================================================
������ FileRenameManage
���� ���������˻�ȡ�ϴ��ļ�ʱ����ͬ���ļ��Ĵ�����
�㷨ʵ�֣��ļ�����/ƥ���㷨
����˵����pFileName:�ļ�����
����ֵ˵����void
============================================================*/
void CServerInstance::FileRenameManage(s8* pFileName)
{
	s8 achReName[MAX_NAME_LEN] = {0};							//��¼�ļ�
	s32 nNum = 1;
	BOOL bfilesame = FALSE;
	strcpy(achReName, pFileName);								//����Դ����ֹ���ı��������
	do 
	{
		//ƥ��Ŀ��Ŀ¼�µ��ļ��������յ��ļ���
		bfilesame = access(pFileName, 0) == 0;
		if ( bfilesame )
		{
			strcpy(pFileName, achReName);
			s8 cfileback[MAX_NAME_LEN] = {0};					//��¼��չ��ǰ�沿��
			s8 cfilefront[MAX_NAME_LEN] = {0};					//��¼��չ��
			for ( s32 i = strlen(pFileName); i > 0; i-- )
			{
				//����鵽�ļ������к�׺������ǰ�Ĳ��ּ�num����׺���
				if ( pFileName[i-1] == '.' )
				{
					strncpy(cfilefront, pFileName, i-1);
					strncpy(cfileback, &pFileName[i], strlen(pFileName) - i);
					sprintf(pFileName, "%s%d.%s", cfilefront, nNum, cfileback);
					break;
				}

				//����ļ���������׺
				if ( i == 1 )
				{
					sprintf(pFileName, "%s%d", pFileName, nNum);
					break;
				}

			}//for()ѭ������
			OspLog(LOGPRINTLEVELH, "ͬ���ļ���Ϊ: %s\n", pFileName);			
			nNum++;//rand()���Գ���
		}
	} while (bfilesame);
	OspLog(LOGPRINTLEVELH,"������ͬ���ļ�: %s\n",pFileName);
}

/*==========================================================
������ IsServerSpaceEnough
���� ���жϴ��̷������Ƿ����㹻�ռ�����ϴ��ļ�
�㷨ʵ�֣�<��ѡ��>
����˵��: dwFileSize:�ϴ����ļ��ܴ�С
����ֵ˵����BOOL������ռ��㹻���򷵻�TRUE�����򷵻�FALSE
============================================================*/
BOOL CServerInstance::IsServerSpaceEnough(DWORD dwFileSize)
{
	//��ȡ��ǰ���̴�С
	ULARGE_INTEGER nFreeBytesAvailable;
	GetDiskFreeSpaceEx(_T(g_achServerBase), &nFreeBytesAvailable, 0, 0);
	ULONGLONG dwServerSpace = nFreeBytesAvailable.QuadPart / 1024 / 1024;
	DWORD dwSize = dwFileSize / 1024 / 1024 + 1;	//Bת��ΪM,��1��ֹ���

	//�жϷ�����ʣ��ռ��С
	if ( dwServerSpace > dwSize )
	{
		OspLog(LOGPRINTLEVELH, "�ռ���㣬�����ϴ���ʣ��%d M!\n", dwServerSpace);
		return TRUE;
	}

	OspLog(LOGPRINTLEVELH,"�ռ䲻�㣬ֻʣ: %d M!\n",dwServerSpace);
	return FALSE;
}

/*==========================================================
������ ServerInstanceRelease
���� ���ͷ���Դ
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CServerInstance::ServerInstanceRelease()
{
	//�ͷ���Դ		
	m_phGetFile			= NULL;	                           
	ZeroMemory(m_achPathFileName, MAX_NAME_LEN);
	ZeroMemory(&m_tGetFileInfo, sizeof(UploadFileInfo));
	ZeroMemory(&m_tCGetFileInfo, sizeof(CFileInfo));
}

//end


















