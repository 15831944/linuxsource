/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : State.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: Recorder��Server����״̬����
   ����        : 
   �汾        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2010/08/26  1.0         �� ��         ����
******************************************************************************/

#ifndef RECSTATE_H
#define RECSTATE_H

#include "StateBase.h"


class CIdleState : public CStateBase
{
public:
	/**
	/* ���캯��
	*/
    CIdleState(CStateMgr* pMgr) : CStateBase(pMgr, E_STATE_REC_IDLE){}

public:
	/**
	/* ����ʱ����ĳ�ʼ������,���߽���ʱ��ʼ���״̬�Ķ�������
	*/
    virtual BOOL32 Enter();
};

class COnRegisterState : public CStateBase
{
public:
	/**
	/* ���캯��
	*/
    COnRegisterState(CStateMgr* pMgr) : CStateBase(pMgr, E_STATE_REC_ONREG){}

public:
	/**
	/* ������Ϣ����
	*/
    virtual RET_TYPE OnMessage(CMessage *const);
	
private:
	/**
	/* COnRegisterState״̬ʱ��Ӧ�ն�ע������ĺ���
	*/
	RET_TYPE OnMtRegReq(CMessage *const);

	/**
	/* ¼����ظ�ע��Ack����Ӧ����
	*/
	RET_TYPE OnMtRegReqAck(CMessage *const);

	/**
	/* ¼����ظ�ע��Ack����Ӧ����
	*/
	RET_TYPE OnMtRegReqNack(CMessage *const);
};


class CRecReadyState : public CStateBase
{
public:
	/**
	/* ���캯��
	*/
    CRecReadyState(CStateMgr* pMgr) : CStateBase(pMgr, E_STATE_REC_READY){}

public:
    // ����ʱ����ĳ�ʼ������,���߽���ʱ��ʼ���״̬�Ķ�������
    virtual BOOL32 Enter();

    // ������Ϣ����
    virtual RET_TYPE OnMessage(CMessage *const);

private:
	/**
	/* ¼����ظ���ʼ¼��Ackʱ����Ӧ����
	*/	
	RET_TYPE OnMtStartRecAck(CMessage *const);
};


class CRecordingState : public CStateBase
{
public:
	/**
	/* ���캯��
	*/
    CRecordingState(CStateMgr* pMgr) : CStateBase(pMgr, E_STATE_RECORDING){}

public:
	/**
	/* ������Ϣ����
	*/
    virtual RET_TYPE OnMessage(CMessage *const);
	
private:
	/**
	/* �ն�����¼��
	*/
	RET_TYPE OnMtStartRecReq(CMessage *const);
	/**
	/* ¼�������ؼ�֡����Ӧ����
	*/
	RET_TYPE OnRecNeedIFrameCmd(CMessage *const);
};


class CRecPauseState : public CStateBase
{
public:
	/**
	/* ���캯��
	*/
    CRecPauseState(CStateMgr* pMgr) : CStateBase(pMgr, E_STATE_REC_PAUSE){}

public:
	/**
	/* ������Ϣ����
	*/
    virtual RET_TYPE OnMessage(CMessage *const);
	
private:
	/**
	/* �ն�����¼��
	*/
	RET_TYPE OnMtStartRecReq(CMessage *const);

	/**
	/* ¼����Իָ�¼��ָ�Ack
	*/
	RET_TYPE OnResumeRecReqAck(CMessage *const);

	/**
	/* ¼����Իָ�¼��ָ�Nack
	*/
	RET_TYPE OnResumeRecReqNack(CMessage *const);

private:
	/**
	/* ��    ��: �ն��Ƿ��ڴ�״̬�������˿�ʼ¼��ı�־.
	/* ��ϸ˵��: �����ڴ�״̬���ն��������¼��,¼�������������¼�������¼��ָ�������
	/*  ֮��¼�����ظ��ָ�¼���Ack��Nack.��ʱ��Ҫ���ն˻ظ���ʼ¼���Ack��Nack
	*/
	BOOL32 m_bIsStartRecReq;		
};


#endif