/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : StateBase.cpp
   ����ļ�    : StateMgr.cpp, State.cpp��
   �ļ�ʵ�ֹ���: Recorder Server��״̬��Base��ʵ��
   ����        : 
   �汾        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2010/08/26  1.0         �� ��        ����
******************************************************************************/

#include "StateBase.h"

/*===================================================================
  �� �� ���� OnMessage
  ��    �ܣ� ״̬��Base����Ϣ���պ���
  �㷨ʵ�֣� (1). ��״̬���������Ϣ������CStateBase::OnMessage����
  ȫ�ֱ����� 
  ��    ���� pcMsg   : CMessage
  �� �� ֵ�� RET_TYPE,�����RET_TYPE����
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::OnMessage(CMessage *const pcMsg)
{
	// ��ʱû��Ϣ����
	switch (pcMsg->event)
	{
	case EV_RECSERV_SHOW_STATUS:
		return OnShowStatus(pcMsg);

	case REC_MCU_EXCPT_NOTIF:
	case REC_MCU_RECSTATUS_NOTIF:
		return OnNodifyEvent(pcMsg);

	default:
		return RET_PASSED;
	}	
}


/*===================================================================
  �� �� ���� OnMtReqAck
  ��    �ܣ� ¼�������Դ�ڸ��ն˵�����ظ�Ackʱ����Ӧ����
  �㷨ʵ�֣� (1). ���жϴ���Ϣ�Ƿ���¼����ĺϷ���Ϣ
			 (2). ����Ϣ��ȥ��CServMsg����Ϣͷ
			 (3). ����Ϣ�����ն�
  ȫ�ֱ����� 
  ��    ���� pcMsg   : CMessage
  �� �� ֵ�� RET_TYPE,�����RET_TYPE����
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::OnMtReqAck(CMessage *const pcMsg)
{
	if (!(GetStateMgr()->IsRecMsgValid(pcMsg)))
	{
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "CRecServInst received un-known msg from recorder\n");
		return RET_FAILED;
	}

	CServMsg cServMsg( pcMsg->content ,pcMsg->length);

	return GetStateMgr()->PostMsgToMt(pcMsg->event, cServMsg.GetMsgBody(), cServMsg.GetServMsgLen());
}


/*===================================================================
  �� �� ���� OnMtReqNack
  ��    �ܣ� ¼�������Դ�ڸ��ն˵�����ظ�nackʱ����Ӧ����
  �㷨ʵ�֣� (1). ���жϴ���Ϣ�Ƿ���¼����ĺϷ���Ϣ
			 (2). ����Ϣ����ȡ����Ų��γ�TErrorInfo
			 (3). ��TErrorInfo���ݷ����ն�
  ȫ�ֱ����� 
  ��    ���� pcMsg   : CMessage
  �� �� ֵ�� RET_TYPE,�����RET_TYPE����
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::OnMtReqNack(CMessage *const pcMsg)
{
	if (!(GetStateMgr()->IsRecMsgValid(pcMsg)))
	{
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "CRecServInst received un-known msg from recorder\n");
		return RET_FAILED;
	}

	CServMsg cServMsg( pcMsg->content ,pcMsg->length);
	return ReplyMtError(pcMsg->event, cServMsg.GetErrorCode());
}

/*===================================================================
  �� �� ���� ReplyMtError
  ��    �ܣ� Nackʱ�ظ��ն�Error
  �㷨ʵ�֣�
  ȫ�ֱ����� 
  ��    ���� 
              wEvent  : Nack�¼���
				wErr  : Error��
  �� �� ֵ�� RET_TYPE,�����RET_TYPE����
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::ReplyMtError(const u16 wEvent, const u16 wErr)
{
	// ���������Ϣ
	RecServLevelLog(RECSERV_LEVEL_LOG_COMMON, "[ReplyMtError]Error Event[%d, %d] occured in State[%d]\n", wEvent, wErr, GetState());

	// ���ô�����Ϣ
	TErrorInfo tErro = {0};
	tErro.m_wErrCode = wErr;
	return GetStateMgr()->PostMsgToMt(wEvent, (u8 *)&tErro, sizeof(tErro));
}

/*===================================================================
  �� �� ���� OnErrorMsg
  ��    �ܣ� ¼�������Դ�ڸ��ն˵�����ظ�nackʱ����Ӧ����
  �㷨ʵ�֣� ��¼�����������һ�㲻����״̬ת��
  ȫ�ֱ����� 
  ��    ���� pcMsg   : CMessage
  �� �� ֵ�� RET_TYPE,�����RET_TYPE����
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::OnErrorMsg(CMessage *const pcMsg)
{
	// ���������Ϣ
	RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "State[%d] received error msg[%d]", GetState(), pcMsg->event);
	return RET_DONE;
}

/*===================================================================
  �� �� ���� ChangeStateTo
  ��    �ܣ� ״̬�л�����
  �㷨ʵ�֣�
  ȫ�ֱ����� 
  ��    ���� dwState   : ��״̬��Enumֵ
  �� �� ֵ�� RET_TYPE,�����RET_TYPE����
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::ChangeStateTo(const u32 &dwState)
{
	if (m_pcStateMgr)
	{
		return m_pcStateMgr->ChangeStateTo(dwState);
	}
	
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "m_pcStateMgr is NULL, please check\n");
	return RET_FAILED;
}

/*===================================================================
  �� �� ���� OnMtStartRecReq
  ��    �ܣ� �ն�����¼�����Ӧ����
  �㷨ʵ�֣� �����߼�����:
			 (1).���ն�����ע���ն��Ƿ�ƥ��.��ƥ����nack
			 (2).����Ϣ����¼���,¼����������¼��ͨ��
			 (3).�ȴ�¼����ظ�
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� void 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::OnMtStartRecReq(CMessage *const pcMsg)
{
	// ��֤��Ϣ��Դ�Ƿ�Ϸ�
	if( !(GetStateMgr()->IsMtReqValid(pcMsg)) )
	{
		// ���������Ϣ
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "CRecServInst received un-authored request\n");
		
		// �ظ�Nack
		TErrorInfo tError = {0};
		tError.m_wErrCode = ERR_REC_SERVER_INVALID;
		OspPost(pcMsg->srcid, pcMsg->event + 2, &tError, sizeof(tError), pcMsg->srcnode);

		return RET_FAILED;
	}

	// ��¼�������¼��
	GetStateMgr()->PostMsgToRecDaemInst(pcMsg->event, pcMsg->content, pcMsg->length);

	return RET_DONE;
}

/*===================================================================
  �� �� ���� OnMtPauseRecReq
  ��    �ܣ� �ն�������ͣ¼�����Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::OnMtPauseRecReq(CMessage *const pcMsg)
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
	return GetStateMgr()->PostMsgToRecChnInst(pcMsg->event, pcMsg->content, pcMsg->length);
}

/*===================================================================
  �� �� ���� OnMtStopRecReq
  ��    �ܣ� �ն�����ֹͣ¼�����Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::OnMtStopRecReq(CMessage *const pcMsg)
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
	
	// ��¼�������ֹͣ¼��
	return GetStateMgr()->PostMsgToRecChnInst(pcMsg->event, pcMsg->content, pcMsg->length);
}

/*===================================================================
  �� �� ���� OnMtResumeRecReq
  ��    �ܣ� �ն�����ظ�¼�����Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::OnMtResumeRecReq(CMessage *const pcMsg)
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
	// ��¼�������ָ�¼��
	return GetStateMgr()->PostMsgToRecChnInst(pcMsg->event, pcMsg->content, pcMsg->length);
}

/*===================================================================
  �� �� ���� OnMtResumeRecReq
  ��    �ܣ� �ն�����ظ�¼�����Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::OnShowStatus(CMessage *const pcMsg)
{
	RecServLog("CRecServInst[%2d] is On %s\n", GETINS(pcMsg->dstid), GetStateStr(GetState()));
	return RET_DONE;
}

/*===================================================================
  �� �� ���� OnNodifyEvent
  ��    �ܣ� ��Nofityת�����ն�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pcMsg : osp��CMessage
  �� �� ֵ�� RET_TYPE 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/09/26    1.0     �� ��      ����
====================================================================*/
RET_TYPE CStateBase::OnNodifyEvent(CMessage *const pcMsg)
{
	// ״̬����,�ظ�Pass
	if (E_STATE_REC_IDLE == GetState())
	{
		return RET_PASSED;
	}

	CServMsg cServMsg( pcMsg->content ,pcMsg->length);
	return GetStateMgr()->PostMsgToMt(pcMsg->event, cServMsg.GetMsgBody(), cServMsg.GetServMsgLen());
}


/*===================================================================
  �� �� ���� GetStateStr
  ��    �ܣ� ����״ֵ̬���ض�Ӧ���ַ�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� dwState : ״ֵ̬,����ERecState
  �� �� ֵ�� ��Ӧ���ַ������� 
---------------------------------------------------------------------
  �޸ļ�¼:
  ��   ��       �汾    �޸���    �޸�����
  2010/08/26    1.0     �� ��      ����
====================================================================*/
const s8* CStateBase::GetStateStr(const u32 &dwState)
{
	switch (dwState)
	{
	case E_STATE_REC_IDLE:
		return "Idle Status";
	case E_STATE_REC_ONREG:
		return "Register Status";
	case E_STATE_REC_READY:
		return "Ready Status";
	case E_STATE_RECORDING:
		return "Recording Status";
	case E_STATE_REC_PAUSE:
		return "Pause Status";
	default:
		return "Invalid Status";
	}
}