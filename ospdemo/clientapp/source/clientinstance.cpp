/*==============================================================================
ģ����  ���ͻ���ʵ��
�ļ���  ��clientinstance.cpp  
����ļ���clientinstance.h,stdafx.h
ʵ�ֹ��ܣ�ʵ��Instanceͨ�Ź���
����    ��������
��Ȩ    ��<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���        �߶���        �޸ļ�¼
2013/08/08	  V1.0        ������						����
==============================================================================*/
#include "stdafx.h"
#include "clientinstance.h"

CClientInstance::CClientInstance()
{
	//��Ա������ʼ��
	ClientInstanceRelease();						//�ͷ���Դ
}

CClientInstance::~CClientInstance()
{
	ClientInstanceRelease();						//�ͷ���Դ
}

/*==========================================================
������ DaemonInstanceEntry
���� ��app�ػ�ʵ�����
�㷨ʵ�֣�<��ѡ��>
����˵����pMsg:��Ϣ
pcApp:��instance������app
����ֵ˵����void
============================================================*/
void CClientInstance::DaemonInstanceEntry(CMessage *const pMsg, CApp* pcApp)
{
	
}

/*==========================================================
������ InstanceEntry
���� ��appʵ�����
�㷨ʵ�֣�<��ѡ��>
����˵����pMsg:��Ϣ
����ֵ˵����void
============================================================*/
void CClientInstance::InstanceEntry(CMessage *const pMsg)
{
	//�ж���Ϣ�Ƿ�Ϊ��
	if ( pMsg == NULL)
	{
		return;
	}

	//���μ����ϴ���Ϣ��ӡ
	if ( pMsg->event != STOC_UPLOADING_ACK)
	{
		OspLog(LOGPRINTLEVELM, "GET MESSAGE NO.%d!\n", pMsg->event);
	}
		
	switch ( pMsg->event )
	{
	case CTOC_CONN_REQ:											//�յ������������Ϣ
		{
			ClientConnectToServer();
		}
		break;
	case STOC_CONN_ACK:											//�յ���������Ӧ�����Ϣ
		{
			CToSConnectAckManage(pMsg);
		}
		break;
	case CTOC_CONN_DISC:										//�յ��Ͽ����ӵ���Ϣ
		{
			CDisconnectManage();
		}
		break;
	case CTOC_UPLOAD_REQ:										//�յ��ϴ��ļ��������Ϣ
		{
			CUploadFileReqToServer(pMsg);
		}
		break;
	case STOC_UPLOAD_ACK:										//�յ��ϴ�����Ӧ�����Ϣ
		{
			CUploadToSAckManage();
		}
		break;
	case STOC_UPLOAD_NACK:										//�յ��ϴ�����ܾ�����Ϣ
		{
			CUploadToSNackManage(pMsg);
		}
		break;
	case STOC_UPLOADING_ACK:									//�յ������ϴ��ļ�����Ϣ
		{
			CUploadingToSAckManage();
		}
		break;
	case STOC_UPLOADING_NACK:									//�յ��ϴ��ļ��ڿͻ��˴���ʧ�ܵ���Ϣ
		{
			CUploadToSNackManage(pMsg);
		}
		break;
	case CTOC_UPLOADING_PAUSE:                                  //�յ��ϴ���ͣ����Ϣ
		{
			CPauseUploadFileToS();
		}
		break;		
	case CTOC_UPLOADING_RESTART:                                //�յ��ϴ����¿�ʼ����Ϣ
		{
			CRestartUploadFileToS();
		}
		break;		
	case CTOC_FILE_REMOVE:										//�յ���ͣɾ������Ϣ
		{
			CPauseRemoveFileManage();
		}
		break;
	case CTOS_CONNENT_TIMER:									//��ʱ
		{
			CDisconnectManage();
		}
		break;
	case OSP_OVERFLOW:											//����������
		{
			CDisconnectManage();
		}
		break;
	case OSP_DISCONNECT:										//����
		{
			CDisconnectManage();
		}
		break;
	default:
		{
			OspLog(LOGPRINTLEVELH, "GET INSTANCE NONE_SENCE!\n");  
		}
		break;
	}
}

