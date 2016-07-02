/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : StateBase.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: Recorder��Server��Base״̬����
   ����        : 
   �汾        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2010/08/26  1.0         �� ��         ����
******************************************************************************/


#ifndef STATEBASE_H
#define STATEBASE_H

#include "StateMgr.h"

enum ERecState
{
	E_STATE_REC_IDLE  = 0,					// ����
	E_STATE_REC_ONREG,						// ע�ᴦ��״̬
	E_STATE_REC_READY,						// ¼��׼����
	E_STATE_RECORDING,						// ����¼��
	E_STATE_REC_PAUSE,						// ¼����ͣ״̬

	E_STATE_REC_INVALID  = 0xFFFF,			// �Ƿ�״̬
};

class CStateBase
{    
public:
	// ���캯��
    CStateBase(CStateMgr* pMgr, u32 dwState = 0) : m_pcStateMgr(pMgr), m_dwState(dwState) {}
    // ��������
	~CStateBase(){}
    
public:
    // ����ʱ����ĳ�ʼ������,���߽���ʱ��ʼ���״̬�Ķ�������
    virtual BOOL32 Enter(){return FALSE;}
    // �뿪���״̬ʱ���ƺ���,�����ͷ��������Դ��
    virtual BOOL32 Exit(){return FALSE;}
    // ������Ϣ����
    virtual RET_TYPE OnMessage(CMessage *const);
    
public:
    // ���״̬
    const u32 GetState(){return m_dwState;}
    
protected:
	/**
	/* �ն�����¼�����Ӧ����
	*/
	RET_TYPE OnMtStartRecReq(CMessage *const);

	/**
	/* �ն�������ͣ¼�����Ӧ����
	*/
	RET_TYPE OnMtPauseRecReq(CMessage *const);

	/**
	/* �ն�����ֹͣ¼�����Ӧ����
	*/
	RET_TYPE OnMtStopRecReq(CMessage *const);

	/**
	/* �ն�����ظ�¼�����Ӧ����
	*/
	RET_TYPE OnMtResumeRecReq(CMessage *const);

    // ��StateMgr�ķ�װ,�л�״̬
    RET_TYPE ChangeStateTo(const u32 &dwState);    
    
    // ��ȡStateMgr
    CStateMgr * const GetStateMgr(){return m_pcStateMgr;}
	
	// ¼�������Դ�ڸ��ն˵�����ظ�Ackʱ����Ӧ����
	RET_TYPE OnMtReqAck(CMessage *const);

	// ¼�������Դ�ڸ��ն˵�����ظ�nackʱ����Ӧ����
	RET_TYPE OnMtReqNack(CMessage *const);

	// ��Ϣ�����˴����״̬��ʱ�Ĵ�����
	RET_TYPE OnErrorMsg(CMessage *const);

	// �ظ��ն�Error
	RET_TYPE ReplyMtError(const u16 wEvent, const u16 wErr);

	// �����ǰ״̬��Ϣ
	RET_TYPE OnNodifyEvent(CMessage *const);

private:
	// �����ǰ״̬��Ϣ
	RET_TYPE OnShowStatus(CMessage *const);

	const s8* GetStateStr(const u32 &dwState);

private:
    CStateMgr * const m_pcStateMgr;					// ״̬����������
    u32 m_dwState;									// ����״ֵ̬
};


#endif