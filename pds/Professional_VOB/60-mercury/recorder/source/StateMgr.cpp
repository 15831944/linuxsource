/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : StateMgr.cpp
   ����ļ�    : State.cpp, State.cpp, RecServInst.cpp��
   �ļ�ʵ�ֹ���: Recorder Server��״̬��ʵ��
   ����        : 
   �汾        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2010/08/26  1.0         �� ��        ����
******************************************************************************/

#include "StateMgr.h"
#include "StateBase.h"
#include "State.h"
#include "RecServInst.h"


/*===================================================================
  �� �� ���� CStateMgr
  ��    �ܣ� CStateMgr�Ĺ��캯��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pInst : CRecServInst��ʵ��ָ��
  �� �� ֵ��
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
CStateMgr::CStateMgr(CRecServInst * const pInst) : m_pServInst(pInst), m_pcState(NULL)
{
	m_pcState = new CIdleState(this);

	// �жϳ�ʼ���Ƿ�ɹ�
	if (!m_pcState || !m_pServInst)
	{
		RecServLog("[Error]CStateMgr initial failed!\n");
	}
}


/*===================================================================
  �� �� ���� CStateMgr
  ��    �ܣ� CStateMgr�����캯��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ��
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
CStateMgr::~CStateMgr()
{
	if (m_pcState)
	{
		m_pcState->Exit();
		delete m_pcState;
		m_pcState = NULL;
	}
}


/*===================================================================
  �� �� ���� CStateMgr
  ��    �ܣ� CStateMgr��״̬�л�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����dwState : ��״̬��enumֵ
  �� �� ֵ��RET_TYPE,�����RET_TYPE����
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateMgr::ChangeStateTo(const u32 &dwState)
{
    if (m_pcState)
    {
		// ���Ҫ�л���״̬�ǵ�ǰ״̬,ֱ�ӷ���false
        if (m_pcState->GetState() == dwState)
        {
            return RET_PASSED;
        }else			// �������˳���״̬
        {
            m_pcState->Exit();
            delete m_pcState;
            m_pcState = NULL;
        }
    }
	
	// ����״ֵ̬��̬����m_pcState
    switch (dwState)
    {
	case E_STATE_REC_IDLE:							// ����
		m_pcState = new CIdleState(this);
		break;
	case E_STATE_REC_ONREG:							// ����׼������ע��
		m_pcState = new COnRegisterState(this);
		break;
	case E_STATE_REC_READY:							// ¼��׼����
		m_pcState = new CRecReadyState(this);
		break;
	case E_STATE_RECORDING:							// ����¼��
		m_pcState = new CRecordingState(this);
		break;
	case E_STATE_REC_PAUSE:							// ¼����ͣ״̬
		m_pcState = new CRecPauseState(this);
		break;
    default:
        break;
    }
	
	// ��֤m_pcState�Ƿ񴴽��ɹ�
    if (!m_pcState)
    {
        RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "ChangeStateTo to state %d failed, please check the value\n ", dwState);
        return RET_FAILED;
    }
	
	// ״̬��ʼ
    m_pcState->Enter();
    return RET_DONE;
}

/*===================================================================
  �� �� ���� OnMessage
  ��    �ܣ� CStateMgr����Ϣ��ں���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����dwState : CMessage*
  �� �� ֵ��RET_TYPE,�����RET_TYPE����
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateMgr::OnMessage(CMessage *const pcMsg)
{
    if (m_pcState)
    {
        return m_pcState->OnMessage(pcMsg);
    }
    
    RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr hasn't been initial success!\n");
    return RET_FAILED;
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
RET_TYPE CStateMgr::PostMsgToMt(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	if (GetOwnInst())
	{
		return GetOwnInst()->PostMsgToMt(wEvent, pContent, dwLength);
	}

	// instanceΪ��,��ʾ����
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
	return RET_FAILED;
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
RET_TYPE CStateMgr::PostMsgToRecDaemInst(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	if (GetOwnInst())
	{
		return GetOwnInst()->PostMsgToRecDaemInst(wEvent, pContent, dwLength);
	}

	// instanceΪ��,��ʾ����	
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
	return RET_FAILED;
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
RET_TYPE CStateMgr::PostMsgToRecChnInst(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	if (GetOwnInst())
	{
		return GetOwnInst()->PostMsgToRecChnInst(wEvent, pContent, dwLength);
	}
	
	// instanceΪ��,��ʾ����
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
	return RET_FAILED;
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
BOOL32 CStateMgr::IsMtReqValid(CMessage * const pcMsg)
{
	if (GetOwnInst())
	{
		return GetOwnInst()->IsMtReqValid(pcMsg);
	}
	
	// instanceΪ��,��ʾ����
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
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
BOOL32 CStateMgr::IsRecMsgValid(CMessage * const pcMsg)
{
	if (GetOwnInst())
	{
		return GetOwnInst()->IsRecMsgValid(pcMsg);
	}
	
	// instanceΪ��,��ʾ����	
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
	return TRUE;
}

/** 
/*	���ܣ���ȡ״̬����ǰ״̬
/*  ������
/*  ����ֵ����ǰ״̬��enumֵ
*/
const u32 CStateMgr::GetState()
{
	if (m_pcState)
	{
		return m_pcState->GetState();
	}else
	{
		return E_STATE_REC_INVALID;
	}
}

/** 
/*	���ܣ��洢¼����ڵ���Ϣ
/*  ������
/*       dwNodeId : �ڵ��
/*       dwInstId : mt��Instance�ţ���MAKEID(appid��instanceid)����
/*  ����ֵ����
*/
void CStateMgr::SetRecNodeInfo(const TNodeInfo& tNode)
{
	if (GetOwnInst())
	{
		GetOwnInst()->SetRecNodeInfo(tNode);
		return;
	}

	// instanceΪ��,��ʾ����	
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
}

/** 
/*	���ܣ��洢�ն˽ڵ���Ϣ
/*  ������
/*       dwNodeId : �ڵ��
/*       dwInstId : mt��Instance�ţ���MAKEID(appid��instanceid)����
/*  ����ֵ����
*/
void CStateMgr::SetMTNodeInfo(const TNodeInfo& tNode)
{
	if (GetOwnInst())
	{
		GetOwnInst()->SetMTNodeInfo(tNode);
		return;
	}

	// instanceΪ��,��ʾ����	
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
}