/*==========================================================
������ ClientConnectToServer
���� �����ӵ�������
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
void CClientInstance::ClientConnectToServer()
{
	//�ж�״̬�Ƿ����
	if ( CurState() != STATE_IDLE )
	{
		CB_CToSProgress(C_SEND_FILE_TO_S_COEENCT);					//״̬��ʾ������
		OspLog(LOGPRINTLEVELH, "ClientConnectToServerδ�Ͽ��������!\n"); 
		return;
	}

	if ( !OspIsValidTcpNode(g_nServerNode) )							//�ж�һ������Ƿ���Ч
	{
		NextState(STATE_IDLE);
		OspLog(LOGPRINTLEVELH, "GetAppID.%d GetInsID.%d\n", GetAppID(), GetInsID());
		return;
	}

	OspNodeDiscCBRegQ(g_nServerNode, GetAppID(), GetInsID());			//���ö������
	NextState(STATE_CONNECT);										//���ӽ��ɹ���״̬��ΪSTATE_CONNECT
	//����������������������Ϣ���÷��������ҿ���InstanceӦ��
	OspPost( MAKEIID(SERVER_APP_ID, CInstance::PENDING), CTOS_CONN_REQ, NULL, 0,
		 g_nServerNode, MAKEIID( CLIENT_APP_ID, 1 ), SERVER_APP_ID);
	SetTimer(CTOS_CONNENT_TIMER, 5*1000); 										//���ж�ʱ����5s��ʱ
}

/*==========================================================
������ CToSDisconnectManage
���� ����������Ͽ�ʱ�Ĵ�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CClientInstance::CDisconnectManage()
{
	NextState(STATE_IDLE);
	OspDisconnectTcpNode(g_nServerNode); 									//�Ͽ��������
	ClientInstanceRelease();											//�ͷ���Դ
	g_nServerNode = INVALID_NODE;
	KillTimer(CTOS_CONNENT_TIMER); 
	CB_CToSProgress(C_SEND_FILE_TO_S_DICCOEENCT);						//״̬��ʾ�Ѿ��Ͽ�����
	OspLog(LOGPRINTLEVELH, "CToSDisconnectManage�Ͽ��������!\n");
}

/*==========================================================
������ CToSConnectAckManage
���� ���յ�����Ӧ��ʱ�Ĵ���
�㷨ʵ�֣�<��ѡ��>
����˵����pMsg:�ļ���Ϣ
����ֵ˵����void
============================================================*/
void CClientInstance::CToSConnectAckManage(CMessage *const pMsg)
{
	//�ж���Ϣ�Ƿ�Ϊ��
	if ( pMsg == NULL)
	{
		ClientInstanceRelease();								//�ͷ���Դ
		NextState(STATE_CONNECT);
		CB_CToSProgress(C_SEND_FILE_TO_S_NACKCOEENCT);			//״̬��ʾ����ʧ��
		return;
	}
	KillTimer(CTOS_CONNENT_TIMER); 
	m_nServerID = pMsg->srcid;                                  // ���󷵻�ʱ�����������ID�� 
	CB_CToSProgress(C_SEND_FILE_TO_S_COEENCT);					//״̬��ʾ��������
	NextState(STATE_READY);
	OspLog(LOGPRINTLEVELH, "CToSConnectAckManage�յ�����Ӧ��!\n");
}

/*==========================================================
������ CUploadFileReqToServer
���� ����������˴����ļ�
�㷨ʵ�֣�<��ѡ��>
����˵����pMsg:�ļ���Ϣ
����ֵ˵����void
============================================================*/
void CClientInstance::CUploadFileReqToServer(CMessage *const pMsg)
{
	//�ж���Ϣ�Ƿ�Ϊ��
	if ( pMsg == NULL)
	{
		ClientInstanceRelease();					//�ͷ���Դ
		NextState(STATE_READY);
		CB_CToSProgress(C_SEND_FILE_TO_S_FAIL);	
		OspLog(LOGPRINTLEVELH, "CUploadFileReqToServer:pMsg == NULL!\n"); 
		return;
	}
	//״̬�ж�
	if ( CurState() != STATE_READY )
	{
		ClientInstanceRelease();					//�ͷ���Դ
		CB_CToSProgress(C_SEND_FILE_TO_S_FAIL);		//�ϴ�ʧ��
		OspLog(LOGPRINTLEVELH, "CUploadFileReqToServer:STATE ERROR.%d!\n",CurState());
		NextState(STATE_READY);		
		return;
	}
	memset(&m_tCFileInfo, 0, sizeof(CFileInfo));
	memcpy(&m_tCFileInfo, pMsg->content, pMsg->length);

	//m_ulFileSize = m_tCFileInfo.dwFileSize;			//�����ļ���СΪ��ͣ�ϴ����ж�
	//������Ҫ�����ܰ���
	m_uPacketCount = m_tCFileInfo.dwFileSize / UPLOAD_PAGESIZE;
	m_uLastPacketSize = m_tCFileInfo.dwFileSize % UPLOAD_PAGESIZE;
	if ( m_uLastPacketSize > 0 )
	{
		m_uPacketCount++;								//�������С�����������ܰ�������1		
	}
	//������������ϴ�������Ϣ
	post( m_nServerID, CTOS_UPLOAD_REQ, &m_tCFileInfo, sizeof(m_tCFileInfo), g_nServerNode);
	NextState(STATE_BUSY);	
}

