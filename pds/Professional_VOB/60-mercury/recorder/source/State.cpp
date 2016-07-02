/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : State.cpp
   ����ļ�    : StateMgr.cpp, StateBase.cpp��
   �ļ�ʵ�ֹ���: Recorder Server��״̬��ʵ��
   ����        : 
   �汾        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2010/08/26  1.0         �� ��        ����
******************************************************************************/

#include "State.h"
#include "ChnInst.h"

/*===================================================================
  �� �� ���� Enter
  ��    �ܣ� CIdleState�����ĳ�ʼ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� ��
  �� �� ֵ�� ����True
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
BOOL32 CIdleState::Enter()
{
	// ״̬��0
	TNodeInfo tNodeInfo = {0};
	GetStateMgr()->SetRecNodeInfo(tNodeInfo);
	GetStateMgr()->SetMTNodeInfo(tNodeInfo);
	
	return TRUE;
}


/*===================================================================
  �� �� ���� OnMessage
  ��    �ܣ� COnRegisterState����Ϣ���պ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE COnRegisterState::OnMessage(CMessage *const pcMsg)
{
	switch (pcMsg->event)
	{
	case MT_RECSERV_REG_REQ:
		return OnMtRegReq(pcMsg);

	case RECSERV_MT_REG_ACK:
		return OnMtRegReqAck(pcMsg);

	case RECSERV_MT_REG_NACK:
		return OnMtRegReqNack(pcMsg);
		
	default:
		return CStateBase::OnMessage(pcMsg);
	}
}


/*===================================================================
  �� �� ���� OnMtRegReq
  ��    �ܣ� COnRegisterState״̬ʱ��Ӧ�ն�ע������ĺ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE COnRegisterState::OnMtRegReq(CMessage *const pcMsg)
{
	// ��֤��Ϣ���Ƿ�Ϸ�
    if (sizeof(TMtRegReq) > pcMsg->length)
    {
		// ���������Ϣ
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "Register infomation is invalid\n");

		// �ظ�Nack
		TErrorInfo tError = {0};
		tError.m_wErrCode = ERR_REC_SERVER_ERR;
		GetStateMgr()->PostMsgToMt(RECSERV_MT_REG_NACK, (u8 *)&tError, sizeof(tError));

		ChangeStateTo(E_STATE_REC_IDLE);
        return RET_FAILED;
    }
	
	// ��¼�������ע��
	if(RET_DONE != GetStateMgr()->PostMsgToRecDaemInst(pcMsg->event, pcMsg->content, pcMsg->length))
	{
		// ��¼���������Ϣʧ��,�ظ�Nack
		TErrorInfo tError = {0};
		tError.m_wErrCode = ERR_REC_SERVER_ERR;
		GetStateMgr()->PostMsgToMt(pcMsg->event + 2, (u8 *)&tError, sizeof(tError));
		ChangeStateTo(E_STATE_REC_IDLE);
	}

	return RET_DONE;
}

/*===================================================================
  �� �� ���� OnMtRegReqAck
  ��    �ܣ� COnRegisterState״̬ʱ¼����ظ�ע��Ack����Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE COnRegisterState::OnMtRegReqAck(CMessage *const pcMsg)
{
	if(RET_DONE != OnMtReqAck(pcMsg))
	{
		ChangeStateTo(E_STATE_REC_IDLE);
		return RET_FAILED;
	}

	return ChangeStateTo(E_STATE_REC_READY);
}


/*===================================================================
  �� �� ���� OnMtRegReqNack
  ��    �ܣ� COnRegisterState״̬ʱ¼����ظ�ע��Nack����Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE COnRegisterState::OnMtRegReqNack(CMessage *const pcMsg)
{
	if(RET_DONE != OnMtReqNack(pcMsg))
	{
		ChangeStateTo(E_STATE_REC_IDLE);
		return RET_FAILED;
	}

	return ChangeStateTo(E_STATE_REC_IDLE);
}

/*===================================================================
  �� �� ���� Enter
  ��    �ܣ� CRecReadyState״̬ʱ��ڴ�����
  �㷨ʵ�֣� (1). ��¼��ͨ����Ϣ��0
  ȫ�ֱ����� 
  ��    ���� ��
  �� �� ֵ�� ����True 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
BOOL32 CRecReadyState::Enter()
{
	// ¼��ͨ��״̬��0
	TNodeInfo tRecNode = {0};
	GetStateMgr()->SetRecNodeInfo(tRecNode);

	return TRUE;
}

/*===================================================================
  �� �� ���� OnMessage
  ��    �ܣ� CRecReadyState״̬ʱ����Ϣ���պ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecReadyState::OnMessage(CMessage *const pcMsg)
{
	switch (pcMsg->event)
	{
	case MCU_REC_STARTREC_REQ:
		return OnMtStartRecReq(pcMsg);

	case REC_MCU_STARTREC_ACK:
		return OnMtStartRecAck(pcMsg);

	case REC_MCU_STARTREC_NACK:
		return OnMtReqNack(pcMsg);

	case MCU_REC_STOPREC_REQ:
		return ReplyMtError(REC_MCU_STOPREC_NACK, ERR_REC_CURDOING);

	case MCU_REC_RESUMEREC_REQ:
		return ReplyMtError(REC_MCU_RESUMEREC_NACK, ERR_REC_NOTRECORD);

	case MCU_REC_PAUSEREC_REQ:
		return ReplyMtError(REC_MCU_PAUSEREC_NACK, ERR_REC_NOTRECORD);
		
	default:
		return CStateBase::OnMessage(pcMsg);
	}
}


/*===================================================================
  �� �� ���� OnMtStartRecAck
  ��    �ܣ� CRecReadyState״̬ʱ¼����ظ���ʼ¼��Ackʱ����Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecReadyState::OnMtStartRecAck(CMessage *const pcMsg)
{
	// ����¼���ͨ���Ľڵ���Ϣ
	TNodeInfo tRecNode = {0};
	tRecNode.m_dwAppId = pcMsg->srcid;
	tRecNode.m_dwNodeId = pcMsg->srcnode;
	GetStateMgr()->SetRecNodeInfo(tRecNode);

	// ���ն˻ظ�Ack
	OnMtReqAck(pcMsg);
	
	// �л�״̬��¼��״̬
	return ChangeStateTo(E_STATE_RECORDING);
}


/*===================================================================
  �� �� ���� OnMessage
  ��    �ܣ� CRecordingState״̬ʱ����Ϣ���պ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecordingState::OnMessage(CMessage *const pcMsg)
{
	switch (pcMsg->event)
	{
	case MCU_REC_STARTREC_REQ:
		return OnMtStartRecReq(pcMsg);
	
	case MCU_REC_PAUSEREC_REQ:
		return OnMtPauseRecReq(pcMsg);
		
	case REC_MCU_PAUSEREC_ACK:
		OnMtReqAck(pcMsg);
		return ChangeStateTo(E_STATE_REC_PAUSE);
		
	case REC_MCU_PAUSEREC_NACK:
		return OnMtReqNack(pcMsg);
		
	case MCU_REC_STOPREC_REQ:
		return OnMtStopRecReq(pcMsg);
		
	case REC_MCU_STOPREC_ACK:
		OnMtReqAck(pcMsg);
		return ChangeStateTo(E_STATE_REC_READY);
		
	case REC_MCU_STOPREC_NACK:
		return OnMtReqNack(pcMsg);
	
	case MCU_REC_RESUMEREC_REQ:
		// ״̬����,�ظ�Nack
		return ReplyMtError(REC_MCU_RESUMEREC_NACK, ERR_REC_CURDOING);
	
	case REC_MCU_NEEDIFRAME_CMD:
		return OnRecNeedIFrameCmd(pcMsg);
	
	case REC_MCU_RECORDPROG_NOTIF:
		return OnNodifyEvent(pcMsg);
		
	default:
		return CStateBase::OnMessage(pcMsg);
	}
}


/*===================================================================
  �� �� ���� OnMtStartRecReq
  ��    �ܣ� CRecordingState״̬ʱ���ն�����¼�����Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecordingState::OnMtStartRecReq(CMessage *const pcMsg)
{
	// ��֤��Ϣ��Դ�Ƿ�Ϸ�
	if( !(GetStateMgr()->IsMtReqValid(pcMsg)) )
	{
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "Mt request to Pause Rec failed, for invalid request\n");
		TErrorInfo tError = {0};
		tError.m_wErrCode = ERR_REC_SERVER_INVALID;
		OspPost(pcMsg->srcid, pcMsg->event + 2, &tError, sizeof(tError), pcMsg->srcnode);
		
		return RET_FAILED;
	}
	
	// ��¼���������ͣ¼��
	return ReplyMtError(REC_MCU_STARTREC_NACK, ERR_REC_CURDOING);
}


/*===================================================================
  �� �� ���� OnRecNeedIFrameCmd
  ��    �ܣ� CRecordingState״̬ʱ��¼�������ؼ�֡����Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecordingState::OnRecNeedIFrameCmd(CMessage *const pcMsg)
{
	// ��֤��Ϣ��Դ�Ƿ�Ϸ�
	if( !(GetStateMgr()->IsRecMsgValid(pcMsg)) )
	{
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "Rec request to get IFrame failed, for invalid request\n");
		return RET_FAILED;
	}

	// ��¼�������ֹͣ¼��
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	return GetStateMgr()->PostMsgToMt(pcMsg->event, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
}


/*===================================================================
  �� �� ���� OnMessage
  ��    �ܣ� CRecPauseState״̬ʱ����Ϣ���պ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecPauseState::OnMessage(CMessage *const pcMsg)
{
	switch (pcMsg->event)
	{
	case MCU_REC_STARTREC_REQ:
		return OnMtStartRecReq(pcMsg);
	
	case MCU_REC_RESUMEREC_REQ:
		m_bIsStartRecReq = FALSE;
		return OnMtResumeRecReq(pcMsg);
		
	case REC_MCU_RESUMEREC_ACK:
		return OnResumeRecReqAck(pcMsg);
		
	case REC_MCU_RESUMEREC_NACK:
		return OnResumeRecReqNack(pcMsg);
		
	case MCU_REC_STOPREC_REQ:
		return OnMtStopRecReq(pcMsg);
	
	case MCU_REC_PAUSEREC_REQ:
		return ReplyMtError(REC_MCU_PAUSEREC_NACK, ERR_REC_CURDOING);
	
	default:
		return CStateBase::OnMessage(pcMsg);
	}
}


/*===================================================================
  �� �� ���� OnResumeRecReqAck
  ��    �ܣ� CRecPauseState״̬ʱ��¼�������ָ�¼���¼����ظ�Ackʱ�Ĵ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecPauseState::OnResumeRecReqAck(CMessage *const pcMsg)
{
	// ���¼��������˿�ʼ¼��,��ظ�REC_MCU_RESUMEREC_ACK
	if (m_bIsStartRecReq)
	{
		GetStateMgr()->PostMsgToMt(REC_MCU_STARTREC_ACK);
	}else
	{
		if(RET_DONE != OnMtReqAck(pcMsg))
		{
			return RET_FAILED;
		}
	}

	// ��¼�������ָ�¼��
	return ChangeStateTo(E_STATE_RECORDING);
}

/*===================================================================
  �� �� ���� OnResumeRecReqNack
  ��    �ܣ� CRecPauseState״̬ʱ��¼�������ָ�¼���¼����ظ�nackʱ�Ĵ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecPauseState::OnResumeRecReqNack(CMessage *const pcMsg)
{
	// ���¼��������˿�ʼ¼��,��ظ�REC_MCU_RESUMEREC_NACK
	if (m_bIsStartRecReq)
	{
		ReplyMtError(REC_MCU_STARTREC_NACK, ERR_REC_ACTFAIL);
	}else
	{
		if(RET_DONE != OnMtReqNack(pcMsg))
		{
			return RET_FAILED;
		}
	}

	// ��¼�������ָ�¼��
	return RET_DONE;
}


/*===================================================================
  �� �� ���� OnMtStartRecReq
  ��    �ܣ� CRecPauseState״̬ʱ�ն�����¼�����Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CRecPauseState::OnMtStartRecReq(CMessage *const pcMsg)
{
	// ��֤��Ϣ��Դ�Ƿ�Ϸ�
	if( !(GetStateMgr()->IsMtReqValid(pcMsg)) )
	{
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "Mt request to Stop Rec failed, for invalid request\n");
		TErrorInfo tError = {0};
		tError.m_wErrCode = ERR_REC_SERVER_INVALID;
		OspPost(pcMsg->srcid, pcMsg->event + 2, &tError, sizeof(tError), pcMsg->srcnode);
		
		return RET_FAILED;
	}
	
	// ��¼���������ͣ¼��
	m_bIsStartRecReq = TRUE;
	return GetStateMgr()->PostMsgToRecChnInst(MCU_REC_RESUMEREC_REQ);
}