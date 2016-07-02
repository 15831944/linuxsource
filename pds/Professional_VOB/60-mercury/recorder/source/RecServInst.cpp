/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : RecServInst.cpp
   ����ļ�    : RecApp.cpp
   �ļ�ʵ�ֹ���: Recorder Serverʵ��
   ����        : 
   �汾        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2010/08/26  1.0         �� ��        ����
******************************************************************************/

#include "RecServInst.h"
#include "ConfAgent.h"
//#include "mcuver.h"
#include "ChnInst.h"
#include "StateBase.h"


// �Զ���ȫ�ֱ���, ¼�����������App����
CRecServApp g_RecServApp;


/*===================================================================
  �� �� ���� CRecServInst
  ��    �ܣ� CRecServInst�Ĺ��캯��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� ��
  �� �� ֵ�� ��
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
CRecServInst::CRecServInst() : m_pcStateMgr(NULL)
{
	m_pcStateMgr = new CStateMgr(this);
	memset(&m_cServMsg, 0, sizeof(m_cServMsg));
	memset(&m_tMTNodeInfo, 0, sizeof(m_tMTNodeInfo));
	memset(&m_tRecNodeInfo, 0, sizeof(m_tRecNodeInfo));
}

/*===================================================================
  �� �� ���� ~CRecServInst
  ��    �ܣ� CRecServInst�����캯��
  �㷨ʵ�֣� (1).�ͷ�������Դ
  ȫ�ֱ����� 
  ��    ���� ��
  �� �� ֵ�� ��
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
CRecServInst::~CRecServInst()
{
	if (m_pcStateMgr)
	{
		delete m_pcStateMgr;
		m_pcStateMgr = NULL;
	}
}


/*===================================================================
  �� �� ���� DaemonInstanceEntry
  ��    �ܣ� CRecServInst���ػ�ʵ����ں���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg = ��Ϣ; pcApp = CApp
  �� �� ֵ�� ��
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
void CRecServInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp)
{
    switch (pcMsg->event)
    {
	// �ն�ע������
    case MT_RECSERV_REG_REQ:
        OnMTRegisterReq(pcMsg, pcApp);
        break;

	case REC_MCU_EXCPT_NOTIF:
	case REC_MCU_RECSTATUS_NOTIF:
		OnDaemNodifyEvent(pcMsg, pcApp);
		break;

	case EV_RECSERV_SHOW_STATUS:
		OnDaemShowStatus(pcMsg, pcApp);
		break;

	// ¼����ϵ���Ϣ
	case EV_REC_POWERON:
		break;
     
    default:
        break;
    }
}

/*===================================================================
  �� �� ���� InstanceEntry
  ��    �ܣ� CRecServInst����ͨʵ����ں���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg = ��Ϣ; pcApp = CApp
  �� �� ֵ�� ��
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
void CRecServInst::InstanceEntry(CMessage * const pcMsg)
{
    const u16 wEventId = pcMsg->event;

    switch (wEventId)
    {
	// �ն˵���֪ͨ
    case OSP_DISCONNECT:
        OnDisconnectMsg(pcMsg);
        break;

    default:
		if (m_pcStateMgr)
		{
			m_pcStateMgr->OnMessage(pcMsg);
		}
        break;
    }
}

/*===================================================================
  �� �� ���� OnMTRegisterReq
  ��    �ܣ� �ն�����ע��ʱCRecServInst��Ϊ֮����һ�����е�instance
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg = ��Ϣ; pcApp = CApp
  �� �� ֵ�� ��
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
void CRecServInst::OnMTRegisterReq(CMessage * const pMsg, CApp* pcApp)
{
	// ��ȡһ��CRecServInst�Ŀ���ʵ��
    CRecServInst *pInstance = (CRecServInst *)GetIdleInst(pcApp);

	// ��ȡ����ʵ��ʧ��
    if (!pInstance)
    {
		RecServLog("RecServer is busy now, refused the register from (%d, %d)\n", pMsg->srcnode, pMsg->srcid);

        // ֪ͨ�ն˷�������æ
        TErrorInfo tErro;
        tErro.m_wErrCode = ERR_REC_SERVER_BUSY;

        OspPost(pMsg->srcid, 
            RECSERV_MT_REG_NACK,
            &tErro,
            sizeof(tErro),
            pMsg->srcnode
        );

        return;
    }

    // ע�����
    OspSetHBParam(pMsg->srcnode, 20, 20);
	// �����Ĵ���Ϊ��Ҫ���մ˴�ע���CRecServInst
    OspNodeDiscCBReg(pMsg->srcnode, pInstance->GetAppID(), pInstance->GetInsID());

	// ״̬�û�
    pInstance->NextState(E_STATE_REC_ONREG);
	
	// �����ն˽ڵ���Ϣ
	TNodeInfo tMtNode = {0};
	tMtNode.m_dwAppId = pMsg->srcid;
	tMtNode.m_dwNodeId = pMsg->srcnode;
    pInstance->SetMTNodeInfo(tMtNode);

	// ���ն�ע����Ϣת�������CRecServInst������
    OspPost(MAKEIID(pInstance->GetAppID(), pInstance->GetInsID()), 
        pMsg->event,
        pMsg->content,
        pMsg->length);
}

/*===================================================================
  �� �� ���� OnDaemNodifyEvent
  ��    �ܣ� �ػ�ʵ����ת����¼�����Nodity��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg = ��Ϣ; pcApp = CApp
  �� �� ֵ�� ��
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
void CRecServInst::OnDaemNodifyEvent(CMessage * const pMsg, CApp* pcApp)
{
    CRecServInst* pInstance = NULL;
    for (u16 iCounter = 1; iCounter <= pcApp->GetInstanceNumber(); iCounter++)
    {
        pInstance = (CRecServInst*)pcApp->GetInstance(iCounter);
        ASSERT(pInstance != NULL);
        
        // pInstance��״̬ʱIdle, �Ҳ���רΪRecorderע���õ��Ǹ�instance
        if ( pInstance && E_STATE_REC_IDLE != pInstance->GetState())
        {
			// ���ն�ע����Ϣת�������CRecServInst������
			OspPost(MAKEIID(pInstance->GetAppID(), pInstance->GetInsID()), pMsg->event, pMsg->content, pMsg->length);
        }
    }
}

/*===================================================================
  �� �� ���� OnDaemShowStatus
  ��    �ܣ� ��ӡ¼�����������״̬��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg = ��Ϣ; pcApp = CApp
  �� �� ֵ�� ��
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
void CRecServInst::OnDaemShowStatus(CMessage * const pMsg, CApp* pcApp)
{
	// ����pApp, �ҵ�һ�����е�Instance.
	RecServLog("\n=====================================================================\n");
	RecServLog("\nThe status of CRecServInst are shown below:\n");
    CInstance* pInstance = NULL;
    for (u16 iCounter = 1; pcApp && iCounter <= pcApp->GetInstanceNumber(); iCounter++)
    {
        pInstance = pcApp->GetInstance(iCounter);
        if (!pInstance) break;
        
		// ���ն�ע����Ϣת�������CRecServInst������
		OspPost(MAKEIID(pInstance->GetAppID(), pInstance->GetInsID()), pMsg->event);
    }
}


/*===================================================================
  �� �� ���� OnDisconnectMsg
  ��    �ܣ� ¼�����������⵽�ն˵���ʱ�Ĵ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg
  �� �� ֵ�� ��
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
void CRecServInst::OnDisconnectMsg(CMessage * const pcMsg)
{
	// �ж���Ϣ�����Ƿ�Ϸ�
    if ( NULL == pcMsg->content )
    {
        RecServLog("[CRecServInst][Error]Ins%d received error msg\n", GetInsID());
        return;
    }
   
	// �ж���Ϣ�����Ƿ���ȷ
    u32 dwNodeId = *(u32*)pcMsg->content;
	if (dwNodeId == m_tMTNodeInfo.m_dwNodeId)
    {
		// �����Ͽ�
        OspDisconnectTcpNode(dwNodeId);
        OspPrintf(TRUE, FALSE, "[CRecServInst][Error]Node%d disconnect, then reset!\n", m_tMTNodeInfo.m_dwNodeId);

		// ֪ͨ¼�����ֹ¼��/�����ͷ�ͨ��
        PostMsgToRecChnInst(EV_MT_DISCONNECTED, NULL, 0);

		// ���m_cServMsg����Ϣͷ
        memset(&m_cServMsg, 0, sizeof(m_cServMsg));		
		// ��ʼ��״̬
        NextState(E_STATE_REC_IDLE);
    }
}


/*===================================================================
  �� �� ���� GetIdleInst
  ��    �ܣ� ��pcApp�л�ȡһ������ʵ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcApp
  �� �� ֵ�� CInstance*����
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
CRecServInst* CRecServInst::GetIdleInst(CApp* pcApp) 
{
    // ����pApp, �ҵ�һ�����е�Instance.
    CRecServInst* pInstance = NULL;
    for (u16 iCounter = 1; iCounter <= pcApp->GetInstanceNumber(); iCounter++)
    {
        pInstance = (CRecServInst *)pcApp->GetInstance(iCounter);
        if(pInstance == NULL) break;
        
        // pInstance��״̬ʱIdle, �Ҳ���רΪRecorderע���õ��Ǹ�instance
        if (E_STATE_REC_IDLE == pInstance->GetState())
        {
             return pInstance;
        }
    }

    return NULL;
}

/*===================================================================
  �� �� ���� PostMsgToMt
  ��    �ܣ� ����Ϣ�����Ѿ�ע����ն�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
			wEvent   : �¼���
			pContent : �¼�����
			dwLength : ���ݳ���
  �� �� ֵ�� RET_TYPE,�����RET_TYPE
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecServInst::PostMsgToMt(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	RecServLevelLog(RECSERV_LEVEL_LOG_BELOW, 
		"CRecServInst[%d] post %s to mt[%d]\n", 
		GetInsID(), 
		OspEventDesc(wEvent), 
		m_tMTNodeInfo.m_dwNodeId);

	OspPost(m_tMTNodeInfo.m_dwAppId, 
		wEvent, 
		pContent, 
		dwLength, 
		m_tMTNodeInfo.m_dwNodeId, 
		GETLOCALAPPID);
	
	return RET_DONE;
}


/*===================================================================
  �� �� ���� PostMsgToRecDaemInst
  ��    �ܣ� ����Ϣ����¼������ػ�ʵ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
			wEvent   : �¼���
			pContent : �¼�����
			dwLength : ���ݳ���
  �� �� ֵ�� RET_TYPE,�����RET_TYPE
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecServInst::PostMsgToRecDaemInst(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	memset(&m_cServMsg, 0, sizeof(m_cServMsg));
	m_cServMsg.SetMsgBody(const_cast<u8 *>(pContent), dwLength);

	OspPost(MAKEIID(AID_RECORDER, CInstance::DAEMON), 
		wEvent, 
		m_cServMsg.GetServMsg(), 
		m_cServMsg.GetServMsgLen(), 
		m_tRecNodeInfo.m_dwNodeId, 
		GETLOCALAPPID);
	
	return RET_DONE;
}


/*===================================================================
  �� �� ���� PostMsgToRecChnInst
  ��    �ܣ� ����Ϣ����¼�����¼��ͨ��
  ��    ��:  ��ͨ������¼��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
			wEvent   : �¼���
			pContent : �¼�����
			dwLength : ���ݳ���
  �� �� ֵ�� RET_TYPE,�����RET_TYPE
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecServInst::PostMsgToRecChnInst(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	memset(&m_cServMsg, 0, sizeof(m_cServMsg));
	m_cServMsg.SetMsgBody(const_cast<u8 *>(pContent), dwLength);

	OspPost(m_tRecNodeInfo.m_dwAppId, 
		wEvent, 
		m_cServMsg.GetServMsg(), 
		m_cServMsg.GetServMsgLen(), 
		m_tRecNodeInfo.m_dwNodeId, 
		GETLOCALAPPID);
	
	return RET_DONE;
}


/*===================================================================
  �� �� ���� IsMtReqValid
  ��    �ܣ� ��֤�ն˵������Ƿ�Ϸ�
  ��    ��:  ���ն��Ѿ�ע��
  �㷨ʵ�֣� (1). �����ϢԴ����ע���ն���Ϣ������,�����ϢΪ�Ƿ���Ϣ
  ȫ�ֱ����� 
  ��    ���� pcMsg   : CMessage
  �� �� ֵ�� �Ϸ�����True,����False
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
BOOL32 CRecServInst::IsMtReqValid(CMessage * const pcMsg)
{
	// ��֤��Ϣ��Դ�Ƿ�Ϸ�
	if (m_tMTNodeInfo.m_dwNodeId != pcMsg->srcnode
		|| m_tMTNodeInfo.m_dwAppId != pcMsg->srcid)
	{
		RecServLog("The Mt[%d, %d] != MyMt[%d, %d], it's invalid!\n", pcMsg->srcnode, pcMsg->srcid, m_tMTNodeInfo.m_dwNodeId, m_tMTNodeInfo.m_dwAppId);
		return FALSE;
	}
	
	return TRUE;
}


/*===================================================================
  �� �� ���� IsRecMsgValid
  ��    �ܣ� ��֤¼�������Ϣ�Ƿ�Ϸ�
  �㷨ʵ�֣� (1). �����Ϣ������¼������ػ�ʵ��,��Ϸ�
			 (2). ������ػ�ʵ��,����Ҫ��֤����ϢԴ�Ƿ����Ѿ���¼��¼��ͨ����Ϣ�����
  ȫ�ֱ����� 
  ��    ���� pcMsg   : CMessage
  �� �� ֵ�� �Ϸ�����True,����False
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
BOOL32 CRecServInst::IsRecMsgValid(CMessage * const pcMsg)
{
	// �����Daemonʵ������Ϣ, �ظ�TRUE
	if (pcMsg->srcnode == m_tRecNodeInfo.m_dwNodeId && DAEMON == GETINS(pcMsg->srcid))
	{
		return TRUE;
	}

	// ��Daemonʵ����Ϣ,��Ҫ�ж�
	if (pcMsg->srcnode != m_tRecNodeInfo.m_dwNodeId || pcMsg->srcid != m_tRecNodeInfo.m_dwAppId)
	{
		RecServLog("The Chanel[%d, %d] != MyChn[%d, %d], it's invalid!\n", pcMsg->srcnode, pcMsg->srcid, m_tRecNodeInfo.m_dwNodeId, m_tRecNodeInfo.m_dwAppId);
		return FALSE;
	}
	
	return TRUE;
}