/*==========================================================
������ CUploadToSAckManage
���� ���յ��ϴ��ļ�Ӧ���Ĵ�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CClientInstance::CUploadToSAckManage()
{	
	//�ж��Ƿ�Ϊ�ϴ�״̬
	if ( (CurState() != STATE_BUSY) )
	{
		OspLog(LOGPRINTLEVELH, "CUploadToSAckManage:STATE ERROR.%d!\n",CurState());
		CReadFileErrManage();									//�ϴ��ļ�ʱʧ�ܴ���		
		return;
	}
	CUploadFileToS();											//�����ļ�
}

/*==========================================================
������ CUploadFileToS
���� ���ϴ��ļ�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CClientInstance::CUploadFileToS()
{
	CB_CToSProgress(C_SEND_FILE_TO_S_UPLOADING);				//״̬��ʾ�ϴ��ļ���
	m_pFile = fopen(m_tCFileInfo.achFileName, "rb+");				
	if ( m_pFile == NULL)
	{
		OspLog(LOGPRINTLEVELH, "CUploadFileToS��m_pFile == NULL!\n");		
		CReadFileErrManage();									//�ϴ��ļ�ʱʧ�ܴ���
		return;
	}
	//�ж�fseek�Ƿ�ɹ�
	if ( fseek(m_pFile, m_uCount * UPLOAD_PAGESIZE, SEEK_SET) != 0 )
	{
		OspLog(LOGPRINTLEVELH, "CUploadFileToS��fseek error!\n");
		CReadFileErrManage();									//�ϴ��ļ�ʱʧ�ܴ���
		return;
	}
	//�ж�fread�Ƿ�ɹ�
	if ( fread(m_tUploadFileInfo.achData, 1, UPLOAD_PAGESIZE, m_pFile) == 0 )//��ȡһ���ݰ�������
	{
		OspLog(LOGPRINTLEVELH, "CUploadFileToS��fread error!\n");
		CReadFileErrManage();									//�ϴ��ļ�ʱʧ�ܴ���
		return;
	}
	fclose(m_pFile);
	m_uCount++;

	static s8 chProgress = 0;									//��¼�ϴ����ȣ����ʹ�ӡƵ��ʱ��
	//�ж��ϴ������ݰ��ܴ�С�Ƿ񳬹��ļ���С
	if ( m_uCount != m_uPacketCount )
	{
		//��¼�ϴ�����
		m_dProgress = m_uCount * 100 / m_uPacketCount;		
		//���ʹ�ӡƵ��
		if ( chProgress != (s8)m_dProgress )
		{
			OspLog(LOGPRINTLEVELL, "�ѷ���: %d%%\n", chProgress);
			chProgress = (s8)m_dProgress;
		}
		m_tUploadFileInfo.nDataLen = UPLOAD_PAGESIZE;			//��¼�ϴ������ϴ�
		CB_CToSProgress(chProgress);							//״̬��ʾ����
		//�����������һ�����ݰ�����������ϴ�
		post(m_nServerID, CTOS_UPLOADING_REQ, &m_tUploadFileInfo, sizeof(UploadFileInfo), g_nServerNode);
	}
	else
	{
		//�������һ������С
		m_tUploadFileInfo.nDataLen = m_uLastPacketSize;		
		//�����������һ�����ݰ�
		post(m_nServerID, CTOS_UPLOADING_REQ, &m_tUploadFileInfo, sizeof(UploadFileInfo), g_nServerNode);
		CB_CToSProgress(C_SEND_FILE_TO_S_OVER);					//״̬��ʾ�ѷ������
		ClientInstanceRelease();								//�ͷ���Դ
		chProgress = 0;
		NextState(STATE_READY);
		OspLog(LOGPRINTLEVELH,"�ѷ������: 100%%!\n");
	}//if ( m_uCount != m_uPacketCount )
}

/*==========================================================
������ CUploadingToSAckManage
���� �������ϴ�δ��ɵ��ļ����󱻽��ܺ�Ĵ�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CClientInstance::CUploadingToSAckManage()
{
	if ( CurState() == STATE_PAUSE )	//��ͣ״̬
	{
		OspLog(LOGPRINTLEVELH,"CUploadingToSAckManage:STATE_PAUSE!\n");
		return;
	}

	CUploadToSAckManage();				//�����ϴ�								
}

/*==========================================================
������ CUploadToSNackManage
���� ���ϴ����󱻾ܾ��Ĵ���
�㷨ʵ�֣�<��ѡ��>
����˵����pMsg:�ļ���Ϣ
����ֵ˵����void
============================================================*/
void CClientInstance::CUploadToSNackManage(CMessage *const pMsg)
{
	s8 eErrCode;
	memcpy(&eErrCode, pMsg->content, pMsg->length);
	CReadFileErrManage();									//�ϴ��ļ�ʱʧ�ܴ���
	OspLog(LOGPRINTLEVELH, "CUploadToSNackManage��eErrCode.%d!\n", eErrCode);
}

/*==========================================================
������ CReadFileErrManage
���� ���ϴ��ļ���������
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CClientInstance::CReadFileErrManage()
{
	//����������Ͷ��ļ�ʧ����Ϣ
	post( m_nServerID, CTOS_UPLOAD_FREADERR, NULL, 0, g_nServerNode);
	ClientInstanceRelease();								//�ͷ���Դ
	NextState(STATE_READY);
	CB_CToSProgress(C_SEND_FILE_TO_S_FAIL);					//״̬��ʾ�ϴ�ʧ��
}

/*==========================================================
������ CPauseUploadFileToS
���� ����ͣ�ϴ��ļ��Ĵ�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CClientInstance::CPauseUploadFileToS()
{
	//�ж��Ƿ��Ѿ���ͣ
	if ( CurState() == STATE_PAUSE )
	{
		OspLog(LOGPRINTLEVELH,"CPauseUploadFileToS:STATE_PAUSE!\n");
		CB_CToSProgress(C_SEND_FILE_TO_S_PAUSE);			//��ͣ��
		return;
	}

	//�ж��Ƿ��ϴ�״̬
	if ( CurState() != STATE_BUSY )
	{
		OspLog(LOGPRINTLEVELH, "CPauseUploadFileToS:STATE ERROR.%d!\n", CurState());
		CReadFileErrManage();		
		return;
	}

	//�ж��Ƿ�������ͣ����
	if ( (m_tCFileInfo.dwFileSize / 1024 / 1024) < PAUSESIZE )
	{
		CB_CToSProgress(C_SEND_FILE_TO_S_NOPAUSE);			//�����ļ�С��50M����������ͣ
		OspLog(LOGPRINTLEVELH, "�ϴ��ļ�С��%dM,��������ͣ!\n",PAUSESIZE); 
		return;
	}

	CB_CToSProgress(C_SEND_FILE_TO_S_PAUSE);				//״̬��ʾ��ͣ��
	NextState(STATE_PAUSE);
	OspLog(LOGPRINTLEVELH,"ע���ϴ��ѱ���ͣ!\n");
}

/*==========================================================
������ CPauseRemoveFileManage
���� ����ͣ�ϴ�ʱɾ���ļ��Ĵ�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CClientInstance::CPauseRemoveFileManage()
{
	//�ϴ�״̬������ɾ���б��ļ�
	if ( CurState() == STATE_BUSY )
	{
		CB_CToSProgress(C_SEND_FILE_TO_S_BUSYNODEL);		
		OspLog(LOGPRINTLEVELH, "�ϴ�״̬,����ɾ��!\n"); 
		return;
	}

	//����ͣ״̬������ɾ��
	if(CurState() != STATE_PAUSE)
	{
		OspLog(LOGPRINTLEVELH, "����ͣ״̬,�ɷ���ɾ��!\n"); 
		return;
	}

	//�������������ͣʱ��ɾ���б��ļ�����Ϣ
	post( m_nServerID, CTOS_UPLOAD_FREADERR, NULL, 0, g_nServerNode);
	ClientInstanceRelease();								//�ͷ���Դ
	NextState(STATE_READY);
	OspLog(LOGPRINTLEVELH, "�ϴ��ļ��Ѿ�ɾ��!\n");
}

/*==========================================================
������ CRestartUploadFileToS
���� ����ͣ�������ϴ��ļ��Ĵ�����
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CClientInstance::CRestartUploadFileToS()
{
	//�ж��Ƿ���ͣ״̬
	if ( CurState() != STATE_PAUSE )
	{
		OspLog(LOGPRINTLEVELH, "����ͣ״̬��������ЧSTATE ERROR.%d!\n", CurState()); 
		return;
	}

	//�л�״̬Ϊ�ϴ�״̬
	NextState(STATE_BUSY);
	OspLog(LOGPRINTLEVELH, "�ļ��ϴ��Ѿ��ָ�!\n"); 
	//���������ļ�
	CUploadFileToS();
}

/*==========================================================
������ ClientInstanceRelease
���� ���ͷ���Դ
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵����void
============================================================*/
void CClientInstance::ClientInstanceRelease()
{
	//��ʼ����Ա
	m_pFile			= NULL;													
	m_dProgress		= 0;
	m_uCount		= 0;								
	m_uPacketCount	= 0;							
	m_uLastPacketSize=0;
	ZeroMemory(&m_tCFileInfo, sizeof(CFileInfo));	
	ZeroMemory(&m_tUploadFileInfo, sizeof(UploadFileInfo));				
}

//end



